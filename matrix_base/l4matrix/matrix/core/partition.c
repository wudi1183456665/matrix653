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
** 文   件   名: partition.c
**
** 创   建   人: Li.Jing (李靖)
**
** 文件创建日期: 2017 年 12 月 19 日
**
** 描        述: 进程管理.
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** 函数名称: __prtnKernCreate
** 功能描述: 创建内核进程
** 输　入  : ulVirtAddr     镜像运行地址
**           ulPhysAddr     镜像物理地址
**           ulSize         镜像大小
** 输　出  : 进程控制块指针
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static PMX_PRTN  __prtnKernCreate (addr_t  ulVirtAddr, addr_t  ulPhysAddr, ULONG  ulSize)
{
    PMX_PRTN        pprtnNew;
    MX_OBJECT_ID    ulPrtnId;
    MX_PAGE_TABLE   pgtblNew;                                           /*  页表                        */

    ulPrtnId = kobjAlloc(_KOBJ_PRTN);
    pprtnNew = (PMX_PRTN)kobjGet(ulPrtnId);

    pgtblNew = archMmuAllocPageTable();
    archMmuMap(pgtblNew,                                                /*  映射内核物理空间            */
               ulVirtAddr,                                              /*  内核运行地址                */
               ulPhysAddr,                                              /*  内核所在物理地址            */
               ulSize,                                                  /*  内核所占大小(16M)           */
               MX_MMU_FLAG_KERN_NOC);                                   /*  内核区域                    */
    //bspDebugMsg("---------------\n");
    archMmuMap(pgtblNew,                                                /*  映射 IO 空间                */
               bspInfoSfrBase(),                                        /*  虚拟 IO 地址                */
               bspInfoSfrBase(),                                        /*  物理 IO 地址                */
               bspInfoSfrSize(),                                        /*  IO 地址空间大小(247M)       */
               MX_MMU_FLAG_DEVS);                                       /*  设备区域                    */
    //bspDebugMsg("---------------\n");

    pprtnNew->PRTN_ulPrtnId    = ulPrtnId;
    pprtnNew->PRTN_pgtblMem    = pgtblNew;
    pprtnNew->RTTN_ppiPtrnInfo = MX_NULL;                               /*  内核进程无配置资源信息      */

    return  (pprtnNew);
}

/*********************************************************************************************************
** 函数名称: prtnMakeCur
** 功能描述: 将指定进程设置为当前进程
** 输　入  : pprtn         即将被设置为当前进程的进程
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  prtnMakeCur (PMX_PRTN  pprtn)
{
    //bspDebugMsg("\033[31m------switch to page table %X, %X-------\033[37m\n",
    //             pprtn->PRTN_pgtblMem, MX_VIRT_2_PHYS(pprtn->PRTN_pgtblMem));

    archMmuLoad((MX_PAGE_TABLE)MX_VIRT_2_PHYS(pprtn->PRTN_pgtblMem));   /*  加载页表                    */

    MX_CPU_GET_CUR()->CPU_pprtnCur = pprtn;
}
/*********************************************************************************************************
** 函数名称: prtnAddKumem
** 功能描述: 在进程空间中增加一段用户和内核共同访问的内存
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  prtnAddKumem (PMX_PRTN  pprtn,  PMX_KUMEM  pkumem)
{
    archMmuMap(pprtn->PRTN_pgtblMem,
               pkumem->KUMEM_ulUserAddr,                                /*  用户空间虚拟地址            */
               MX_VIRT_2_PHYS(pkumem->KUMEM_ulKernAddr),                /*  共享的物理地址              */
               pkumem->KUMEM_ulSize,                                    /*  共享区大小                  */
               MX_MMU_FLAG_RDWR);                                       /*  读写                        */
}
/*********************************************************************************************************
** 函数名称: __prtnUserAlloc
** 功能描述: 申请一个进程控制块
** 输　入  : NONE
** 输　出  : 进程控制块指针
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static PMX_PRTN  __prtnUserAlloc (VOID)
{
    MX_OBJECT_ID          ulPrtnId;
    PMX_PRTN              pprtnUser;

    ulPrtnId  = kobjAlloc(_KOBJ_PRTN);
    pprtnUser = (PMX_PRTN)kobjGet(ulPrtnId);

    pprtnUser->PRTN_ulPrtnId = ulPrtnId;

    return  (pprtnUser);
}
/*********************************************************************************************************
** 函数名称: __prtnUserMemMap
** 功能描述: 映射用户地址空间
** 输　入  : ppiPrtn            进程配置信息
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static VOID  __prtnUserMemMap (PMX_PRTN_INFO  ppiPrtn, PMX_PRTN  pprtnUser)
{
    MX_PAGE_TABLE   pgtblNew;                                           /*  页表                        */
    UINT32          i;
    ULONG           ulFlag;
    PMX_MEM_AREA    pmaCur;

    pgtblNew = archMmuCopy(_K_pprtnKernel->PRTN_pgtblMem);

    for (i = 0; i < ppiPrtn->PI_uiMemMum; i++) {
        pmaCur = &ppiPrtn->PI_pmaMems[i];

        if (pmaCur->MA_ulPhysAddr == MX_PRTN_MEM_AUTO) {
            pmaCur->MA_ulPhysAddr = (ULONG)heapAlloc(pmaCur->MA_ulSize, (UINT)ppiPrtn->PI_cpcId);
        } else {
            heapAllocSegment((PVOID)pmaCur->MA_ulPhysAddr, pmaCur->MA_ulSize,
                             (UINT)ppiPrtn->PI_cpcId);
        }

        switch (pmaCur->MA_ulType) {

        case MX_PRTN_MEM_NORM:
            ulFlag = MX_MMU_FLAG_KERN_NOC;
            break;

        case MX_PRTN_MEM_DMA:
            ulFlag = MX_MMU_FLAG_DEVS;
            break;

        default:
            ulFlag = MX_MMU_FLAG_KERN_NOC;
            break;
        }

        archMmuMap(pgtblNew,                                            /*  映射进程独有物理空间        */
                   pmaCur->MA_ulVirtAddr,                               /*  虚拟地址                    */
                   pmaCur->MA_ulPhysAddr,                               /*  物理地址                    */
                   pmaCur->MA_ulSize,                                   /*  大小                        */
                   ulFlag);                                             /*  属性                        */
    }

    pprtnUser->PRTN_pgtblMem = pgtblNew;
}
/*********************************************************************************************************
** 函数名称: __prtnUserCodeCopy
** 功能描述: 复制进程镜像到其配置的物理内存中
** 输　入  : ppiPrtn            进程配置信息
**           bIsRestart         是否是重启
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static  VOID  __prtnUserCodeCopy (PMX_PRTN_INFO  ppiPrtn, BOOL  bIsRestart)
{
    ULONG                *pulFileData;
    ULONG                *pulLoadAddr;

    pulLoadAddr = (ULONG *)(bIsRestart ? ppiPrtn->PI_pmaMems[0].MA_ulVirtAddr :
                                         ppiPrtn->PI_pmaMems[0].MA_ulPhysAddr);
    pulFileData = (ULONG *)ppiPrtn->PI_cpcStart;
    while (pulFileData < (ULONG *)ppiPrtn->PI_cpcEnd) {
        *pulLoadAddr = *pulFileData;                                    /*  复制代码到进程地址空间      */
        pulLoadAddr++;
        pulFileData++;
        //bspDebugMsg(".");
    }
}
/*********************************************************************************************************
** 函数名称: __prtnUserMainVcpu
** 功能描述: 创建用户进程中的主 VCPU
** 输　入  : ppiPrtn            进程配置信息
**           pprtnUser          进程控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static  VOID __prtnUserMainVcpu (PMX_PRTN_INFO  ppiPrtn, PMX_PRTN  pprtnUser)
{
    PMX_VCPU              pvcpu;
    MX_VCPU_ATTR          vcpuattr;

    vcpuattr.VCPUATTR_uiPriority = ppiPrtn->PI_uiPriority;
    vcpuattr.VCPUATTR_pvArg      = (PVOID)5;
    vcpuattr.VCPUATTR_bIsUser    = MX_TRUE;
    pvcpu = vcpuCreate((PVCPU_START_ROUTINE)ppiPrtn->PI_ulEntryAddr, &vcpuattr);
                                                                        /*  创建进程主线程              */

    vcpuBindPartition(pvcpu, pprtnUser);
    //vcpuSetAffinity(pvcpu, 0);
    vcpuStart(pvcpu);
}
/*********************************************************************************************************
** 函数名称: prtnRestart
** 功能描述: 重新启动一个分区
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  prtnRestart (PMX_PRTN  pprtn)
{
    PMX_PRTN_INFO         ppiPrtnInfo;
    PMX_VCPU              pvcpuCur;

    pprtn       = MX_CPU_GET_CUR()->CPU_pprtnCur;                       /*  仅支持当前进程重启          */
    pvcpuCur    = MX_VCPU_GET_CUR();
    ppiPrtnInfo = pprtn->RTTN_ppiPtrnInfo;

    if (ppiPrtnInfo == NULL) {                                          /*  内核进程                    */

    } else {                                                            /*  分区进程                    */
        bspDebugMsg("--------------------------------------------------\n");
        bspDebugMsg("   Part Id : %s\n", ppiPrtnInfo->PI_cpcId);
        bspDebugMsg(" Part File : %s\n", ppiPrtnInfo->PI_cpcFile);
        bspDebugMsg("Part Start : %X\n", ppiPrtnInfo->PI_cpcStart);
        bspDebugMsg("  Part End : %X\n", ppiPrtnInfo->PI_cpcEnd);

        bspDebugMsg("Part Entry : %X\n", ppiPrtnInfo->PI_ulEntryAddr);
        bspDebugMsg(" Part iMem : %X\n", ppiPrtnInfo->PI_uiMemMum);
        bspDebugMsg(" Memory[0] : %X, ", ppiPrtnInfo->PI_pmaMems[0].MA_ulPhysAddr);
        bspDebugMsg(             "%X\n", ppiPrtnInfo->PI_pmaMems[0].MA_ulSize);
        bspDebugMsg(" Part iIrq : %X\n", ppiPrtnInfo->PI_uiIrqMum);
        bspDebugMsg("--------------------------------------------------\n");

        __prtnUserCodeCopy(ppiPrtnInfo, MX_TRUE);                       /*  拷贝用户镜像                */

        irqDisAttach(pvcpuCur);
//        pvcpuCur->VCPU_puctxSP->UCTX_uiCpsr = 0x50;                     /*  开中断, USER 模式           */
//        pvcpuCur->VCPU_puctxSP->UCTX_uiR0   = 5;
//        pvcpuCur->VCPU_puctxSP->UCTX_uiR15  = ppiPrtnInfo->PI_ulEntryAddr;
    }
}
/*********************************************************************************************************
** 函数名称: prtnInit
** 功能描述: 初始化进程模块
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  prtnInit (VOID)
{
    extern  MX_PRTN_INFO  _prtn_info[];
    extern  MX_PRTN_INFO  _eprtn_info[];

    PMX_PRTN_INFO         ppiCur;
    PMX_PRTN              pprtnUser;

    /*
     *  创建内核进程
     */
    _K_pprtnKernel = __prtnKernCreate(MX_CFG_KERN_VIRT_ADDR,
                                      bspInfoKernRamBase(),
                                      bspInfoKernRamSize());

    for (ppiCur = _prtn_info; ppiCur < _eprtn_info; ppiCur++) {
        bspDebugMsg("--------------------------------------------------\n");
        bspDebugMsg("   Part Id : %s\n", ppiCur->PI_cpcId);
        bspDebugMsg(" Part File : %s\n", ppiCur->PI_cpcFile);
        bspDebugMsg("Part Start : %X\n", ppiCur->PI_cpcStart);
        bspDebugMsg("  Part End : %X\n", ppiCur->PI_cpcEnd);

        bspDebugMsg("Part Entry : %X\n", ppiCur->PI_ulEntryAddr);
        bspDebugMsg(" Part iMem : %X\n", ppiCur->PI_uiMemMum);
        bspDebugMsg(" Memory[0] : %X, ", ppiCur->PI_pmaMems[0].MA_ulPhysAddr);
        bspDebugMsg(             "%X\n", ppiCur->PI_pmaMems[0].MA_ulSize);
        bspDebugMsg(" Part iIrq : %X\n", ppiCur->PI_uiIrqMum);

        pprtnUser = __prtnUserAlloc();                                  /*  申请进程控制块              */
        pprtnUser->RTTN_ppiPtrnInfo = ppiCur;                           /*  保存配置资源信息            */
        __prtnUserMemMap(ppiCur, pprtnUser);                            /*  映射进程配置内存            */
        __prtnUserCodeCopy(ppiCur, FALSE);                              /*  拷贝用户镜像                */
        __prtnUserMainVcpu(ppiCur, pprtnUser);                          /*  创建用户主线程              */

        bspDebugMsg("--------------------------------------------------\n");
    }

    dumpHeap();
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

