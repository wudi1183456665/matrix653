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
** 文   件   名: bootMmu.S
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 02 月 03 日
**
** 描        述: 在跳转到 3G 之后的地址之前初始化页表.
*********************************************************************************************************/
#include <Matrix.h>
#include "../config.h"
//#include "bootScu.h"
/*********************************************************************************************************
  CP15 特性
*********************************************************************************************************/
#define CP15_CONTROL_TEXREMAP                       (1 << 28)
/*********************************************************************************************************
  AUX 特性
*********************************************************************************************************/
#define AUX_CTRL_A9_SMP                             (1 <<  6)
#define AUX_CTRL_A9_L1_PREFETCH                     (1 <<  2)
#define AUX_CTRL_A9_L2_PREFETCH                     (1 <<  1)
#define AUX_CTRL_A9_CACHE_MAINTENANCE_BROADCAST     (1 <<  0)
/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#define  PTE_ENTRY_NUM     256                                          /*  一张二级页表项个数          */
/*********************************************************************************************************
  汇编函数
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
 * 新添加的extern 函数
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
extern VOID    start(VOID);                                             /*  虚地址入口函数              */
/*********************************************************************************************************
  定义在链接脚本中的页表符合
*********************************************************************************************************/
extern  ULONG   _pgd_table[];                                            /*  一级页表                    */
//extern ULONG   _pte_table[];                                            /*  二级页表                    */
extern  MX_PTE_TRANSENTRY _pte_table[];      /*?改为ppc二级页表的结构*/
/*********************************************************************************************************
** 函数名称: bootArmErrataFix
** 功能描述: Fix Arm Errata
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 说明：多核初始化会调用此函数，但是ppc暂时用不到多核，先注释掉
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
  域属性位
*********************************************************************************************************/
#define DOMAIN_FAIL         (0x0)                                       /*  产生访问失效                */
#define DOMAIN_CHECK        (0x1)                                       /*  进行权限检查                */
#define DOMAIN_NOCHK        (0x3)                                       /*  不进行权限检查              */
/*********************************************************************************************************
  域描述符 (使用到了三个域: D2 不可访问, D1 不进行权限检查, D0 进行权限检查
*********************************************************************************************************/
#define DOMAIN_ATTR         ((DOMAIN_FAIL << 4) | (DOMAIN_NOCHK << 2) | (DOMAIN_CHECK))
/*********************************************************************************************************
** 函数名称: bootPpcMmuGlobalInit
** 功能描述: MMU 硬件的初始化
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
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
** 函数名称: bootSmpCoreInit (ppc暂时用不到)
** 功能描述: 初始化 SMP 核心
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/

/*********************************************************************************************************
** 函数名称: bootMmu
** 功能描述: 设置最初的内存映射
**           映射物理内存 [256M -- 260M] ---> [256M -- 260M]
** 输　入  : NONE                        \
** 输　出  : NONE                         +-> [3G --- 3G+4M]
** 全局变量: 
** 调用模块: 
** 注  意  : 此页表作用范围从此函数始, 至本核加载选中线程的页表(加入微内核调度器)止.
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootPrimaryMmu (VOID)
{
    INT          i, j;
    ULONG        ulPhysAddr;
    const ULONG  ulVir1Addr = 0x10000000;//?256...B，虚拟地址
    const ULONG  ulVir2Addr = 0xC0000000;//?3.....B
    const INT    iStartPgd1 = (ulVir1Addr >> 20);//?单位转为MB
    const INT    iStartPgd2 = (ulVir2Addr >> 20);
    INT          iStartPgd;
    INT          iKnMapMB;//?内核镜像和各分区的大小
    INT          iIoMapMB;
    ULONG        ulTTBR0Val;
    //ULONG        ulTTBR1Val;

    /*
     *  映射 [32MB 内核镜像] + [32MB 分区1] + [16MB 分区2] + [8MB 分区3] + [8MB 分区4]
     *
     *  之所以分配给分区的内存也要在这里映射, 是因为要在本页表作用范围内,
     *  将分区代码拷贝到其所在内存区.
     */
    ulPhysAddr = 0x10000000;//?物理地址256MB
    iKnMapMB   = 32 + 500;//32 + 16 + 8 + 8;
    for (i = 0; i < iKnMapMB; i++) {
        _pgd_table[iStartPgd1 + i] = (ULONG)&_pte_table[i * PTE_ENTRY_NUM]  //?去掉了arm中D0域的设置
                                   | 0x1;                               /*  设置一级页表项值            */
        _pgd_table[iStartPgd2 + i] = (ULONG)&_pte_table[i * PTE_ENTRY_NUM]
                                   | 0x1;                               /*  类型是 Page table           */

        for (j = 0; j < PTE_ENTRY_NUM; j++) {

            /*_pte_table[i * PTE_ENTRY_NUM + j] = ulPhysAddr                设置二级页表项值            */
                                              /*| (0  << 11)                nG                          */
                                              /*| (1  << 10)                S                           */
                                              /*| (0  <<  9)                ucAP2                       */
                                              /*| (1  <<  6)                ucTEX                       */
                                              /*| (3  <<  4)                ucAP                        */
                                              /*| (3  <<  2)                ucCB                        */
                                              /*| (0  <<  0)                ucXN                        */
                                              /*| (2);                      ucType ?这里arm与ppc一致    */

            _pte_table[i * PTE_ENTRY_NUM + j].PTE_bRef = 0;
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_bChange = 0;
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_ucPP = 3;//?暂设为arm的ucAP
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_ucWIMG = 0;//?暂设为arm的ucCB
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_uiRPN = ulPhysAddr >> 12;
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_ucReserved1 = 2;   //?ucType的值
            _pte_table[i * PTE_ENTRY_NUM + j].PTE_bReserved2 = 0;//?ucEXec执行位
            ulPhysAddr += 0x1000;                                       /*  下个 4KB 页面               */
        }
    }

#if 0
    /*
     *  直接将分区 1 的内存映射到 0x50000000, 不使用微内核内的页表
     */
    {
    const ULONG  ulVir3Addr = 0x50000000;
    const INT    iStartPgd3 = (ulVir3Addr >> 20);
    for (i = 0; i < 512; i++) {                                         /*  分区1 有 512M               */
        _pgd_table[iStartPgd3 + i] = (ULONG)&_pte_table[(i + 32) * PTE_ENTRY_NUM] /* 加上微内核的 32M   */
                                   | (1 << 5)                           /*  选择 D0 域                  */
                                   | 0x1;                               /*  设置一级页表项值            */
    }
    }
#endif
    /*
     *  为 L4SylixOS 分区建立 6M 代码段的映射 0x30000000
     *  以便创建该分区时将代码复制过去
     */
    {
    ulPhysAddr = 0x30000000;
    iStartPgd  = (ulPhysAddr >> 20);
    for (i = 0; i < 6; i++) {                                           /*  代码段有 6M                 */
        _pgd_table[iStartPgd + i] = (ULONG)&_pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM] /* 加上之前的 532M */
                                   | 0x1;                               /*  设置一级页表项值            */

        for (j = 0; j < PTE_ENTRY_NUM; j++) {


            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bRef = 0;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bChange = 0;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucPP = 3;//?暂设为arm的ucAP
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucWIMG = 0;//?暂设为arm的ucCB
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_uiRPN = ulPhysAddr >> 12;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucReserved1 = 2;   //?ucType的值
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bReserved2 = 0;//?ucEXec执行位
            ulPhysAddr += 0x1000;                                       /*  下个 4KB 页面               */
        } 
    }
    iKnMapMB += 12;
    }

    /*
     *  映射 247MB 特殊功能寄存器
     */
    ulPhysAddr = 0x900000;
    iIoMapMB   = 247;
    iStartPgd  = (ulPhysAddr >> 20);
    for (i = 0; i < iIoMapMB; i++) {
        _pgd_table[iStartPgd + i] = (ULONG)&_pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM]
                                  | 0x1;                                /*  设置一级页表项值            */
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
                                                                        /*  设置二级页表项值            */

            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bRef = 0;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bChange = 0;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucPP = 3;//?暂设为arm的ucAP
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucWIMG = 0;//?暂设为arm的ucCB
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_uiRPN = ulPhysAddr >> 12;
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_ucReserved1 = 2;   //?ucType的值
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j].PTE_bReserved2 = 0;//?ucEXec执行位
            ulPhysAddr += 0x1000;                                       /*  下个 4KB 页面               */
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
//    _G_pMmuContext[ulCPUId] = pmmuctx;     //ppc设置MMU当前上下文的方式,参考微内核

    bootPpcMmuGlobalInit();


    //bootSmpCoreInit();

    //bootArmMmuSetTTBase0(ulTTBR0Val);                                   /*  加载页表地址到 TT 寄存器    */
     //bootArmMmuSetTTBase1(ulTTBR1Val);
    KN_SMP_MB();

    ppc60xDCacheEnable();
//    ppc750DCacheEnable();
    ppcMmuEnable();                                                 /*  使能 MMU                    */

    start();                                                            /*  跳转到 3G 之后的虚地址函数  */
}
/*********************************************************************************************************
** 函数名称: bootSecondaryMmu
** 功能描述: 设置从核的页表基址, 使能从核的 MMU
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
** 注  意  : 此页表作用范围从此函数始, 至本核加载选中线程的页表(加入微内核调度器)止.
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

    bootArmMmuSetTTBase0(ulTTBR0Val);                                   /*  加载页表地址到 TT 寄存器    */
    /*bootArmMmuSetTTBase1(ulTTBR1Val);
    KN_SMP_MB();

    bootArmDCacheEnable();

    bootArmMmuEnable();                                                 /*  使能 MMU                    */

    start();                                                            /*  跳转到 3G 之后的虚地址函数  */
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

