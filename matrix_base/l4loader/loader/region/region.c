/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: region.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 11 �� 06 ��
**
** ��        ��: region ����.
*********************************************************************************************************/
#include <loader.h>

/*********************************************************************************************************
  ��غ궨��
*********************************************************************************************************/
#define __MEM_ADDR_MASK     ((1UL << 10) - 1)                           /*  (KIP) �ڴ���������ַ����    */
#define __MEM_ALIGN         (1UL << 12)                                 /*  �ڴ��ַ���� (4K)           */
#define __MEM_ADDR_ALIGN(a) (((a) + __MEM_ALIGN - 1) & ~(__MEM_ALIGN - 1))
                                                                        /*  �����ڴ����϶����ַ        */
/*********************************************************************************************************
  ȫ�ֱ���
*********************************************************************************************************/
static REGION    _G_rgKipMem[100];                                      /*  �ڴ�����������              */
static UINT      _G_uiMemCount;                                         /*  ��������Ч�ڴ�������        */
/*********************************************************************************************************
  �������ӽű�����
*********************************************************************************************************/
extern CHAR      _loader_start[];                                       /*  Loader ������ʼ��ַ         */
extern CHAR      _loader_end[];                                         /*  Loader ���������ַ         */

/*********************************************************************************************************
** ��������: __regionFind
** ��������: �ҳ�ָ���ڴ����ȫ���ڴ����������е�����
** �䡡��  : prgRequest     Ҫ���ҵ��ڴ��ָ��
** �䡡��  : ����ָ���ڴ�������ֵ, ���û�ҵ�, ���� _G_uiMemCount
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static UINT  __regionFind (PREGION  prgRequest)
{
    UINT      i;
    UINT64    ui64CurrentEndAddr;                                       /*  ��ǰ�ڴ���������ַ          */
    UINT64    ui64RequestEndAddr;                                       /*  ������������ַ              */

    for (i = 0; i < _G_uiMemCount; i++) {
        if (_G_rgKipMem[i].RG_uiType != MEMORY_TYPE_CONVENTIONAL) {     /*  �ǿ����ڴ���                */
            continue;
        }

        ui64CurrentEndAddr = (UINT64)_G_rgKipMem[i].RG_ulAddr + _G_rgKipMem[i].RG_stSize;
        ui64RequestEndAddr = (UINT64)prgRequest->RG_ulAddr + prgRequest->RG_stSize;
        if (prgRequest->RG_ulAddr >= _G_rgKipMem[i].RG_ulAddr &&
            ui64RequestEndAddr <= ui64CurrentEndAddr) {                 /*  ���ڴ�����ȫ����������      */
            break;
        }
    }

    return  (i);
}
/*********************************************************************************************************
** ��������: __regionMergeToPrev
** ��������: ���Խ���������ǰһ�ڴ����ϲ�
** �䡡��  : uiContain      �������������ڴ�������
**           prgRequest     ������ָ��
** �䡡��  : �µİ������������ڴ�������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static UINT  __regionMergeToPrev (UINT  uiContain, PREGION  prgRequest)
{
    UINT64     ui64PrevEndAddr;                                         /*  ǰһ�ڴ���������ַ          */
    PREGION    prgContain  = &_G_rgKipMem[uiContain];
    PREGION    prgPrevious = prgContain - 1;
    UINT       i;

    if (uiContain == 0) {                                               /*  �������ǵ�һ��Ԫ��          */
        return  (uiContain);
    }

    ui64PrevEndAddr = (UINT64)prgPrevious->RG_ulAddr + prgPrevious->RG_stSize;
    if ((prgRequest->RG_ulAddr == ui64PrevEndAddr) &&                   /*  ��������ǰһ�ڴ�������      */
        (prgRequest->RG_uiType == prgPrevious->RG_uiType)) {            /*  ����������ͬ                */

        prgPrevious->RG_stSize += prgContain->RG_stSize;                /*  ǰһ�ڴ����Ͱ������ϲ�      */
        for (i = uiContain; i < _G_uiMemCount - 1; i++) {               /*  ��������֮���Ԫ��ǰ��      */
            _G_rgKipMem[i] = _G_rgKipMem[i + 1];
        }
        uiContain--;                                                    /*  ������������һ              */

        _G_uiMemCount--;                                                /*  �ڴ���������һ              */
    }

    return  (uiContain);
}
/*********************************************************************************************************
** ��������: __regionApartFront
** ��������: ���������ָ�Ϊ������: ǰ�벿����, ��벿������������ͷ
** �䡡��  : uiContain      �������������ڴ�������
**           prgRequest     ������ָ��
** �䡡��  : �µİ������������ڴ�������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static UINT  __regionApartFront (UINT  uiContain, PREGION  prgRequest)
{
    PREGION    prgContain = &_G_rgKipMem[uiContain];
    PREGION    prgNew;
    UINT       i;

    for (i = _G_uiMemCount; i > uiContain; i--) {                       /*  ����������֮���Ԫ�غ���    */
        _G_rgKipMem[i] = _G_rgKipMem[i - 1];
    }

    prgNew            = &_G_rgKipMem[uiContain];                        /*  ���ڴ�����ԭ������λ��      */
    prgNew->RG_uiType = MEMORY_TYPE_CONVENTIONAL;                       /*  ���ڴ�����Ϊ�ɷ�������      */
    prgNew->RG_stSize = prgRequest->RG_ulAddr - prgContain->RG_ulAddr;

    uiContain++;                                                        /*  ������������һ              */
    prgContain            = &_G_rgKipMem[uiContain];
    prgContain->RG_ulAddr = prgRequest->RG_ulAddr;
    prgContain->RG_stSize = prgContain->RG_stSize - prgNew->RG_stSize;

    _G_uiMemCount++;                                                    /*  �ڴ���������һ              */

    return  (uiContain);
}
/*********************************************************************************************************
** ��������: __regionMergeToNext
** ��������: ���Խ��������ͺ�һ�ڴ����ϲ�
** �䡡��  : uiContain      �������������ڴ�������
**           prgRequest     ������ָ��
** �䡡��  : �µİ������������ڴ�������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static UINT  __regionMergeToNext (UINT  uiContain, PREGION  prgRequest)
{
    UINT64     ui64RequestEndAddr;                                      /*  ������������ַ              */
    PREGION    prgContain  = &_G_rgKipMem[uiContain];                   /*  ������ָ��                  */
    PREGION    prgNext     = prgContain + 1;                            /*  ��һ�ڴ���ָ��              */
    UINT       i;

    if (uiContain == _G_uiMemCount - 1) {                               /*  �����������һ��Ԫ��        */
        return  (uiContain);
    }

    ui64RequestEndAddr = (UINT64)prgRequest->RG_ulAddr + prgRequest->RG_stSize;
    if ((ui64RequestEndAddr    == prgNext->RG_ulAddr) &&                /*  �������ͺ�һ�ڴ�������      */
        (prgRequest->RG_uiType == prgNext->RG_uiType)) {                /*  ����������ͬ                */

        prgContain->RG_stSize += prgNext->RG_stSize;                    /*  ����������һ�ڴ����ϲ�      */
        for (i = uiContain + 1; i < _G_uiMemCount - 1; i++) {           /*  ����һ�ڴ���֮���Ԫ��ǰ��  */
            _G_rgKipMem[i] = _G_rgKipMem[i + 1];
        }

        _G_uiMemCount--;                                                /*  �ڴ���������һ              */
    }

    return  (uiContain);
}
/*********************************************************************************************************
** ��������: __regionApartTail
** ��������: ���������ָ�Ϊ������: ǰ�벿�ְ���������, ��벿����
** �䡡��  : uiContain      �������������ڴ�������
**           prgRequest     ������ָ��
** �䡡��  : �µİ������������ڴ�������
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static UINT  __regionApartTail (UINT  uiContain, PREGION  prgRequest)
{
    PREGION    prgContain = &_G_rgKipMem[uiContain];
    PREGION    prgNew;
    UINT       i;

    for (i = _G_uiMemCount; i > uiContain; i--) {                       /*  ����������֮���Ԫ�غ���    */
        _G_rgKipMem[i] = _G_rgKipMem[i - 1];
    }

    prgNew            = &_G_rgKipMem[uiContain + 1];                    /*  ���ڴ�����ԭ������֮��λ��  */
    prgNew->RG_uiType = MEMORY_TYPE_CONVENTIONAL;                       /*  ���ڴ�����Ϊ�ɷ�������      */
    prgNew->RG_ulAddr = prgRequest->RG_ulAddr + prgRequest->RG_stSize;  /*  ���ڴ�����ʼ��������������  */
    prgNew->RG_stSize = prgContain->RG_ulAddr + prgContain->RG_stSize -
                        prgNew->RG_ulAddr;

    prgContain->RG_stSize  = prgRequest->RG_ulAddr + prgRequest->RG_stSize -
                             prgContain->RG_ulAddr;

    _G_uiMemCount++;                                                    /*  �ڴ���������һ              */

    return  (uiContain);
}
/*********************************************************************************************************
** ��������: __regionRequest
** ��������: ����һ���ڴ�
** ��  ��  : prgRequest     ������ָ��
** ��  ��  : ERROR_NONE     �ɹ�, ����ֵʧ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __regionRequest (PREGION  prgRequest)
{
    INT        iReturn = ERROR_REGION_LACK;
    UINT64     ui64RequestEndAddr;                                      /*  ������������ַ              */
    UINT64     ui64ContainEndAddr;                                      /*  ������������ַ              */
    UINT       uiContain;                                               /*  ���������ڴ�������        */
    PREGION    prgContain;

    uiContain = __regionFind(prgRequest);                               /*  �����ڴ������λ��          */

    prgContain = &_G_rgKipMem[uiContain];
    if (uiContain < _G_uiMemCount) {
        prgContain->RG_uiType = prgRequest->RG_uiType;                  /*  ��ʱ��������ȫ�����        */

        if ((prgRequest->RG_ulAddr == prgContain->RG_ulAddr)) {         /*  �������ڰ�����ǰ��          */
            uiContain = __regionMergeToPrev(uiContain, prgRequest);     /*  ���Ժ�ǰһ�ڴ����ϲ�        */
        } else {                                                        /*  �������ڰ������к��        */
            uiContain = __regionApartFront(uiContain, prgRequest);      /*  ��������ǰ���ָ��ȥ        */
        }

        prgContain         = &_G_rgKipMem[uiContain];                   /*  ���°�����ָ��              */
        ui64RequestEndAddr = (UINT64)prgRequest->RG_ulAddr + prgRequest->RG_stSize;
        ui64ContainEndAddr = (UINT64)prgContain->RG_ulAddr + prgContain->RG_stSize;

        if ((ui64RequestEndAddr == ui64ContainEndAddr)) {               /*  �������ڰ��������          */
            uiContain = __regionMergeToNext(uiContain, prgRequest);     /*  ���Ժͺ�һ�ڴ����ϲ�        */
        } else {                                                        /*  �������ڰ������к��        */
            uiContain = __regionApartTail(uiContain, prgRequest);       /*  ����������벿�ָ��ȥ      */
        }

        iReturn = ERROR_NONE;
    }

    return  (iReturn);
}
/*********************************************************************************************************
** ��������: __regionSort
** ��������: ��ȫ���ڴ��������������, ʹ��ð������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __regionSort (VOID)
{
    REGION    rgTmp;
    UINT      i, j;

    for (i = _G_uiMemCount; i > 0; i--) {
       BOOL bBreak = LW_TRUE;

       for (j = 1; j < i; j++) {
            if (_G_rgKipMem[j - 1].RG_ulAddr > _G_rgKipMem[j].RG_ulAddr) {
                bBreak             = LW_FALSE;
                rgTmp              = _G_rgKipMem[j - 1];
                _G_rgKipMem[j - 1] = _G_rgKipMem[j];
                _G_rgKipMem[j]     = rgTmp;
            }
       }

       if (bBreak == LW_TRUE) {
           break;
       }
    }
}
/*********************************************************************************************************
** ��������: __regionMerge
** ��������: �Խ�����ַ���� 4K ������ڴ���������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __regionMerge (VOID)
{
    REGION    rgRequest;
    UINT64    ui64EndAddr;                                              /*  �ڴ���������ַ              */
    size_t    stGapSize;                                                /*  ������ַ���϶�������ն�    */
    size_t    stRequestSize;                                            /*  ��Ҫ����ϲ��ڴ�����С      */
    UINT    i;

    for (i = 0; i < _G_uiMemCount - 1; i++) {
        ui64EndAddr = (UINT64)_G_rgKipMem[i].RG_ulAddr + _G_rgKipMem[i].RG_stSize;
        stGapSize   = __MEM_ADDR_ALIGN(ui64EndAddr) - ui64EndAddr;

        if (_G_rgKipMem[i + 1].RG_stSize < (__MEM_ALIGN + stGapSize)) { /*  ��һ���ڴ����зֺ��С      */
            stRequestSize = _G_rgKipMem[i + 1].RG_stSize;               /*  ȫ������                    */
        } else {
            stRequestSize = stGapSize;                                  /*  �����벹������ռ�          */
        }

        if ((stRequestSize != 0) &&                                     /*  ��Ҫ�����Բ���ն�          */
            (ui64EndAddr == _G_rgKipMem[i + 1].RG_ulAddr)) {            /*  ����ǰ�����ڴ�������        */
            rgRequest.RG_ulAddr = ui64EndAddr;
            rgRequest.RG_stSize = stRequestSize;
            rgRequest.RG_uiType = _G_rgKipMem[i].RG_uiType;
            if (__regionRequest(&rgRequest) == ERROR_NONE) {            /*  ����ɹ�                    */
                i--;                                                    /*  ��Ҫ�Ա��ڴ����������      */
            }
        }
    }
}
/*********************************************************************************************************
** ��������: regionInit
** ��������: ��ʼ�� region ģ��
** �䡡��  : prgBspRegions      BSP ������ڴ�������
**           uiNum              ��Ч�ڴ�������
** �䡡��  : ERROR_NONE �ɹ�, ����ֵʧ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  regionInit (REGION  *prgBspRegions, UINT  uiNum)
{
    REGION    rgRequest;
    UINT      i;

    _G_uiMemCount = uiNum;
    for (i = 0; i < uiNum; i++) {
        _G_rgKipMem[i] = prgBspRegions[i];

        switch (prgBspRegions[i].RG_uiType) {

        case REGION_TYPE_ROM:
            _G_rgKipMem[i].RG_uiType = MEMORY_TYPE_ARCH;                /*  ����ת��Ϊ MEMORY_TYPE_ARCH */
            break;

        case REGION_TYPE_RAM:
            _G_rgKipMem[i].RG_uiType = MEMORY_TYPE_CONVENTIONAL;        /*  ����ת��Ϊ MEMORY_TYPE_XXX  */
            break;

        default:
            _G_rgKipMem[i].RG_uiType = MEMORY_TYPE_UNDEFINED;
            break;
        }
    }

    __regionSort();                                                     /*  ���ڴ�����������            */

    rgRequest.RG_ulAddr = (addr_t)(_loader_start);
    rgRequest.RG_stSize = (addr_t)(_loader_end - _loader_start);
    rgRequest.RG_uiType = MEMORY_TYPE_LOADER;
    __regionRequest(&rgRequest);                                        /*  ��������ռ���ڴ���          */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: regionRequest
** ��������: �����ڴ���
** �䡡��  : ulAddr             �ڴ�����ַ
**           stSize             �ڴ�����С
** �䡡��  : ERROR_NONE �ɹ�, ����ֵʧ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  regionRequest (addr_t  ulAddr, size_t  stSize)
{
    REGION    rgRequest;

    rgRequest.RG_ulAddr = ulAddr;
    rgRequest.RG_stSize = stSize;
    rgRequest.RG_uiType = MEMORY_TYPE_RESERVED;                         /*  Ĭ��Ϊ MEMORY_TYPE_RESERVED */

    return  (__regionRequest(&rgRequest));
}
/*********************************************************************************************************
** ��������: regionWrite
** ��������: ����ģ�������ڴ���תΪ MEM_DESC �ṹ��, ����д�� KIP
** �䡡��  : pkipPage           KIP �ṹ��ָ��
** �䡡��  : ERROR_NONE �ɹ�, ����ֵʧ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  regionWrite (KIP  *pkipPage)
{
    INT         iReturn   = ERROR_NONE;
    MEM_DESC   *pmdInKip  = (MEM_DESC *)(pkipPage + 1);
    UINT        i;

    __regionMerge();                                                    /*  �ϲ��ڴ�����Ƭ              */

    pkipPage->KIP_uiMemDescNum = _G_uiMemCount;                         /*  д����Ч�ڴ��������� KIP    */

    for (i = 0; i < _G_uiMemCount; i++) {
        if (_G_rgKipMem[i].RG_ulAddr & __MEM_ADDR_MASK) {               /*  ��ʼ��ַδ����              */
            iReturn = ERROR_REGION_ALIGN;                               /*  ����, �޷�д�� KIP �ṹ��   */
            break;
        }

        pmdInKip->MD_ulAddrType = _G_rgKipMem[i].RG_ulAddr |
                                  _G_rgKipMem[i].RG_uiType;
        pmdInKip->MD_stSize     = _G_rgKipMem[i].RG_stSize;

        pmdInKip++;
    }

    return  (iReturn);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
