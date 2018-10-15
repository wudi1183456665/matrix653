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
#include "bootScu.h"
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
extern VOID    bootArmMmuEnable(VOID);
extern VOID    bootArmMmuDisable(VOID);
extern VOID    bootArmMmuSetTTBase0(ULONG  ulTTBR0Val);
extern VOID    bootArmMmuSetTTBase1(ULONG  ulTTBR1Val);
extern VOID    bootArmMmuSetDomain(UINT32  uiDomainAttr);
extern VOID    bootArmMmuV7SetTTBCR(UINT32  uiTTBCR);
extern VOID    bootArmMmuInvalidateTLB(VOID);
extern VOID    bootArmMmuSetProcessId(INT  pid);
extern VOID    bootArmControlFeatureDisable(UINT32  uiFeature);
extern VOID    bootArmAuxControlFeatureEnable(UINT32  ulFeature);
extern VOID    bootArmAuxControlFeatureDisable(UINT32  uiFeature);
extern VOID    bootArmDCacheV7Disable(VOID);
extern VOID    bootArmDCacheEnable(VOID);
extern addr_t  bootArmPrivatePeriphBaseGet(VOID);
extern UINT32  bootArmCp15MainIdReg(VOID);
extern ULONG   bootArmCurCpuId(VOID);
extern VOID    start(VOID);                                             /*  虚地址入口函数              */
/*********************************************************************************************************
  定义在链接脚本中的页表符合
*********************************************************************************************************/
extern ULONG   _pgd_table[];                                            /*  一级页表                    */
extern ULONG   _pte_table[];                                            /*  二级页表                    */
/*********************************************************************************************************
** 函数名称: bootArmErrataFix
** 功能描述: Fix Arm Errata
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmErrataFix (VOID)
{
    UINT32  uiCtrl;
    UINT32  uiCpuId   = bootArmCp15MainIdReg();
    UINT32  uiScuBase = bootArmPrivatePeriphBaseGet();

    /*
     *  ARM_ERRATA_764369 (Cortex-A9 only)
     */
    if ((uiCpuId & 0xff0ffff0) == 0x410fc090) {
        uiCtrl = *(volatile UINT32 *)(uiScuBase + 0x30);
        if (!(uiCtrl & 1)) {
            *(volatile UINT32 *)(uiScuBase + 0x30) = (uiCtrl | 0x1);
        }
    }
}
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
** 函数名称: bootArmMmuGlobalInit
** 功能描述: MMU 硬件的初始化
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmMmuGlobalInit (VOID)
{
    bootArmMmuInvalidateTLB();
    bootArmMmuSetDomain(DOMAIN_ATTR);
    bootArmMmuSetProcessId(0);

    bootArmControlFeatureDisable(CP15_CONTROL_TEXREMAP);                /*  Disable TEX remapping       */
    bootArmMmuV7SetTTBCR(0);                                            /*  Use the 32-bit translation  */
                                                                        /*  system, Always use TTBR0    */
}
/*********************************************************************************************************
** 函数名称: bootSmpCoreInit
** 功能描述: 初始化 SMP 核心
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootSmpCoreInit(VOID)
{ 
    ULONG  ulCpuId = bootArmCurCpuId();

    bootArmErrataFix();

    if (ulCpuId == 0) {
        bootArmScuAccessCtrlSet(0xF);                                   /*  允许所有 CPU 访问 SCU 寄存器*/
        bootArmScuFeatureEnable(SCU_FEATURE_SCU);                       /*  使能 SCU                    */
        bootArmScuFeatureEnable(SCU_FEATURE_SCU_SPECULATIVE_LINEFILL);  /*  使能投机的行填充            */
        bootArmScuFeatureEnable(SCU_FEATURE_SCU_RAMS_PARITY);           /*  使能投机的行填充            */
        bootArmScuFeatureEnable(SCU_FEATURE_SCU_STANDBY);               /*  使能投机的行填充            */
        bootArmScuFeatureEnable(SCU_FEATURE_IC_STANDBY);                /*  使能投机的行填充            */
        bootArmScuFeatureDisable(SCU_FEATURE_ADDRESS_FILTERING);        /*  关闭地址过滤                */
    }
    bootArmScuSecureInvalidateAll(ulCpuId, 0xF);

    bootArmAuxControlFeatureDisable(AUX_CTRL_A9_L1_PREFETCH);           /*  Errorta 751473              */
    bootArmAuxControlFeatureDisable(AUX_CTRL_A9_L2_PREFETCH);           /*  Errorta 751473              */
    bootArmAuxControlFeatureEnable(AUX_CTRL_A9_SMP);
    bootArmAuxControlFeatureEnable(AUX_CTRL_A9_CACHE_MAINTENANCE_BROADCAST);
}
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
    const ULONG  ulVir1Addr = 0x10000000;
    const ULONG  ulVir2Addr = 0xC0000000;
    const INT    iStartPgd1 = (ulVir1Addr >> 20);
    const INT    iStartPgd2 = (ulVir2Addr >> 20);
    INT          iStartPgd;
    INT          iKnMapMB;
    INT          iIoMapMB;
    ULONG        ulTTBR0Val;
    //ULONG        ulTTBR1Val;

    /*
     *  映射 [32MB 内核镜像] + [32MB 分区1] + [16MB 分区2] + [8MB 分区3] + [8MB 分区4]
     *
     *  之所以分配给分区的内存也要在这里映射, 是因为要在本页表作用范围内,
     *  将分区代码拷贝到其所在内存区.
     */
    ulPhysAddr = 0x10000000;
    iKnMapMB   = 32 + 500;//32 + 16 + 8 + 8;
    for (i = 0; i < iKnMapMB; i++) {
        _pgd_table[iStartPgd1 + i] = (ULONG)&_pte_table[i * PTE_ENTRY_NUM]
                                   | (1 << 5)                           /*  选择 D0 域                  */
                                   | 0x1;                               /*  设置一级页表项值            */
        _pgd_table[iStartPgd2 + i] = (ULONG)&_pte_table[i * PTE_ENTRY_NUM]
                                   | (1 << 5)                           /*  选择 D0 域                  */
                                   | 0x1;                               /*  类型是 Page table           */

        for (j = 0; j < PTE_ENTRY_NUM; j++) {

            _pte_table[i * PTE_ENTRY_NUM + j] = ulPhysAddr              /*  设置二级页表项值            */
                                              | (0  << 11)              /*  nG                          */
                                              | (1  << 10)              /*  S                           */
                                              | (0  <<  9)              /*  ucAP2                       */
                                              | (1  <<  6)              /*  ucTEX                       */
                                              | (3  <<  4)              /*  ucAP                        */
                                              | (3  <<  2)              /*  ucCB                        */
                                              | (0  <<  0)              /*  ucXN                        */
                                              | (2);                    /*  ucType                      */

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
                                   | (1 << 5)                           /*  选择 D0 域                  */
                                   | 0x1;                               /*  设置一级页表项值            */

        for (j = 0; j < PTE_ENTRY_NUM; j++) {

            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j] = ulPhysAddr   /*  设置二级页表项值            */
                                                         | (0  << 11)   /*  nG                          */
                                                         | (1  << 10)   /*  S                           */
                                                         | (0  <<  9)   /*  ucAP2                       */
                                                         | (1  <<  6)   /*  ucTEX                       */
                                                         | (3  <<  4)   /*  ucAP                        */
                                                         | (3  <<  2)   /*  ucCB                        */
                                                         | (0  <<  0)   /*  ucXN                        */
                                                         | (2);         /*  ucType                      */

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
                                  | (1 << 5)                            /*  选择 D0 域                  */
                                  | 0x1;                                /*  设置一级页表项值            */
        for (j = 0; j < PTE_ENTRY_NUM; j++) {
            _pte_table[(i+iKnMapMB) * PTE_ENTRY_NUM + j] = ulPhysAddr /* | 0x432; */
                                                         | (0  << 11)   /*  nG                          */
                                                         | (1  << 10)   /*  S                           */
                                                         | (0  <<  9)   /*  ucAP2                       */
                                                         | (0  <<  6)   /*  ucTEX                       */
                                                         | (3  <<  4)   /*  ucAP                        */
                                                         | (0  <<  2)   /*  ucCB                        */
                                                         | (0  <<  0)   /*  ucXN                        */
                                                         | (2);         /*  ucType                      */
                                                                        /*  设置二级页表项值            */

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
    ulTTBR0Val = /*ulTTBR1Val = */((ULONG)_pgd_table
                            | (1 << 6)
                            | (0 << 5)
                            | (1 << 3)
                            | (0 << 2)
                            | (1 << 1)
                            | (0 << 0));

    bootArmMmuGlobalInit();
    bootSmpCoreInit();

    bootArmMmuSetTTBase0(ulTTBR0Val);                                   /*  加载页表地址到 TT 寄存器    */
    //bootArmMmuSetTTBase1(ulTTBR1Val);
    KN_SMP_MB();

    bootArmDCacheEnable();

    bootArmMmuEnable();                                                 /*  使能 MMU                    */

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
    ULONG        ulTTBR0Val;
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
    bootArmMmuSetTTBase1(ulTTBR1Val);
    KN_SMP_MB();

    bootArmDCacheEnable();

    bootArmMmuEnable();                                                 /*  使能 MMU                    */

    start();                                                            /*  跳转到 3G 之后的虚地址函数  */
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

