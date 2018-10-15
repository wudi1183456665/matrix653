/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: region.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 11 月 06 日
**
** 描        述: region 管理.
*********************************************************************************************************/
#include <loader.h>

/*********************************************************************************************************
  相关宏定义
*********************************************************************************************************/
#define __MEM_ADDR_MASK     ((1UL << 10) - 1)                           /*  (KIP) 内存描述符地址掩码    */
#define __MEM_ALIGN         (1UL << 12)                                 /*  内存地址对齐 (4K)           */
#define __MEM_ADDR_ALIGN(a) (((a) + __MEM_ALIGN - 1) & ~(__MEM_ALIGN - 1))
                                                                        /*  计算内存向上对齐地址        */
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
static REGION    _G_rgKipMem[100];                                      /*  内存区描述数组              */
static UINT      _G_uiMemCount;                                         /*  数组中有效内存区个数        */
/*********************************************************************************************************
  声明链接脚本变量
*********************************************************************************************************/
extern CHAR      _loader_start[];                                       /*  Loader 镜像起始地址         */
extern CHAR      _loader_end[];                                         /*  Loader 镜像结束地址         */

/*********************************************************************************************************
** 函数名称: __regionFind
** 功能描述: 找出指定内存块在全局内存描述数组中的索引
** 输　入  : prgRequest     要查找的内存块指针
** 输　出  : 包含指定内存块的索引值, 如果没找到, 返回 _G_uiMemCount
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static UINT  __regionFind (PREGION  prgRequest)
{
    UINT      i;
    UINT64    ui64CurrentEndAddr;                                       /*  当前内存区结束地址          */
    UINT64    ui64RequestEndAddr;                                       /*  请求区结束地址              */

    for (i = 0; i < _G_uiMemCount; i++) {
        if (_G_rgKipMem[i].RG_uiType != MEMORY_TYPE_CONVENTIONAL) {     /*  非可用内存区                */
            continue;
        }

        ui64CurrentEndAddr = (UINT64)_G_rgKipMem[i].RG_ulAddr + _G_rgKipMem[i].RG_stSize;
        ui64RequestEndAddr = (UINT64)prgRequest->RG_ulAddr + prgRequest->RG_stSize;
        if (prgRequest->RG_ulAddr >= _G_rgKipMem[i].RG_ulAddr &&
            ui64RequestEndAddr <= ui64CurrentEndAddr) {                 /*  本内存区完全包含请求区      */
            break;
        }
    }

    return  (i);
}
/*********************************************************************************************************
** 函数名称: __regionMergeToPrev
** 功能描述: 尝试将包含区和前一内存区合并
** 输　入  : uiContain      包含请求区的内存区索引
**           prgRequest     请求区指针
** 输　出  : 新的包含请求区的内存区索引
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static UINT  __regionMergeToPrev (UINT  uiContain, PREGION  prgRequest)
{
    UINT64     ui64PrevEndAddr;                                         /*  前一内存区结束地址          */
    PREGION    prgContain  = &_G_rgKipMem[uiContain];
    PREGION    prgPrevious = prgContain - 1;
    UINT       i;

    if (uiContain == 0) {                                               /*  包含区是第一个元素          */
        return  (uiContain);
    }

    ui64PrevEndAddr = (UINT64)prgPrevious->RG_ulAddr + prgPrevious->RG_stSize;
    if ((prgRequest->RG_ulAddr == ui64PrevEndAddr) &&                   /*  请求区和前一内存区相邻      */
        (prgRequest->RG_uiType == prgPrevious->RG_uiType)) {            /*  并且类型相同                */

        prgPrevious->RG_stSize += prgContain->RG_stSize;                /*  前一内存区和包含区合并      */
        for (i = uiContain; i < _G_uiMemCount - 1; i++) {               /*  将包含区之后的元素前移      */
            _G_rgKipMem[i] = _G_rgKipMem[i + 1];
        }
        uiContain--;                                                    /*  包含区索引减一              */

        _G_uiMemCount--;                                                /*  内存区总数减一              */
    }

    return  (uiContain);
}
/*********************************************************************************************************
** 函数名称: __regionApartFront
** 功能描述: 将包含区分割为两部分: 前半部空闲, 后半部分以请求区开头
** 输　入  : uiContain      包含请求区的内存区索引
**           prgRequest     请求区指针
** 输　出  : 新的包含请求区的内存区索引
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static UINT  __regionApartFront (UINT  uiContain, PREGION  prgRequest)
{
    PREGION    prgContain = &_G_rgKipMem[uiContain];
    PREGION    prgNew;
    UINT       i;

    for (i = _G_uiMemCount; i > uiContain; i--) {                       /*  将包含区及之后的元素后移    */
        _G_rgKipMem[i] = _G_rgKipMem[i - 1];
    }

    prgNew            = &_G_rgKipMem[uiContain];                        /*  新内存区在原包含区位置      */
    prgNew->RG_uiType = MEMORY_TYPE_CONVENTIONAL;                       /*  新内存区变为可分配类型      */
    prgNew->RG_stSize = prgRequest->RG_ulAddr - prgContain->RG_ulAddr;

    uiContain++;                                                        /*  包含区索引加一              */
    prgContain            = &_G_rgKipMem[uiContain];
    prgContain->RG_ulAddr = prgRequest->RG_ulAddr;
    prgContain->RG_stSize = prgContain->RG_stSize - prgNew->RG_stSize;

    _G_uiMemCount++;                                                    /*  内存区总数加一              */

    return  (uiContain);
}
/*********************************************************************************************************
** 函数名称: __regionMergeToNext
** 功能描述: 尝试将包含区和后一内存区合并
** 输　入  : uiContain      包含请求区的内存区索引
**           prgRequest     请求区指针
** 输　出  : 新的包含请求区的内存区索引
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static UINT  __regionMergeToNext (UINT  uiContain, PREGION  prgRequest)
{
    UINT64     ui64RequestEndAddr;                                      /*  请求区结束地址              */
    PREGION    prgContain  = &_G_rgKipMem[uiContain];                   /*  包含区指针                  */
    PREGION    prgNext     = prgContain + 1;                            /*  后一内存区指针              */
    UINT       i;

    if (uiContain == _G_uiMemCount - 1) {                               /*  包含区是最后一个元素        */
        return  (uiContain);
    }

    ui64RequestEndAddr = (UINT64)prgRequest->RG_ulAddr + prgRequest->RG_stSize;
    if ((ui64RequestEndAddr    == prgNext->RG_ulAddr) &&                /*  请求区和后一内存区相邻      */
        (prgRequest->RG_uiType == prgNext->RG_uiType)) {                /*  并且类型相同                */

        prgContain->RG_stSize += prgNext->RG_stSize;                    /*  包含区和下一内存区合并      */
        for (i = uiContain + 1; i < _G_uiMemCount - 1; i++) {           /*  将下一内存区之后的元素前移  */
            _G_rgKipMem[i] = _G_rgKipMem[i + 1];
        }

        _G_uiMemCount--;                                                /*  内存区总数减一              */
    }

    return  (uiContain);
}
/*********************************************************************************************************
** 函数名称: __regionApartTail
** 功能描述: 将包含区分割为两部分: 前半部分包含请求区, 后半部空闲
** 输　入  : uiContain      包含请求区的内存区索引
**           prgRequest     请求区指针
** 输　出  : 新的包含请求区的内存区索引
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static UINT  __regionApartTail (UINT  uiContain, PREGION  prgRequest)
{
    PREGION    prgContain = &_G_rgKipMem[uiContain];
    PREGION    prgNew;
    UINT       i;

    for (i = _G_uiMemCount; i > uiContain; i--) {                       /*  将包含区及之后的元素后移    */
        _G_rgKipMem[i] = _G_rgKipMem[i - 1];
    }

    prgNew            = &_G_rgKipMem[uiContain + 1];                    /*  新内存区在原包含区之后位置  */
    prgNew->RG_uiType = MEMORY_TYPE_CONVENTIONAL;                       /*  新内存区变为可分配类型      */
    prgNew->RG_ulAddr = prgRequest->RG_ulAddr + prgRequest->RG_stSize;  /*  新内存区起始在请求区结束处  */
    prgNew->RG_stSize = prgContain->RG_ulAddr + prgContain->RG_stSize -
                        prgNew->RG_ulAddr;

    prgContain->RG_stSize  = prgRequest->RG_ulAddr + prgRequest->RG_stSize -
                             prgContain->RG_ulAddr;

    _G_uiMemCount++;                                                    /*  内存区总数加一              */

    return  (uiContain);
}
/*********************************************************************************************************
** 函数名称: __regionRequest
** 功能描述: 请求一块内存
** 输  入  : prgRequest     请求区指针
** 输  出  : ERROR_NONE     成功, 其它值失败
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __regionRequest (PREGION  prgRequest)
{
    INT        iReturn = ERROR_REGION_LACK;
    UINT64     ui64RequestEndAddr;                                      /*  请求区结束地址              */
    UINT64     ui64ContainEndAddr;                                      /*  包含区结束地址              */
    UINT       uiContain;                                               /*  包含查找内存块的索引        */
    PREGION    prgContain;

    uiContain = __regionFind(prgRequest);                               /*  查找内存块所在位置          */

    prgContain = &_G_rgKipMem[uiContain];
    if (uiContain < _G_uiMemCount) {
        prgContain->RG_uiType = prgRequest->RG_uiType;                  /*  暂时将包含区全部标记        */

        if ((prgRequest->RG_ulAddr == prgContain->RG_ulAddr)) {         /*  请求区在包含区前端          */
            uiContain = __regionMergeToPrev(uiContain, prgRequest);     /*  尝试和前一内存区合并        */
        } else {                                                        /*  请求区在包含区中后端        */
            uiContain = __regionApartFront(uiContain, prgRequest);      /*  将包含区前部分割出去        */
        }

        prgContain         = &_G_rgKipMem[uiContain];                   /*  更新包含区指针              */
        ui64RequestEndAddr = (UINT64)prgRequest->RG_ulAddr + prgRequest->RG_stSize;
        ui64ContainEndAddr = (UINT64)prgContain->RG_ulAddr + prgContain->RG_stSize;

        if ((ui64RequestEndAddr == ui64ContainEndAddr)) {               /*  请求区在包含区后端          */
            uiContain = __regionMergeToNext(uiContain, prgRequest);     /*  尝试和后一内存区合并        */
        } else {                                                        /*  请求区在包含区中后端        */
            uiContain = __regionApartTail(uiContain, prgRequest);       /*  将包含区后半部分割出去      */
        }

        iReturn = ERROR_NONE;
    }

    return  (iReturn);
}
/*********************************************************************************************************
** 函数名称: __regionSort
** 功能描述: 对全局内存区数组进行排序, 使用冒泡排序法
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: __regionMerge
** 功能描述: 对结束地址不是 4K 对齐的内存区做处理
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __regionMerge (VOID)
{
    REGION    rgRequest;
    UINT64    ui64EndAddr;                                              /*  内存区结束地址              */
    size_t    stGapSize;                                                /*  结束地址向上对齐所需空洞    */
    size_t    stRequestSize;                                            /*  需要申请合并内存区大小      */
    UINT    i;

    for (i = 0; i < _G_uiMemCount - 1; i++) {
        ui64EndAddr = (UINT64)_G_rgKipMem[i].RG_ulAddr + _G_rgKipMem[i].RG_stSize;
        stGapSize   = __MEM_ADDR_ALIGN(ui64EndAddr) - ui64EndAddr;

        if (_G_rgKipMem[i + 1].RG_stSize < (__MEM_ALIGN + stGapSize)) { /*  下一个内存区切分后过小      */
            stRequestSize = _G_rgKipMem[i + 1].RG_stSize;               /*  全部申请                    */
        } else {
            stRequestSize = stGapSize;                                  /*  仅申请补齐所需空间          */
        }

        if ((stRequestSize != 0) &&                                     /*  需要申请以补齐空洞          */
            (ui64EndAddr == _G_rgKipMem[i + 1].RG_ulAddr)) {            /*  并且前后两内存区相连        */
            rgRequest.RG_ulAddr = ui64EndAddr;
            rgRequest.RG_stSize = stRequestSize;
            rgRequest.RG_uiType = _G_rgKipMem[i].RG_uiType;
            if (__regionRequest(&rgRequest) == ERROR_NONE) {            /*  申请成功                    */
                i--;                                                    /*  需要对本内存区再做检查      */
            }
        }
    }
}
/*********************************************************************************************************
** 函数名称: regionInit
** 功能描述: 初始化 region 模块
** 输　入  : prgBspRegions      BSP 传入的内存区数组
**           uiNum              有效内存区个数
** 输　出  : ERROR_NONE 成功, 其它值失败
** 全局变量: 
** 调用模块: 
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
            _G_rgKipMem[i].RG_uiType = MEMORY_TYPE_ARCH;                /*  类型转换为 MEMORY_TYPE_ARCH */
            break;

        case REGION_TYPE_RAM:
            _G_rgKipMem[i].RG_uiType = MEMORY_TYPE_CONVENTIONAL;        /*  类型转换为 MEMORY_TYPE_XXX  */
            break;

        default:
            _G_rgKipMem[i].RG_uiType = MEMORY_TYPE_UNDEFINED;
            break;
        }
    }

    __regionSort();                                                     /*  对内存区描述排序            */

    rgRequest.RG_ulAddr = (addr_t)(_loader_start);
    rgRequest.RG_stSize = (addr_t)(_loader_end - _loader_start);
    rgRequest.RG_uiType = MEMORY_TYPE_LOADER;
    __regionRequest(&rgRequest);                                        /*  请求自身占用内存区          */

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: regionRequest
** 功能描述: 请求内存区
** 输　入  : ulAddr             内存区地址
**           stSize             内存区大小
** 输　出  : ERROR_NONE 成功, 其它值失败
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
INT  regionRequest (addr_t  ulAddr, size_t  stSize)
{
    REGION    rgRequest;

    rgRequest.RG_ulAddr = ulAddr;
    rgRequest.RG_stSize = stSize;
    rgRequest.RG_uiType = MEMORY_TYPE_RESERVED;                         /*  默认为 MEMORY_TYPE_RESERVED */

    return  (__regionRequest(&rgRequest));
}
/*********************************************************************************************************
** 函数名称: regionWrite
** 功能描述: 将本模块管理的内存区转为 MEM_DESC 结构体, 排序并写入 KIP
** 输　入  : pkipPage           KIP 结构体指针
** 输　出  : ERROR_NONE 成功, 其它值失败
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
INT  regionWrite (KIP  *pkipPage)
{
    INT         iReturn   = ERROR_NONE;
    MEM_DESC   *pmdInKip  = (MEM_DESC *)(pkipPage + 1);
    UINT        i;

    __regionMerge();                                                    /*  合并内存区碎片              */

    pkipPage->KIP_uiMemDescNum = _G_uiMemCount;                         /*  写入有效内存区个数到 KIP    */

    for (i = 0; i < _G_uiMemCount; i++) {
        if (_G_rgKipMem[i].RG_ulAddr & __MEM_ADDR_MASK) {               /*  起始地址未对齐              */
            iReturn = ERROR_REGION_ALIGN;                               /*  错误, 无法写入 KIP 结构体   */
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
