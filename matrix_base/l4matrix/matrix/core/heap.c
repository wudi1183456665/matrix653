/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                嵌入式 L4 微内核操作系统
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: heap.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 04 月 10 日
**
** 描        述: 堆内存操作.
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** 函数名称: __segmentIsValid
** 功能描述: 判断一块区域是否合法
** 输　入  : psegment    要判断的区域
** 输　出  : 是否合法
** 全局变量: 
** 调用模块: 本模块
*********************************************************************************************************/
static BOOL  __segmentIsValid (const PMX_SEGMENT  psegment)
{
    return (psegment->SEGMENT_ulHigAddr >= psegment->SEGMENT_ulLowAddr);
}
/*********************************************************************************************************
** 函数名称: __addSegment
** 功能描述: 向数组中添加一块区域
** 输　入  : psegment    要添加的区域
** 输　出  : 是否添加成功
** 全局变量: 
** 调用模块: 本模块
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
** 函数名称: __deleteSegment
** 功能描述: 删除数组中的一个区域 (后面的区域向前移动)
** 输　入  : psegment         要删除的区域 (必须指向数组中某一元素)
** 输　出  : 是否删除成功
** 全局变量: 
** 调用模块: 本模块
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
** 函数名称: __findCrossSegment
** 功能描述: 查找数组中和指定区域有交集的区域
** 输　入  : psegment         要查找的区域
** 输　出  : 查找到的区域地址
** 全局变量: 
** 调用模块: 本模块
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
** 函数名称: __findContainSegment
** 功能描述: 查找数组中是否有包含指定区域的区域
** 输　入  : psegment         要查找的区域
** 输　出  : 查找到的区域
** 全局变量: 
** 调用模块: 本模块
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
** 函数名称: __allocFrom
** 功能描述: 在大区域中申请小区域, 如果不相等, 将拆分后的区域加入数组
** 输　入  : psegmentContain    大区域
**           psegment           要申请的小区域
** 输　出  : 是否申请成功
** 全局变量: 
** 调用模块: 本模块
*********************************************************************************************************/
static BOOL  __allocFrom (PMX_SEGMENT  psegmentContain, const PMX_SEGMENT  psegment)
{
    MX_SEGMENT          segmentLeft;

    if (psegmentContain->SEGMENT_uiOwner != 0 &&                        /*  大区域已经有拥有者          */
        psegmentContain->SEGMENT_uiOwner != psegment->SEGMENT_uiOwner) {/*  并且拥有者不等于小区域的    */
        return  (MX_FALSE);
    }
    if (psegmentContain->SEGMENT_uiOwner == psegment->SEGMENT_uiOwner) {/*  大区域的拥有者等于小区域的  */
        return  (MX_TRUE);
    }

    /*
     *  大小区域完全相等的情况
     */
    if (psegmentContain->SEGMENT_ulLowAddr == psegment->SEGMENT_ulLowAddr &&
        psegmentContain->SEGMENT_ulHigAddr == psegment->SEGMENT_ulHigAddr) {
        psegmentContain->SEGMENT_uiOwner = psegment->SEGMENT_uiOwner;
        return  (MX_TRUE);
    }

    /*
     *  大小区域仅低地址相等的情况
     */
    if (psegmentContain->SEGMENT_ulLowAddr == psegment->SEGMENT_ulLowAddr) {
        psegmentContain->SEGMENT_ulLowAddr = psegment->SEGMENT_ulHigAddr + 1;
        return  (__addSegment(psegment));                               /*  添加小区域                  */
    }

    /*
     *  大小区域仅高地址相等的情况
     */
    if (psegmentContain->SEGMENT_ulHigAddr == psegment->SEGMENT_ulHigAddr) {
        psegmentContain->SEGMENT_ulHigAddr = psegment->SEGMENT_ulLowAddr - 1;
        return  (__addSegment(psegment));                               /*  添加小区域                  */
    }

    /*
     *  低地址和高地址全不相等，将大区域拆分为 3 个区域: [拆分低区域] [申请的小区域] [拆分高区域]
     */
    segmentLeft.SEGMENT_uiOwner   = psegmentContain->SEGMENT_uiOwner;
    segmentLeft.SEGMENT_ulLowAddr = psegment->SEGMENT_ulHigAddr + 1;
    segmentLeft.SEGMENT_ulHigAddr = psegmentContain->SEGMENT_ulHigAddr;

    psegmentContain->SEGMENT_ulHigAddr = psegment->SEGMENT_ulLowAddr - 1;

    return  (__addSegment(psegment) && __addSegment(&segmentLeft));
}
/*********************************************************************************************************
** 函数名称: heapInit
** 功能描述: 初始化微内核堆
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  heapInit (VOID)
{
    _K_heapKernel.HEAP_uiValidNum = 0;

    heapAdd((PVOID)bspInfoPrtnRamBase(), bspInfoPrtnRamSize());
}
/*********************************************************************************************************
** 函数名称: heapAdd
** 功能描述: 向堆内添加内存
** 输　入  : pvMemory           需要添加的内存 (必须保证对齐)
**           ulSize             内存大小
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
    if (segmentTmp.SEGMENT_ulLowAddr > 0) {                                         /*  尝试和前面的区域合并        */
        segmentTmp.SEGMENT_ulLowAddr = psegmentNew->SEGMENT_ulLowAddr - 1;

        psegmentCross = __findCrossSegment(&segmentTmp);
        if (psegmentCross && psegmentCross->SEGMENT_uiOwner == psegmentNew->SEGMENT_uiOwner) {
            psegmentNew->SEGMENT_ulLowAddr = psegmentCross->SEGMENT_ulLowAddr;
            __deleteSegment(psegmentCross);
        }
    }

    segmentTmp = segmentNew;
    if (segmentTmp.SEGMENT_ulHigAddr + 1 != 0) {                                    /*  尝试和前面的区域合并        */
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
** 函数名称: heapAllocAlign
** 功能描述: 从堆中申请一块内存 (首次适应算法)
** 输　入  : ulSize             申请的字节数
**           ulAlign            内存对齐值
** 输　出  : 分配的内存地址
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PVOID  heapAllocAlign (ULONG  ulSize, UINT  uiType, ULONG  ulAlign)
{
    UINT              i;
    ULONG             ulAlignStart;
    MX_SEGMENT        segmentAlloc;

    for (i = 0; i < _K_heapKernel.HEAP_uiValidNum; i++) {
        if (_K_heapKernel.HEAP_segments[i].SEGMENT_uiOwner != 0) {
            continue;                                                   /*  已被占用                    */
        }
        if ((_K_heapKernel.HEAP_segments[i].SEGMENT_ulLowAddr + ulSize - 1) < 
             _K_heapKernel.HEAP_segments[i].SEGMENT_ulLowAddr) {
            continue;                                                   /*  超出 ULONG 型表示范围       */
        }

        ulAlignStart = ROUND_UP(_K_heapKernel.HEAP_segments[i].SEGMENT_ulLowAddr, ulAlign);

        if (ulAlignStart < _K_heapKernel.HEAP_segments[i].SEGMENT_ulHigAddr &&
            ulAlignStart + ulSize - 1 <= _K_heapKernel.HEAP_segments[i].SEGMENT_ulHigAddr) {
            break;                                                      /*  找到符合区域                */
        }
    }

    if (i >= _K_heapKernel.HEAP_uiValidNum) {
        return  (MX_NULL);
    }

    /*
     *  确定要申请的区域
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
** 函数名称: heapAlloc
** 功能描述: 从堆中申请一块内存 (首次适应算法)
** 输　入  : ulSize             申请的字节数
** 输　出  : 分配的内存地址
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PVOID  heapAlloc (ULONG  ulSize, UINT  uiType)
{
    return  (heapAllocAlign(ulSize, uiType, 1));
}
/*********************************************************************************************************
** 函数名称: heapAllocSegment
** 功能描述: 申请指定区域
** 输　入  : psegmentAlloc         要申请的区域
** 输　出  : 指定区域起始地址
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PVOID  heapAllocSegment (PVOID  pvStartAddr, ULONG  ulSize, UINT  uiType)
{
    MX_SEGMENT      segmentAlloc;
    PMX_SEGMENT     psegmentAlloc = &segmentAlloc;
    PMX_SEGMENT     psegmentContain;

    /*
     *  确定要申请的区域
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
** 函数名称: heapFree
** 功能描述: 将申请的空间释放回堆 (立即聚合算法)
** 输　入  : pvStartAddr        归还的地址
** 输　出  : 分配的内存地址
** 全局变量: 正确返回 MX_NULL 否则返回 pvStartAddr
** 调用模块: 
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
