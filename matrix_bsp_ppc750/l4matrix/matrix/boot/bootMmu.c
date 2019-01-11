/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: bootMmu.S
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 02 �� 03 ��
**
** ��        ��: ����ת�� 3G ֮��ĵ�ַ֮ǰ��ʼ��ҳ��.
*********************************************************************************************************/
#include <Matrix.h>
#include "../config.h"
//#include "bootScu.h"
/*********************************************************************************************************
  CP15 ����
*********************************************************************************************************/
#define CP15_CONTROL_TEXREMAP                       (1 << 28)
/*********************************************************************************************************
  AUX ����
*********************************************************************************************************/
#define AUX_CTRL_A9_SMP                             (1 <<  6)
#define AUX_CTRL_A9_L1_PREFETCH                     (1 <<  2)
#define AUX_CTRL_A9_L2_PREFETCH                     (1 <<  1)
#define AUX_CTRL_A9_CACHE_MAINTENANCE_BROADCAST     (1 <<  0)
/*********************************************************************************************************
  �궨��
*********************************************************************************************************/
#define  PTE_ENTRY_NUM     256                                          /*  һ�Ŷ���ҳ�������          */
/*********************************************************************************************************
  ��ຯ��
*********************************************************************************************************/
extern VOID    ppcMmuEnable(VOID);
//extern VOID    ppcMmuDisable(VOID);

extern VOID    ppcMmuInvalidateTLBNr(VOID);
extern VOID    ppcMmuInvalidateTLBEA(VOID);

extern VOID    ppc60xICacheEnable(VOID);
extern VOID	   ppc60xDCacheEnable(VOID);
//extern VOID    ppc750DCacheEnable(VOID);
extern VOID    ppc60xICacheDisable(VOID);
extern VOID    ppc60xDCacheDisable(VOID);


/*
 * ����ӵ�extern ����
 */
extern VOID     ppc60xICacheInvalidateAll(VOID);
extern VOID     ppc60xDCacheInvalidateAll(VOID);
extern VOID     ppc60xBranchPredictionDisable(VOID);
extern VOID	    ppc60xBranchPredictionEnable(VOID);
extern VOID     ppc60xBranchPredictorInvalidate(VOID);

//extern VOID    bootArmControlFeatureDisable(UINT32  uiFeature);
//extern VOID    bootArmAuxControlFeatureEnable(UINT32  ulFeature);
//extern VOID    bootArmAuxControlFeatureDisable(UINT32  uiFeature);
//extern VOID    bootArmDCacheV7Disable(VOID);
//extern VOID    bootArmDCacheEnable(VOID);
//extern addr_t  bootArmPrivatePeriphBaseGet(VOID);
//extern UINT32  bootArmCp15MainIdReg(VOID);
//extern ULONG   bootArmCurCpuId(VOID);
extern VOID    start(VOID);                                             /*  ���ַ��ں���              */
/*********************************************************************************************************
  ���������ӽű��е�ҳ�����
*********************************************************************************************************/
extern  ULONG   _pgd_table[];                                            /*  һ��ҳ��                    */
//extern ULONG   _pte_table[];                                            /*  ����ҳ��                    */
extern  MX_PTE_TRANSENTRY _pte_table[];      /*?��Ϊppc����ҳ��Ľṹ*/
/*********************************************************************************************************
** ��������: bootArmErrataFix
** ��������: Fix Arm Errata
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
** ˵������˳�ʼ������ô˺���������ppc��ʱ�ò�����ˣ���ע�͵�
*********************************************************************************************************/
//MX_BTXT_SEC  VOID  bootArmErrataFix (VOID)
//{
//    UINT32  uiCtrl;
//    UINT32  uiCpuId   = bootArmCp15MainIdReg();
//    UINT32  uiScuBase = bootArmPrivatePeriphBaseGet();
//
//    /*
//     *  ARM_ERRATA_764369 (Cortex-A9 only)
//     */
//    if ((uiCpuId & 0xff0ffff0) == 0x410fc090) {
//        uiCtrl = *(volatile UINT32 *)(uiScuBase + 0x30);
//        if (!(uiCtrl & 1)) {
//            *(volatile UINT32 *)(uiScuBase + 0x30) = (uiCtrl | 0x1);
//        }
//    }
//}
/*********************************************************************************************************
  ������λ
*********************************************************************************************************/
#define DOMAIN_FAIL         (0x0)                                       /*  ��������ʧЧ                */
#define DOMAIN_CHECK        (0x1)                                       /*  ����Ȩ�޼��                */
#define DOMAIN_NOCHK        (0x3)                                       /*  ������Ȩ�޼��              */
/*********************************************************************************************************
  �������� (ʹ�õ���������: D2 ���ɷ���, D1 ������Ȩ�޼��, D0 ����Ȩ�޼��
*********************************************************************************************************/
#define DOMAIN_ATTR         ((DOMAIN_FAIL << 4) | (DOMAIN_NOCHK << 2) | (DOMAIN_CHECK))
/*********************************************************************************************************
** ��������: bootPpcMmuGlobalInit
** ��������: MMU Ӳ���ĳ�ʼ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootPpcMmuGlobalInit (VOID)
{
    ppcMmuInvalidateTLBNr();
	ppcMmuInvalidateTLBEA();
	
    //bootArmMmuSetDomain(DOMAIN_ATTR);
    //bootArmMmuSetProcessId(0);

    //bootArmControlFeatureDisable(CP15_CONTROL_TEXREMAP);                /*  Disable TEX remapping       */
    //bootArmMmuV7SetTTBCR(0);                                            /*  Use the 32-bit translation  */
                                                                        /*  system, Always use TTBR0    */
}
/*********************************************************************************************************
** ��������: bootSmpCoreInit (ppc��ʱ�ò���)
** ��������: ��ʼ�� SMP ����
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/

/*********************************************************************************************************
** ��������: bootMmu
** ��������: ����������ڴ�ӳ��
**           ӳ�������ڴ� [256M -- 260M] ---> [256M -- 260M]
** �䡡��  : NONE                        \
** �䡡��  : NONE                         +-> [3G --- 3G+4M]
** ȫ�ֱ���: 
** ����ģ��: 
** ע  ��  : ��ҳ�����÷�Χ�Ӵ˺���ʼ, �����˼���ѡ���̵߳�ҳ��(����΢�ں˵�����)ֹ.
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootPrimaryMmu (VOID)
{
    INT          i, j;
    ULONG        ulPhysAddr;
    const ULONG  ulVir1Addr = 0x10000000;//?256...B�������ַ
    const ULONG  ulVir2Addr = 0xC0000000;//?3.....B
    const INT    iStartPgd1 = (ulVir1Addr >> 20);//?��λתΪMB
    const INT    iStartPgd2 = (ulVir2Addr >> 20);
    INT          iStartPgd;
    INT          iKnMapMB;//?�ں˾���͸������Ĵ�С
    INT          iIoMapMB;
    ULONG        ulTTBR0Val;
    //ULONG        ulTTBR1Val;

    /*
     *  ӳ�� [32MB �ں˾���] + [32MB ����1] + [16MB ����2] + [8MB ����3] + [8MB ����4]
     *
     *  ֮���Է�����������ڴ�ҲҪ������ӳ��, ����ΪҪ�ڱ�ҳ�����÷�Χ��,
     *  ���������뿽�����������ڴ���.
     */
    ulPhysAddr = 0x10000000;//?�����ַ256MB
    iKnMapMB   = 32 + 500;//32 + 16 + 8 + 8;
    for (i = 0; i < iKnMapMB; i++) {
        _pgd_table[iStartPgd1 + i] = (ULONG)&_pte_table[i * PTE_ENTRY_NUM]  //?ȥ����arm��D0�������
                                   | 0x1;                               /*  ����һ��ҳ����ֵ            */
        _pgd_table[iStartPgd2 + i] = (ULONG)&_pte_table[i * PTE_ENTRY_NUM]
                                   | 0x1;                               /*  ������ Page table           */

        for (j = 0; j < PTE_ENTRY_NUM; j++) {

            /*_pte_table[i * PTE_ENTRY_NUM + j] = ulPhysAddr                ���ö���ҳ����ֵ            */
                                              /*| (0  << 11)                nG                          */
                                              /*| (1  << 10)                S                           */
                                              /*| (0  <<  9)                ucAP2                       */
                                              /*| (1  <<  6)                ucTEX                       */
                                              /*| (3  <<  4)                ucAP                        */
                                              /*| (3  <<  2)                ucCB                        */
                                              /*| (0  <<  0)                ucXN                        */
                                              /*| (2);                      ucType ?����arm��ppcһ��    */

            _pte_table[i * PTE_ENTRY_NUM + j].PTE_bRef = 0;
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_bChange = 0;
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_ucPP = 3;//?����Ϊarm��ucAP
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_ucWIMG = 0;//?����Ϊarm��ucCB
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_uiRPN = ulPhysAddr >> 12;
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_ucReserved1 = 2;   //?ucType��ֵ
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_bReserved2 = 0;//?ucEXecִ��λ
            ulPhysAddr += 0x1000;                                       /*  �¸� 4KB ҳ��               */
        }
    }

#if 0
    /*
     *  ֱ�ӽ����� 1 ���ڴ�ӳ�䵽 0x50000000, ��ʹ��΢�ں��ڵ�ҳ��
     */
    {
    const ULONG  ulVir3Addr = 0x50000000;
    const INT    iStartPgd3 = (ulVir3Addr >> 20);
    for (i = 0; i < 512; i++) {                                         /*  ����1 �� 512M               */
        _pgd_table[iStartPgd3 + i] = (ULONG)&_pte_table[(i + 32) * PTE_ENTRY_NUM] /* ����΢�ں˵� 32M   */
                                   | (1 << 5)                           /*  ѡ�� D0 ��                  */
                                   | 0x1;                               /*  ����һ��ҳ����ֵ            */
    }
    }
#endif
    /*
     *  Ϊ L4SylixOS �������� 6M ����ε�ӳ�� 0x30000000
     *  �Ա㴴���÷���ʱ�����븴�ƹ�ȥ
     */
    {
    ulPhysAddr = 0x30000000;
    iStartPgd  = (ulPhysAddr >> 20);
    for (i = 0; i < 6; i++) {                                           /*  ������� 6M                 */
        _pgd_table[iStartPgd + i] = (ULONG)&_pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM] /* ����֮ǰ�� 532M */
                                   | 0x1;                               /*  ����һ��ҳ����ֵ            */

        for (j = 0; j < PTE_ENTRY_NUM; j++) {


            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bRef = 0;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bChange = 0;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucPP = 3;//?����Ϊarm��ucAP
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucWIMG = 0;//?����Ϊarm��ucCB
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_uiRPN = ulPhysAddr >> 12;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucReserved1 = 2;   //?ucType��ֵ
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bReserved2 = 0;//?ucEXecִ��λ
            ulPhysAddr += 0x1000;                                       /*  �¸� 4KB ҳ��               */
        } 
    }
    iKnMapMB += 12;
    }

    /*
     *  ӳ�� 247MB ���⹦�ܼĴ���
     */
    ulPhysAddr = 0x900000;
    iIoMapMB   = 247;
    iStartPgd  = (ulPhysAddr >> 20);
    for (i = 0; i < iIoMapMB; i++) {
        _pgd_table[iStartPgd + i] = (ULONG)&_pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM]
                                  | 0x1;                                /*  ����һ��ҳ����ֵ            */
        for (j = 0; j < PTE_ENTRY_NUM; j++) {
            /*_pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j] = ulPhysAddr /* | 0x432; */
                                                        /* | (0  << 11)     nG                          */
                                                        /* | (1  << 10)     S                           */
                                                        /* | (0  <<  9)     ucAP2                       */
                                                        /* | (0  <<  6)     ucTEX                       */
                                                        /* | (3  <<  4)     ucAP                        */
                                                        /* | (0  <<  2)     ucCB                        */
                                                        /* | (0  <<  0)     ucXN                        */
                                                        /* | (2);           ucType                      */
                                                                        /*  ���ö���ҳ����ֵ            */

            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bRef = 0;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bChange = 0;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucPP = 3;//?����Ϊarm��ucAP
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucWIMG = 0;//?����Ϊarm��ucCB
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_uiRPN = ulPhysAddr >> 12;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucReserved1 = 2;   //?ucType��ֵ
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bReserved2 = 0;//?ucEXecִ��λ
            ulPhysAddr += 0x1000;                                       /*  �¸� 4KB ҳ��               */
        }
    }

    /*
     *  Set location of level 1 page table
     * ------------------------------------
     *  31:14 - Base addr
     *  13:7  - 0x0
     *  6     - IRGN[0]     0x1 (Normal memory, Inner Write-Back Write-Allocate Cacheable)
     *  5     - NOS         0x0 (Outer Shareable)
     *  4:3   - RGN         0x1 (Normal memory, Outer Write-Back Write-Allocate Cacheable)
     *  2     - IMP         0x0
     *  1     - S           0x1 (Shareable)
     *  0     - IRGN[1]     0x0 (Normal memory, Inner Write-Back Write-Allocate Cacheable)
     */
    /*ulTTBR0Val = ulTTBR1Val = ((ULONG)_pgd_table
                            | (1 << 6)
                            | (0 << 5)
                            | (1 << 3)
                            | (0 << 2)
                            | (1 << 1)
                            | (0 << 0));*/

//    ULONG   ulCPUId = MX_CPU_GET_CUR_ID();
//
//    _G_pMmuContext[ulCPUId] = pmmuctx;     //ppc����MMU��ǰ�����ĵķ�ʽ,�ο�΢�ں�

    bootPpcMmuGlobalInit();


    //bootSmpCoreInit();

    //bootArmMmuSetTTBase0(ulTTBR0Val);                                   /*  ����ҳ���ַ�� TT �Ĵ���    */
     //bootArmMmuSetTTBase1(ulTTBR1Val);
    KN_SMP_MB();

    ppc60xDCacheEnable();
//    ppc750DCacheEnable();
    ppcMmuEnable();                                                 /*  ʹ�� MMU                    */

    start();                                                            /*  ��ת�� 3G ֮������ַ����  */
}
/*********************************************************************************************************
** ��������: bootSecondaryMmu
** ��������: ���ôӺ˵�ҳ���ַ, ʹ�ܴӺ˵� MMU
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
** ע  ��  : ��ҳ�����÷�Χ�Ӵ˺���ʼ, �����˼���ѡ���̵߳�ҳ��(����΢�ں˵�����)ֹ.
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootSecondaryMmu (VOID)
{
    /*ULONG        ulTTBR0Val;
    ULONG        ulTTBR1Val;

    bootArmMmuGlobalInit();
    bootSmpCoreInit();

    ulTTBR0Val = ulTTBR1Val = ((ULONG)_pgd_table
                            | (1 << 6)
                            | (0 << 5)
                            | (1 << 3)
                            | (0 << 2)
                            | (1 << 1)
                            | (0 << 0));

    bootArmMmuSetTTBase0(ulTTBR0Val);                                   /*  ����ҳ���ַ�� TT �Ĵ���    */
    /*bootArmMmuSetTTBase1(ulTTBR1Val);
    KN_SMP_MB();

    bootArmDCacheEnable();

    bootArmMmuEnable();                                                 /*  ʹ�� MMU                    */

    start();                                                            /*  ��ת�� 3G ֮������ַ����  */
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

