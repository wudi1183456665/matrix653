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
** ��   ��   ��: heap.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 04 �� 10 ��
**
** ��        ��: ���ڴ����.
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** ��������: __segmentIsValid
** ��������: �ж�һ�������Ƿ�Ϸ�
** �䡡��  : psegment    Ҫ�жϵ�����
** �䡡��  : �Ƿ�Ϸ�
** ȫ�ֱ���: 
** ����ģ��: ��ģ��
*********************************************************************************************************/
static BOOL  __segmentIsValid (const PMX_SEGMENT  psegment)
{
    return (psegment->SEGMENT_ulHigAddr >= psegment->SEGMENT_ulLowAddr);
}
/*********************************************************************************************************
** ��������: __addSegment
** ��������: �����������һ������
** �䡡��  : psegment    Ҫ��ӵ�����
** �䡡��  : �Ƿ���ӳɹ�
** ȫ�ֱ���: 
** ����ģ��: ��ģ��
*********************************************************************************************************/
static BOOL  __addSegment (const PMX_SEGMENT  psegment)
{
    if (_K_heapKernel.HEAP_uiValidNum < MX_CFG_MAX_SEGMENTS) {
        _K_heapKernel.HEAP_segments[_K_heapKernel.HEAP_uiValidNum++] = *psegment;
        return  (MX_TRUE);

    } else {
        return  (MX_FALSE);
    }
}
/*********************************************************************************************************
** ��������: __deleteSegment
** ��������: ɾ�������е�һ������ (�����������ǰ�ƶ�)
** �䡡��  : psegment         Ҫɾ�������� (����ָ��������ĳһԪ��)
** �䡡��  : �Ƿ�ɾ���ɹ�
** ȫ�ֱ���: 
** ����ģ��: ��ģ��
*********************************************************************************************************/
static BOOL  __deleteSegment (const PMX_SEGMENT  psegment)
{
    UINT    i;

    for (i = 0; i < _K_heapKernel.HEAP_uiValidNum; i++) {
        if (&_K_heapKernel.HEAP_segments[i] > psegment) {
            _K_heapKernel.HEAP_segments[i-1] = _K_heapKernel.HEAP_segments[i];
        }
    }
    _K_heapKernel.HEAP_uiValidNum--;

    return  (MX_TRUE);
}
/*********************************************************************************************************
** ��������: __findCrossSegment
** ��������: ���������к�ָ�������н���������
** �䡡��  : psegment         Ҫ���ҵ�����
** �䡡��  : ���ҵ��������ַ
** ȫ�ֱ���: 
** ����ģ��: ��ģ��
*********************************************************************************************************/
static PMX_SEGMENT  __findCrossSegment (const PMX_SEGMENT  psegment)
{
    UINT    i;

    for (i = 0; i < _K_heapKernel.HEAP_uiValidNum; i++) {
        if (_K_heapKernel.HEAP_segments[i].SEGMENT_ulLowAddr <= psegment->SEGMENT_ulHigAddr &&
            _K_heapKernel.HEAP_segments[i].SEGMENT_ulHigAddr >= psegment->SEGMENT_ulLowAddr) {
            return  (&_K_heapKernel.HEAP_segments[i]);
        }
    }
    return  (MX_NULL);
}
/*********************************************************************************************************
** ��������: __findContainSegment
** ��������: �����������Ƿ��а���ָ�����������
** �䡡��  : psegment         Ҫ���ҵ�����
** �䡡��  : ���ҵ�������
** ȫ�ֱ���: 
** ����ģ��: ��ģ��
*********************************************************************************************************/
static PMX_SEGMENT  __findContainSegment (const PMX_SEGMENT  psegment)
{
    UINT    i;

    for (i = 0; i < _K_heapKernel.HEAP_uiValidNum; i++) {
        if (_K_heapKernel.HEAP_segments[i].SEGMENT_ulLowAddr <= psegment->SEGMENT_ulLowAddr &&
            _K_heapKernel.HEAP_segments[i].SEGMENT_ulHigAddr >= psegment->SEGMENT_ulHigAddr) {
            return &_K_heapKernel.HEAP_segments[i];
        }
    }
    return  (MX_NULL);
}
/*********************************************************************************************************
** ��������: __allocFrom
** ��������: �ڴ�����������С����, ��������, ����ֺ�������������
** �䡡��  : psegmentContain    ������
**           psegment           Ҫ�����С����
** �䡡��  : �Ƿ�����ɹ�
** ȫ�ֱ���: 
** ����ģ��: ��ģ��
*********************************************************************************************************/
static BOOL  __allocFrom (PMX_SEGMENT  psegmentContain, const PMX_SEGMENT  psegment)
{
    MX_SEGMENT          segmentLeft;

    if (psegmentContain->SEGMENT_uiOwner != 0 &&                        /*  �������Ѿ���ӵ����          */
        psegmentContain->SEGMENT_uiOwner != psegment->SEGMENT_uiOwner) {/*  ����ӵ���߲�����С�����    */
        return  (MX_FALSE);
    }
    if (psegmentContain->SEGMENT_uiOwner == psegment->SEGMENT_uiOwner) {/*  �������ӵ���ߵ���С�����  */
        return  (MX_TRUE);
    }

    /*
     *  ��С������ȫ��ȵ����
     */
    if (psegmentContain->SEGMENT_ulLowAddr == psegment->SEGMENT_ulLowAddr &&
        psegmentContain->SEGMENT_ulHigAddr == psegment->SEGMENT_ulHigAddr) {
        psegmentContain->SEGMENT_uiOwner = psegment->SEGMENT_uiOwner;
        return  (MX_TRUE);
    }

    /*
     *  ��С������͵�ַ��ȵ����
     */
    if (psegmentContain->SEGMENT_ulLowAddr == psegment->SEGMENT_ulLowAddr) {
        psegmentContain->SEGMENT_ulLowAddr = psegment->SEGMENT_ulHigAddr + 1;
        return  (__addSegment(psegment));                               /*  ���С����                  */
    }

    /*
     *  ��С������ߵ�ַ��ȵ����
     */
    if (psegmentContain->SEGMENT_ulHigAddr == psegment->SEGMENT_ulHigAddr) {
        psegmentContain->SEGMENT_ulHigAddr = psegment->SEGMENT_ulLowAddr - 1;
        return  (__addSegment(psegment));                               /*  ���С����                  */
    }

    /*
     *  �͵�ַ�͸ߵ�ַȫ����ȣ�����������Ϊ 3 ������: [��ֵ�����] [�����С����] [��ָ�����]
     */
    segmentLeft.SEGMENT_uiOwner   = psegmentContain->SEGMENT_uiOwner;
    segmentLeft.SEGMENT_ulLowAddr = psegment->SEGMENT_ulHigAddr + 1;
    segmentLeft.SEGMENT_ulHigAddr = psegmentContain->SEGMENT_ulHigAddr;

    psegmentContain->SEGMENT_ulHigAddr = psegment->SEGMENT_ulLowAddr - 1;

    return  (__addSegment(psegment) && __addSegment(&segmentLeft));
}
/*********************************************************************************************************
** ��������: heapInit
** ��������: ��ʼ��΢�ں˶�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  heapInit (VOID)
{
    _K_heapKernel.HEAP_uiValidNum = 0;

    heapAdd((PVOID)bspInfoPrtnRamBase(), bspInfoPrtnRamSize());
}
/*********************************************************************************************************
** ��������: heapAdd
** ��������: ���������ڴ�
** �䡡��  : pvMemory           ��Ҫ��ӵ��ڴ� (���뱣֤����)
**           ulSize             �ڴ��С
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
BOOL  heapAdd (PVOID  pvMemory, ULONG  ulSize)
{
    MX_SEGMENT          segmentNew;
    PMX_SEGMENT         psegmentNew = &segmentNew;
    PMX_SEGMENT         psegmentCross;
    MX_SEGMENT          segmentTmp;
    
    psegmentNew->SEGMENT_uiOwner   = 0;
    psegmentNew->SEGMENT_ulLowAddr = (ULONG)pvMemory;
    psegmentNew->SEGMENT_ulHigAddr = (ULONG)pvMemory + ulSize - 1;

    if (!__segmentIsValid(psegmentNew)) {
        return  (MX_FALSE);
    }

    psegmentCross = __findCrossSegment(psegmentNew);
    if (psegmentCross != MX_NULL) {
        psegmentNew->SEGMENT_ulLowAddr = MIN(psegmentCross->SEGMENT_ulLowAddr,
                                             psegmentNew->SEGMENT_ulLowAddr);
        psegmentNew->SEGMENT_ulHigAddr = MAX(psegmentCross->SEGMENT_ulHigAddr,
                                             psegmentNew->SEGMENT_ulHigAddr);
        __deleteSegment(psegmentCross);
    }

    segmentTmp = segmentNew;
    if (segmentTmp.SEGMENT_ulLowAddr > 0) {                                         /*  ���Ժ�ǰ�������ϲ�        */
        segmentTmp.SEGMENT_ulLowAddr = psegmentNew->SEGMENT_ulLowAddr - 1;

        psegmentCross = __findCrossSegment(&segmentTmp);
        if (psegmentCross && psegmentCross->SEGMENT_uiOwner == psegmentNew->SEGMENT_uiOwner) {
            psegmentNew->SEGMENT_ulLowAddr = psegmentCross->SEGMENT_ulLowAddr;
            __deleteSegment(psegmentCross);
        }
    }

    segmentTmp = segmentNew;
    if (segmentTmp.SEGMENT_ulHigAddr + 1 != 0) {                                    /*  ���Ժ�ǰ�������ϲ�        */
        segmentTmp.SEGMENT_ulHigAddr = psegmentNew->SEGMENT_ulHigAddr + 1;

        psegmentCross = __findCrossSegment(&segmentTmp);
        if (psegmentCross && psegmentCross->SEGMENT_uiOwner == psegmentNew->SEGMENT_uiOwner) {
            psegmentNew->SEGMENT_ulHigAddr = psegmentCross->SEGMENT_ulHigAddr;
            __deleteSegment(psegmentCross);
        }
    }

    return  (__addSegment(psegmentNew));
}
/*********************************************************************************************************
** ��������: heapAllocAlign
** ��������: �Ӷ�������һ���ڴ� (�״���Ӧ�㷨)
** �䡡��  : ulSize             ������ֽ���
**           ulAlign            �ڴ����ֵ
** �䡡��  : ������ڴ��ַ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PVOID  heapAllocAlign (ULONG  ulSize, UINT  uiType, ULONG  ulAlign)
{
    UINT              i;
    ULONG             ulAlignStart;
    MX_SEGMENT        segmentAlloc;

    for (i = 0; i < _K_heapKernel.HEAP_uiValidNum; i++) {
        if (_K_heapKernel.HEAP_segments[i].SEGMENT_uiOwner != 0) {
            continue;                                                   /*  �ѱ�ռ��                    */
        }
        if ((_K_heapKernel.HEAP_segments[i].SEGMENT_ulLowAddr + ulSize - 1) < 
             _K_heapKernel.HEAP_segments[i].SEGMENT_ulLowAddr) {
            continue;                                                   /*  ���� ULONG �ͱ�ʾ��Χ       */
        }

        ulAlignStart = ROUND_UP(_K_heapKernel.HEAP_segments[i].SEGMENT_ulLowAddr, ulAlign);

        if (ulAlignStart < _K_heapKernel.HEAP_segments[i].SEGMENT_ulHigAddr &&
            ulAlignStart + ulSize - 1 <= _K_heapKernel.HEAP_segments[i].SEGMENT_ulHigAddr) {
            break;                                                      /*  �ҵ���������                */
        }
    }

    if (i >= _K_heapKernel.HEAP_uiValidNum) {
        return  (MX_NULL);
    }

    /*
     *  ȷ��Ҫ���������
     */
    segmentAlloc.SEGMENT_uiOwner   = uiType;
    segmentAlloc.SEGMENT_ulLowAddr = ulAlignStart;
    segmentAlloc.SEGMENT_ulHigAddr = ulAlignStart + ulSize - 1;

    if (__allocFrom(&_K_heapKernel.HEAP_segments[i], &segmentAlloc)) {
        return  ((PVOID)segmentAlloc.SEGMENT_ulLowAddr);
    } else {
        return  (MX_NULL);
    }
}
/*********************************************************************************************************
** ��������: heapAlloc
** ��������: �Ӷ�������һ���ڴ� (�״���Ӧ�㷨)
** �䡡��  : ulSize             ������ֽ���
** �䡡��  : ������ڴ��ַ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PVOID  heapAlloc (ULONG  ulSize, UINT  uiType)
{
    return  (heapAllocAlign(ulSize, uiType, 1));
}
/*********************************************************************************************************
** ��������: heapAllocSegment
** ��������: ����ָ������
** �䡡��  : psegmentAlloc         Ҫ���������
** �䡡��  : ָ��������ʼ��ַ
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
PVOID  heapAllocSegment (PVOID  pvStartAddr, ULONG  ulSize, UINT  uiType)
{
    MX_SEGMENT      segmentAlloc;
    PMX_SEGMENT     psegmentAlloc = &segmentAlloc;
    PMX_SEGMENT     psegmentContain;

    /*
     *  ȷ��Ҫ���������
     */
    segmentAlloc.SEGMENT_uiOwner   = uiType;
    segmentAlloc.SEGMENT_ulLowAddr = (ULONG)pvStartAddr;
    segmentAlloc.SEGMENT_ulHigAddr = (ULONG)pvStartAddr + ulSize - 1;

    if (!__segmentIsValid(psegmentAlloc)) {
        return  (MX_FALSE);
    }

    psegmentContain = __findContainSegment(psegmentAlloc);
    if (psegmentContain != MX_NULL) {
        if (__allocFrom(psegmentContain, psegmentAlloc)) {
            return  ((PVOID)psegmentAlloc->SEGMENT_ulLowAddr);
        }
    }

    return  (MX_NULL);
}
/*********************************************************************************************************
** ��������: heapFree
** ��������: ������Ŀռ��ͷŻض� (�����ۺ��㷨)
** �䡡��  : pvStartAddr        �黹�ĵ�ַ
** �䡡��  : ������ڴ��ַ
** ȫ�ֱ���: ��ȷ���� MX_NULL ���򷵻� pvStartAddr
** ����ģ��: 
*********************************************************************************************************/
PVOID  heapFree (PVOID  pvStartAddr)
{
    MX_SEGMENT        segmentFree;
    PMX_SEGMENT       psegmentContain;
    ULONG             ulSize;

    segmentFree.SEGMENT_uiOwner   = 0;
    segmentFree.SEGMENT_ulLowAddr = (ULONG)pvStartAddr;
    segmentFree.SEGMENT_ulHigAddr = (ULONG)pvStartAddr;

    psegmentContain = __findContainSegment(&segmentFree);
    if (psegmentContain != MX_NULL) {
        pvStartAddr = (PVOID)psegmentContain->SEGMENT_ulLowAddr;
        ulSize      = psegmentContain->SEGMENT_ulHigAddr - (ULONG)pvStartAddr + 1;
        __deleteSegment(psegmentContain);
        heapAdd(pvStartAddr, ulSize);
    }

    return  (pvStartAddr);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
