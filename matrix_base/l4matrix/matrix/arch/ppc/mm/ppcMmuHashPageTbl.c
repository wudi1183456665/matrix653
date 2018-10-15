/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: ppcMmuHashPageTbl.c
**
** ��   ��   ��: Dang.YueDong (��Ծ��)
**
** �ļ���������: 2016 �� 01 �� 15 ��
**
** ��        ��: PowerPC ��ϵ���� MMU Hashed ҳ������.
*********************************************************************************************************/
#define  __MATRIX_STDIO
#define  __MXTRIX_KERNEL
#include "Matrix.h"
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
#include "./ppcMmuHashPageTbl.h"
/*********************************************************************************************************
  �ⲿ�ӿ�����
*********************************************************************************************************/
extern VOID  ppcMmuSetSR(UINT32  uiSRn, UINT32  uiValue);
extern VOID  ppcMmuSetSDR1(UINT32);
extern VOID  ppcHashPageTblPteSet(PTE    *pPte,
                                  UINT32  uiWord0,
                                  UINT32  uiWord1,
                                  UINT32  uiEffectiveAddr);
/*********************************************************************************************************
  ȫ�ֱ�������
*********************************************************************************************************/
static SR       _G_SRs[16];
static UINT32   _G_uiHashPageTblOrg;
static UINT32   _G_uiHashPageTblMask;
static MX_SPINLOCK_DEFINE_CACHE_ALIGN(_G_slHashPageTblLock);

#define __MMU_PTE_DEBUG
#ifdef __MMU_PTE_DEBUG
static UINT     _G_uiPtegNr = 0;
static UINT     _G_uiPteMissCounter = 0;
#endif                                                                  /*  defined(__MMU_PTE_DEBUG)    */
/*********************************************************************************************************
** ��������: ppcMmuPtegAddrGet
** ��������: ͨ����Ч��ַ��� PTEG
** �䡡��  : effectiveAddr           ��Ч��ַ
**           ppPrimPteg              �� PTEG
**           ppSecPteg               �� PTEG
**           puiAPI                  API
**           puiVSID                 VSID
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  ppcMmuPtegAddrGet (EA       effectiveAddr,
                                PTEG   **ppPrimPteg,
                                PTEG   **ppSecPteg,
                                UINT32  *puiAPI,
                                UINT32  *puiVSID)
{
    UINT32  uiPageIndex;
    UINT32  uiHashValue1;
    UINT32  uiHashValue2;
    UINT32  uiHashValue1L;
    UINT32  uiHashValue1H;
    UINT32  uiHashValue2L;
    UINT32  uiHashValue2H;

    *puiVSID      = _G_SRs[effectiveAddr.field.EA_ucSRn].field.SR_uiVSID;
    uiPageIndex   = effectiveAddr.field.EA_uiPageIndex;
    *puiAPI       = (uiPageIndex & MMU_EA_API) >> MMU_EA_API_SHIFT;

    uiHashValue1  = (*puiVSID & MMU_VSID_PRIM_HASH) ^ uiPageIndex;
    uiHashValue2  = ~uiHashValue1;

    uiHashValue1L = (uiHashValue1 & MMU_HASH_VALUE_LOW)  << MMU_PTE_HASH_VALUE_LOW_SHIFT;
    uiHashValue1H = (uiHashValue1 & MMU_HASH_VALUE_HIGH) >> MMU_HASH_VALUE_HIGH_SHIFT;

    uiHashValue2L = (uiHashValue2 & MMU_HASH_VALUE_LOW)  << MMU_PTE_HASH_VALUE_LOW_SHIFT;
    uiHashValue2H = (uiHashValue2 & MMU_HASH_VALUE_HIGH) >> MMU_HASH_VALUE_HIGH_SHIFT;

    uiHashValue1H = (uiHashValue1H & _G_uiHashPageTblMask) << MMU_PTE_HASH_VALUE_HIGH_SHIFT;
    uiHashValue2H = (uiHashValue2H & _G_uiHashPageTblMask) << MMU_PTE_HASH_VALUE_HIGH_SHIFT;

    *ppPrimPteg   = (PTEG *)(_G_uiHashPageTblOrg | uiHashValue1H | uiHashValue1L);
    *ppSecPteg    = (PTEG *)(_G_uiHashPageTblOrg | uiHashValue2H | uiHashValue2L);
}
/*********************************************************************************************************
** ��������: ppcMmuHashPageTblInit
** ��������: ��ʼ�� Hashed ҳ��
** �䡡��  : uiMemSize               �ڴ��С
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  ppcMmuHashPageTblInit (UINT32  uiMemSize)
{
    UINT32  stHashPageTblSize;
    PVOID   pvHashPageTblAddr;
    INT     i;

    spinInit(&_G_slHashPageTblLock);

    /*
     * ���� 16 ���μĴ������� VA = EA
     */
    for (i = 0; i < 16; i++) {
        _G_SRs[i].SR_uiValue      = 0;
        _G_SRs[i].field.SR_bT     = 0;
        _G_SRs[i].field.SR_bKS    = 1;
        _G_SRs[i].field.SR_bKP    = 1;
        _G_SRs[i].field.SR_bN     = 0;
        _G_SRs[i].field.SR_uiVSID = i;
        ppcMmuSetSR(i, _G_SRs[i].SR_uiValue);
    }

    if (uiMemSize < MMU_PTE_MIN_SIZE_8M) {
        return  (PX_ERROR);
    }

    if (uiMemSize >= MMU_PTE_MIN_SIZE_4G) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_4G;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_4G;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_4G;

    } else if (uiMemSize >= MMU_PTE_MIN_SIZE_2G) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_2G;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_2G;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_2G;

    } else if (uiMemSize >= MMU_PTE_MIN_SIZE_1G) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_1G;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_1G;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_1G;

    } else if (uiMemSize >= MMU_PTE_MIN_SIZE_512M) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_512M;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_512M;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_512M;

    } else if (uiMemSize >= MMU_PTE_MIN_SIZE_256M) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_256M;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_256M;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_256M;

    } else if (uiMemSize >= MMU_PTE_MIN_SIZE_128M) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_128M;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_128M;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_128M;

    } else if (uiMemSize >= MMU_PTE_MIN_SIZE_64M) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_64M;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_64M;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_64M;

    } else if (uiMemSize >= MMU_PTE_MIN_SIZE_32M) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_32M;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_32M;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_32M;

    } else if (uiMemSize >= MMU_PTE_MIN_SIZE_16M) {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_16M;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_16M;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_16M;

    } else {
        _G_uiHashPageTblOrg  = MMU_SDR1_HTABORG_8M;
        _G_uiHashPageTblMask = MMU_SDR1_HTABMASK_8M;
        stHashPageTblSize    = MMU_PTE_MIN_SIZE_8M;
    }

    /*
     * Hashed ҳ�����ڵ��ڴ���ڴ�������� M λ����Ϊ 1 (��Ӳ����֤�ڴ�һ����)
     */
    pvHashPageTblAddr = __KHEAP_ALLOC_ALIGN(stHashPageTblSize, stHashPageTblSize);
    if (pvHashPageTblAddr) {
        lib_bzero(pvHashPageTblAddr, stHashPageTblSize);

        _G_uiHashPageTblOrg &= (UINT32)pvHashPageTblAddr;

        ppcMmuSetSDR1(_G_uiHashPageTblOrg | _G_uiHashPageTblMask);

#ifdef __MMU_PTE_DEBUG
        _G_uiPtegNr = stHashPageTblSize / 64;
#endif                                                                  /*  defined(__MMU_PTE_DEBUG)    */
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: ppcHashPageTblSearchInvalidPte
** ��������: ������Ч PTE
** �䡡��  : pPteg                 PTEG
** �䡡��  : PTE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static PTE  *ppcHashPageTblSearchInvalidPte (PTEG  *pPteg)
{
    UINT  uiIndex;
    PTE  *pPte;

    for (uiIndex = 0; uiIndex < MMU_PTES_IN_PTEG; ++uiIndex) {
        pPte = &pPteg->PTEG_PTEs[uiIndex];
        if (!pPte->field.PTE_bV) {
            return  (pPte);
        }
    }

    return  (MX_NULL);
}
/*********************************************************************************************************
** ��������: ppcHashPageTblSearchEliminatePte
** ��������: ǿ����̭һ�� PTE
** �䡡��  : pPrimPteg             �� PTEG
**           pSecPteg              �� PTEG
**           pbIsPrimary           �Ƿ��� PTE
** �䡡��  : PTE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static PTE  *ppcHashPageTblSearchEliminatePte (PTEG  *pPrimPteg,
                                               PTEG  *pSecPteg,
                                               BOOL  *pbIsPrimary)
{
    UINT  uiIndex;
    PTE  *pPte;

    for (uiIndex = 0; uiIndex < MMU_PTES_IN_PTEG; ++uiIndex) {
        pPte = &pPrimPteg->PTEG_PTEs[uiIndex];
        if (!pPte->field.PTE_bR) {
            *pbIsPrimary = MX_TRUE;
            return  (pPte);
        }
    }

    for (uiIndex = 0; uiIndex < MMU_PTES_IN_PTEG; ++uiIndex) {
        pPte = &pSecPteg->PTEG_PTEs[uiIndex];
        if (!pPte->field.PTE_bR) {
            *pbIsPrimary = MX_FALSE;
            return  (pPte);
        }
    }

    for (uiIndex = 0; uiIndex < MMU_PTES_IN_PTEG; ++uiIndex) {
        pPte = &pPrimPteg->PTEG_PTEs[uiIndex];
        if (!pPte->field.PTE_bC) {
            *pbIsPrimary = MX_TRUE;
            return  (pPte);
        }
    }

    for (uiIndex = 0; uiIndex < MMU_PTES_IN_PTEG; ++uiIndex) {
        pPte = &pSecPteg->PTEG_PTEs[uiIndex];
        if (!pPte->field.PTE_bC) {
            *pbIsPrimary = MX_FALSE;
            return  (pPte);
        }
    }

    pPte = &pPrimPteg->PTEG_PTEs[0];
    *pbIsPrimary = MX_TRUE;

    return  (pPte);
}
/*********************************************************************************************************
** ��������: ppcHashPageTblPteAdd
** ��������: ����һ�� PTE �� Hash ҳ��
** �䡡��  : pPte                  PTE
**           effectiveAddr         ��Ч��ַ
**           uiPteValue1           PTE ֵ1
**           uiAPI                 API
**           uiVSID                VSID
**           bR                    Referenced bit
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  ppcHashPageTblPteAdd (PTE    *pPte,
                                   EA      effectiveAddr,
                                   UINT32  uiPteValue1,
                                   BOOL    bIsPrimary,
                                   UINT32  uiAPI,
                                   UINT32  uiVSID,
                                   BOOL    bR)
{
    PTE     pteTemp;

    pteTemp.words.PTE_uiWord0 = 0;
    pteTemp.words.PTE_uiWord1 = 0;

    pteTemp.field.PTE_uiVSID  = uiVSID;
    pteTemp.field.PTE_bH      = !bIsPrimary;
    pteTemp.field.PTE_ucAPI   = uiAPI;
    pteTemp.field.PTE_bV      = 1;
    pteTemp.field.PTE_bR      = bR;
    ppcHashPageTblPteSet(pPte,
                         pteTemp.words.PTE_uiWord0,
                         uiPteValue1,
                         effectiveAddr.EA_uiValue);
}
/*********************************************************************************************************
** ��������: ppcHashPageTblSearchPteByEA
** ��������: ͨ����Ч��ַ���� PTE
** �䡡��  : pPteg                 PTEG
**           effectiveAddr         ��Ч��ַ
**           uiAPI                 API
**           uiVSID                VSID
** �䡡��  : PTE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static PTE  *ppcHashPageTblSearchPteByEA (PTEG   *pPteg,
                                          EA      effectiveAddr,
                                          UINT32  uiAPI,
                                          UINT32  uiVSID)
{
    UINT    uiIndex;
    PTE    *pPte;

    for (uiIndex = 0; uiIndex < MMU_PTES_IN_PTEG; ++uiIndex) {
        pPte = &pPteg->PTEG_PTEs[uiIndex];
        if ((pPte->field.PTE_bV) &&
            (pPte->field.PTE_uiVSID == uiVSID) &&
            (pPte->field.PTE_ucAPI  == uiAPI)) {
            return  (pPte);
        }
    }

    return  (MX_NULL);
}
/*********************************************************************************************************
** ��������: ppcMmuHashPageTblMakeTrans
** ��������: ����ӳ���ϵ
** �䡡��  : ulEffectiveAddr       ��Ч��ַ
**           uiPteValue1           PTE ֵ1
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ppcMmuHashPageTblMakeTrans (addr_t  ulEffectiveAddr,
                                  UINT32  uiPteValue1)
{
    INTREG  iregInterLevel;
    PTEG   *pPrimPteg;
    PTEG   *pSecPteg;
    PTE    *pPte;
    BOOL    bIsPrimary = MX_TRUE;
    EA      effectiveAddr;
    UINT32  uiAPI;
    UINT32  uiVSID;

    effectiveAddr.EA_uiValue = ulEffectiveAddr;
    ppcMmuPtegAddrGet(effectiveAddr,
                      &pPrimPteg,
                      &pSecPteg,
                      &uiAPI,
                      &uiVSID);

    MX_SPIN_LOCK_QUICK(&_G_slHashPageTblLock, &iregInterLevel);

    /*
     * ���� EA �� PTE
     */
    pPte = ppcHashPageTblSearchPteByEA(pPrimPteg,
                                       effectiveAddr,
                                       uiAPI,
                                       uiVSID);
    if (!pPte) {
        pPte = ppcHashPageTblSearchPteByEA(pSecPteg,
                                           effectiveAddr,
                                           uiAPI,
                                           uiVSID);
    }

    if (pPte) {
        /*
         * �ҵ���
         */
        if (uiPteValue1) {
            /*
             * �ı� PTE
             */
            ppcHashPageTblPteSet(pPte,
                                 pPte->words.PTE_uiWord0,
                                 uiPteValue1,
                                 ulEffectiveAddr);
        } else {
            /*
             * ɾ�� PTE
             */
            ppcHashPageTblPteSet(pPte,
                                 0,
                                 0,
                                 ulEffectiveAddr);
        }
    } else {
        if (uiPteValue1) {
            /*
             * û���ҵ�������һ����Ч�� PTE
             */
            pPte = ppcHashPageTblSearchInvalidPte(pPrimPteg);
            if (!pPte) {
                pPte = ppcHashPageTblSearchInvalidPte(pSecPteg);
                if (pPte) {
                    bIsPrimary = MX_FALSE;
                }
            }

            if (pPte) {
                /*
                 * �ҵ���Ч�� PTE������һ�� PTE
                 */
                ppcHashPageTblPteAdd(pPte,
                                     effectiveAddr,
                                     uiPteValue1,
                                     bIsPrimary,
                                     uiAPI,
                                     uiVSID,
                                     MX_FALSE);
            } else {
                /*
                 * û����Ч�� PTE���ⲿ����Ч TLB������ǿ����̭ PTE��������� MISS
                 */
            }
        } else {
            /*
             * û����Ч�� PTE���ⲿ����Ч TLB������ǿ����̭ PTE��������� MISS
             */
        }
    }

    MX_SPIN_UNLOCK_QUICK(&_G_slHashPageTblLock, iregInterLevel);
}
/*********************************************************************************************************
** ��������: ppcMmuHashPageTblFlagSet
** ��������: ����ӳ���־
** �䡡��  : ulEffectiveAddr       ��Ч��ַ
**           uiPteValue1           PTE ֵ1
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: 
** ˵  ��  : ���û���ҵ����ⲿ����Ч TLB������������Ч PTE ��ǿ����̭ PTE��������� MISS
*********************************************************************************************************/
VOID  ppcMmuHashPageTblFlagSet (addr_t  ulEffectiveAddr,
                                UINT32  uiPteValue1)
{
    INTREG  iregInterLevel;
    PTEG   *pPrimPteg;
    PTEG   *pSecPteg;
    PTE    *pPte;
    EA      effectiveAddr;
    UINT32  uiAPI;
    UINT32  uiVSID;

    effectiveAddr.EA_uiValue = ulEffectiveAddr;
    ppcMmuPtegAddrGet(effectiveAddr,
                      &pPrimPteg,
                      &pSecPteg,
                      &uiAPI,
                      &uiVSID);

    MX_SPIN_LOCK_QUICK(&_G_slHashPageTblLock, &iregInterLevel);

    /*
     * ���� EA �� PTE
     */
    pPte = ppcHashPageTblSearchPteByEA(pPrimPteg,
                                       effectiveAddr,
                                       uiAPI,
                                       uiVSID);
    if (!pPte) {
        pPte = ppcHashPageTblSearchPteByEA(pSecPteg,
                                           effectiveAddr,
                                           uiAPI,
                                           uiVSID);
    }

    if (pPte) {
        /*
         * �ҵ���
         */
        if (uiPteValue1) {
            /*
             * �ı� PTE
             */
            ppcHashPageTblPteSet(pPte,
                                 pPte->words.PTE_uiWord0,
                                 uiPteValue1,
                                 ulEffectiveAddr);
        } else {
            /*
             * ɾ�� PTE
             */
            ppcHashPageTblPteSet(pPte,
                                 0,
                                 0,
                                 ulEffectiveAddr);
        }
    } else {
        /*
         * û���ҵ����ⲿ����Ч TLB������������Ч PTE ��ǿ����̭ PTE��������� MISS
         */
    }

    MX_SPIN_UNLOCK_QUICK(&_G_slHashPageTblLock, iregInterLevel);
}
/*********************************************************************************************************
** ��������: ppcMmuHashPageTblPteMiss
** ��������: ���� PTE MISS
** �䡡��  : ulEffectiveAddr       ��Ч��ַ
**           uiPteValue1           PTE ֵ1
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ppcMmuHashPageTblPteMiss (addr_t  ulEffectiveAddr,
                                UINT32  uiPteValue1)
{
    INTREG  iregInterLevel;
    PTEG   *pPrimPteg;
    PTEG   *pSecPteg;
    PTE    *pPte;
    BOOL    bIsPrimary = MX_TRUE;
    EA      effectiveAddr;
    UINT32  uiAPI;
    UINT32  uiVSID;

    effectiveAddr.EA_uiValue = ulEffectiveAddr;
    ppcMmuPtegAddrGet(effectiveAddr,
                      &pPrimPteg,
                      &pSecPteg,
                      &uiAPI,
                      &uiVSID);

    MX_SPIN_LOCK_QUICK(&_G_slHashPageTblLock, &iregInterLevel);

#ifdef __MMU_PTE_DEBUG
    _G_uiPteMissCounter++;
#endif                                                                  /*  defined(__MMU_PTE_DEBUG)    */

    /*
     * ����һ����Ч�� PTE
     */
    pPte = ppcHashPageTblSearchInvalidPte(pPrimPteg);
    if (!pPte) {
        pPte = ppcHashPageTblSearchInvalidPte(pSecPteg);
        if (pPte) {
            bIsPrimary = MX_FALSE;
        }
    }

    if (!pPte) {
        /*
         * ǿ����̭һ�� PTE
         */
        pPte = ppcHashPageTblSearchEliminatePte(pPrimPteg,
                                                pSecPteg,
                                                &bIsPrimary);

        /*
         * ���ﲢ����Ч����̭�� PTE ��Ӧ�� TLB
         * See << programming_environment_manual >> Figure 7-17 and Figure 7-26
         */
    }

    /*
     * ����һ�� PTE
     */
    ppcHashPageTblPteAdd(pPte,
                         effectiveAddr,
                         uiPteValue1,
                         bIsPrimary,
                         uiAPI,
                         uiVSID,
                         MX_FALSE);

    MX_SPIN_UNLOCK_QUICK(&_G_slHashPageTblLock, iregInterLevel);
}
/*********************************************************************************************************
** ��������: ppcMmuHashPageTblPtePreLoad
** ��������: PTE Ԥ����
** �䡡��  : ulEffectiveAddr       ��Ч��ַ
**           uiPteValue1           PTE ֵ1
** �䡡��  : ERROR CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ppcMmuHashPageTblPtePreLoad (addr_t  ulEffectiveAddr,
                                   UINT32  uiPteValue1)
{
    INTREG  iregInterLevel;
    PTEG   *pPrimPteg;
    PTEG   *pSecPteg;
    PTE    *pPte;
    BOOL    bIsPrimary = MX_TRUE;
    EA      effectiveAddr;
    UINT32  uiAPI;
    UINT32  uiVSID;

    effectiveAddr.EA_uiValue = ulEffectiveAddr;
    ppcMmuPtegAddrGet(effectiveAddr,
                      &pPrimPteg,
                      &pSecPteg,
                      &uiAPI,
                      &uiVSID);

    MX_SPIN_LOCK_QUICK(&_G_slHashPageTblLock, &iregInterLevel);

    /*
     * ���� EA �� PTE
     */
    pPte = ppcHashPageTblSearchPteByEA(pPrimPteg,
                                       effectiveAddr,
                                       uiAPI,
                                       uiVSID);
    if (!pPte) {
        pPte = ppcHashPageTblSearchPteByEA(pSecPteg,
                                           effectiveAddr,
                                           uiAPI,
                                           uiVSID);
    }

    if (!pPte) {
        /*
         * ����һ����Ч�� PTE
         */
        pPte = ppcHashPageTblSearchInvalidPte(pPrimPteg);
        if (!pPte) {
            pPte = ppcHashPageTblSearchInvalidPte(pSecPteg);
            if (pPte) {
                bIsPrimary = MX_FALSE;
            }
        }

        if (!pPte) {
            /*
             * ǿ����̭һ�� PTE
             */
            pPte = ppcHashPageTblSearchEliminatePte(pPrimPteg,
                                                    pSecPteg,
                                                    &bIsPrimary);

            /*
             * ���ﲢ����Ч����̭�� PTE ��Ӧ�� TLB
             * See << programming_environment_manual >> Figure 7-17 and Figure 7-26
             */
        }

        /*
         * ����һ�� PTE
         */
        ppcHashPageTblPteAdd(pPte,
                             effectiveAddr,
                             uiPteValue1,
                             bIsPrimary,
                             uiAPI,
                             uiVSID,
                             MX_TRUE);                                  /*  ������������̭              */
    }

    MX_SPIN_UNLOCK_QUICK(&_G_slHashPageTblLock, iregInterLevel);
}
/*********************************************************************************************************
** ��������: ppcMmuHashPageTblShow
** ��������: ��ӡ MMU HASH PAGE ��Ϣ
** �䡡��  : NONE
** �䡡��  : ERROR_NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#ifdef __MMU_PTE_DEBUG

INT  ppcMmuHashPageTblShow (VOID)
{
    PTEG   *pPteg = (PTEG *)_G_uiHashPageTblOrg;
    INT     i, j;
    INT     iCount;
    INT     iNewLine = 0;

    printf("PTEG usage:\n");

    for (i = 0; i < _G_uiPtegNr; i++, pPteg++) {
        iCount = 0;
        for (j = 0; j < 8; j++) {
            if (pPteg->PTEG_PTEs[j].field.PTE_bV) {
                iCount++;
            }
        }

        if (iCount) {
            printf("%5d-%d ", i, iCount);
            if (++iNewLine % 10 == 0) {
                printf("\n");
            }
        }
    }

    printf("\n\nPTE Miss Counter = %d\n", _G_uiPteMissCounter);
    printf("PTEG Number\t = %d\n", _G_uiPtegNr);

    return  (ERROR_NONE);
}

#endif                                                                  /*  defined(__MMU_PTE_DEBUG)    */
/*********************************************************************************************************
  END
*********************************************************************************************************/
