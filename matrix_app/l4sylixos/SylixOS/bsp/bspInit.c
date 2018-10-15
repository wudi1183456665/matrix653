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
** 文   件   名: bspInit.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 03 月 16 日
**
** 描        述: BSP 用户 C 程序入口.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "config.h"                                                     /*  工程配置 & 处理器相关       */
/*********************************************************************************************************
  操作系统相关
*********************************************************************************************************/
#include "SylixOS.h"                                                    /*  操作系统                    */
#include "stdlib.h"                                                     /*  for system() function       */
#include "gdbserver.h"                                                  /*  GDB 调试器                  */
#include "sys/compiler.h"                                               /*  编译器相关                  */
#include <linux/mtd/mtd.h>
#include "sys/ioccom.h"
/*********************************************************************************************************
  BSP 及 驱动程序
*********************************************************************************************************/
//#include "board.h"
#if 0 //l4
#include "irq/armGic.h"
#include "arch/arm/common/cp15/armCp15.h"
#include "arch/arm/mpcore/scu/armScu.h"
#include "driver/ccm_pll/ccm_pll.h"
#include "driver/uart/sio.h"
#include "driver/usb/imx6q_usb.h"
#endif
/*********************************************************************************************************
  操作系统符号表
*********************************************************************************************************/
#if LW_CFG_SYMBOL_EN > 0 && defined(__GNUC__)
#include "symbol.h"
#endif                                                                  /*  LW_CFG_SYMBOL_EN > 0        */
                                                                        /*  defined(__GNUC__)           */
/*********************************************************************************************************
  内存初始化映射表
*********************************************************************************************************/
#define  __BSPINIT_MAIN_FILE
#include "bspMap.h"
/*********************************************************************************************************
  主线程与启动线程堆栈 (t_boot 可以大一点, startup.sh 中可能有很多消耗堆栈的操作)
*********************************************************************************************************/
#define  __LW_THREAD_BOOT_STK_SIZE      (16 * LW_CFG_KB_SIZE)
#define  __LW_THREAD_MAIN_STK_SIZE      (16 * LW_CFG_KB_SIZE)
/*********************************************************************************************************
  外部函数声明
*********************************************************************************************************/
extern VOID  t_main(VOID);
extern VOID  bspCpuUpDone(VOID);
extern VOID  bspCpuDown(ULONG  ulCPUId);
extern SIO_CHAN  *sioChanCreate(INT  iChannel);
/*********************************************************************************************************
** 函数名称: targetInit
** 功能描述: 初始化 i.MX6DQ
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块: 原来放在targetInit.c文件中，这里统一放在bspInit
*********************************************************************************************************/
VOID  targetInit (VOID)
{
#if 0 //l4
    imx6qCcmPLLInit();

    /*
     * 拷贝到异常向量表到 OCRAM 的指定位置, 不开 MMU 时有用
     */
#define RAM_VECTORS_SIZE    72
#define OCRAM_LEN           (256 * LW_CFG_KB_SIZE)
#define OCRAM               0x00900000

    memcpy((PVOID)(OCRAM + OCRAM_LEN - RAM_VECTORS_SIZE), (PVOID)BSP_CFG_RAM_BASE, RAM_VECTORS_SIZE);
#endif
}
/*********************************************************************************************************
** 函数名称: halModeInit
** 功能描述: 初始化目标系统运行的模式
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  halModeInit (VOID)
{
}
/*********************************************************************************************************
** 函数名称: halTimeInit
** 功能描述: 初始化目标系统时间系统 (系统默认时区为: 东8区)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_RTC_EN > 0

static VOID  halTimeInit (VOID)
{
    //boardTimeInit();
}

#endif                                                                  /*  LW_CFG_RTC_EN > 0           */
/*********************************************************************************************************
** 函数名称: __arm_wfi
** 功能描述: CPU 进入空闲等待中断
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : CPU0 不能使用 WFI 指令.
*********************************************************************************************************/
#if 0 //l4
#define IDLE_WFI

#ifdef IDLE_WFI

static VOID  __arm_wfi (ULONG  ulCPUId)
{
  if (ulCPUId != 0) {
      armWaitForInterrupt();
  }
}

#endif
/*********************************************************************************************************
** 函数名称: __armErrataFix
** 功能描述: Fix Arm Errata
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  __armErrataFix (VOID)
{
             UINT  uiCtrl;
             UINT  uiCpuID   = armCp15MainIdReg();
    REGISTER UINT  uiScuBase = armPrivatePeriphBaseGet();

    /*
     * ARM_ERRATA_764369 (Cortex-A9 only)
     */
    if ((uiCpuID & 0xff0ffff0) == 0x410fc090) {
        uiCtrl = readl(uiScuBase + 0x30);
        if (!(uiCtrl & 1)) {
            writel(uiCtrl | 0x1, uiScuBase + 0x30);
        }
    }
}
#endif
/*********************************************************************************************************
** 函数名称: halIdleInit
** 功能描述: 初始化目标系统空闲时间作业
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  halIdleInit (VOID)
{
#ifdef IDLE_WFI
    API_SystemHookAdd(__arm_wfi,
                      LW_OPTION_THREAD_IDLE_HOOK);                      /*  空闲时暂停 CPU              */
#endif
}
/*********************************************************************************************************
** 函数名称: halSmpCoreInit
** 功能描述: 初始化 SMP 核心
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  halSmpCoreInit (VOID)
{
#if 0 //l4
    ULONG  ulCPUId = archMpCur();

    __armErrataFix();                                                   /*  Fix ARM Errata              */

    if (ulCPUId == 0) {
        armScuAccessCtrlSet(0xF);                                       /*  允许所有 CPU 访问 SCU 寄存器*/
        armScuFeatureEnable(SCU_FEATURE_SCU);                           /*  使能 SCU                    */
        armScuFeatureEnable(SCU_FEATURE_SCU_SPECULATIVE_LINEFILL);      /*  使能投机的行填充            */
        armScuFeatureEnable(SCU_FEATURE_SCU_RAMS_PARITY);               /*  使能投机的行填充            */
        armScuFeatureEnable(SCU_FEATURE_SCU_STANDBY);                   /*  使能投机的行填充            */
        armScuFeatureEnable(SCU_FEATURE_IC_STANDBY);                    /*  使能投机的行填充            */
        armScuFeatureDisable(SCU_FEATURE_ADDRESS_FILTERING);            /*  关闭地址过滤                */
    }

    armScuSecureInvalidateAll(ulCPUId, 0xF);                            /*  无效 SCU TAG RAMS COPY      */

    armAuxControlFeatureDisable(AUX_CTRL_A9_L1_PREFETCH);               /*  Errorta 751473              */
    armAuxControlFeatureDisable(AUX_CTRL_A9_L2_PREFETCH);               /*  Errorta 751473              */
    armAuxControlFeatureEnable(AUX_CTRL_A9_SMP);                        /*  加入 SMP                    */
    armAuxControlFeatureEnable(AUX_CTRL_A9_CACHE_MAINTENANCE_BROADCAST);/*  使能 TLB, L1 D-Cache 同步   */

    API_CacheEnable(INSTRUCTION_CACHE);                                 /*  使能 L1 I-Cache             */
    API_CacheEnable(DATA_CACHE);                                        /*  使能 L1 D-Cache             */

    KN_SMP_MB();
    API_VmmMmuEnable();                                                 /*  启动 MMU                    */

    API_InterVectorIpi(ulCPUId, ARM_SW_INT_VECTOR(ulCPUId));            /*  安装 IPI 向量               */
    armGicIntVecterEnable(ARM_SW_INT_VECTOR(ulCPUId),
                          LW_FALSE,
                          ARM_SW_INT_PRIORITY, 1 << ulCPUId);           /*  使能 IPI 中断               */
#endif

    bspCpuUpDone();                                                     /*  通知其它 CPU 本核心启动完成 */
}
/*********************************************************************************************************
** 函数名称: halDisableOtherCpu
** 功能描述: 主动停止其他CPU，保证当时只有主核在运行
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  halDisableOtherCpu (VOID)
{
    ULONG  ulCPUId;

    for (ulCPUId = 1; ulCPUId < LW_NCPUS; ulCPUId++) {                  /*  停止其它 CPU                */
        bspCpuDown(ulCPUId);
    }
}
/*********************************************************************************************************
** 函数名称: halPrimaryCpuInit
** 功能描述: 按顺序初始化目标系统的 Primary CPU 的 VFP MMU CACHE
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  halPrimaryCpuInit (VOID)
{
    halDisableOtherCpu();

    API_KernelFpuPrimaryInit(ARM_MACHINE_A9, ARM_FPU_VFPv3);            /*  初始化 FPU 系统             */

    API_VmmLibPrimaryInit(_G_physicalDesc,
                          _G_virtualDesc,
                          ARM_MACHINE_A9);                              /*  初始化 VMM 系统             */

    API_CacheLibPrimaryInit(CACHE_COPYBACK, CACHE_COPYBACK,
                            ARM_MACHINE_A9);                            /*  初始化 CACHE 系统           */

    halSmpCoreInit();                                                   /*  初始化 SMP 核心             */
}
#if LW_CFG_SMP_EN > 0
/*********************************************************************************************************
** 函数名称: halSecondaryCpuInit
** 功能描述: 按顺序初始化目标系统的 Secondary CPU 的 VFP MMU CACHE
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  halSecondaryCpuInit (VOID)
{
    API_KernelFpuSecondaryInit(ARM_MACHINE_A9, ARM_FPU_VFPv3);          /*  初始化 FPU 系统             */

    API_VmmLibSecondaryInit(ARM_MACHINE_A9);                            /*  初始化 VMM 系统             */

    API_CacheLibSecondaryInit(ARM_MACHINE_A9);                          /*  初始化 CACHE 系统           */

    //armGicCpuInit(LW_FALSE, 255);                                       /*  初始化当前 CPU 使用 GIC 接口*/

    halSmpCoreInit();                                                   /*  初始化 SMP 核心             */
}

#endif
/*********************************************************************************************************
** 函数名称: halPmInit
** 功能描述: 初始化目标系统电源管理系统
** 输　入:   NONE
** 输　出:   NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_POWERM_EN > 0

static VOID  halPmInit (VOID)
{

}

#endif                                                                  /*  LW_CFG_POWERM_EN > 0        */
/*********************************************************************************************************
** 函数名称: halBusInit
** 功能描述: 初始化目标系统总线系统
** 输　入:   NONE
** 输　出:   NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0

static VOID  halBusInit (VOID)
{
    //boardBusInit();
}

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
/*********************************************************************************************************
** 函数名称: halDrvInit
** 功能描述: 初始化目标系统静态驱动程序
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0

static VOID  halDrvInit (VOID)
{
    /*
     *  standard device driver (rootfs and procfs need install first.)
     */
    rootFsDrv();                                                        /*  ROOT   device driver        */
    procFsDrv();                                                        /*  proc   device driver        */
    shmDrv();                                                           /*  shm    device driver        */
    randDrv();                                                          /*  random device driver        */
    ptyDrv();                                                           /*  pty    device driver        */
    ttyDrv();                                                           /*  tty    device driver        */
    memDrv();                                                           /*  mem    device driver        */
    pipeDrv();                                                          /*  pipe   device driver        */
    spipeDrv();                                                         /*  spipe  device driver        */
    fatFsDrv();                                                         /*  FAT FS device driver        */
    tpsFsDrv();                                                         /*  TPS FS device driver        */
    ramFsDrv();                                                         /*  RAM FS device driver        */
    romFsDrv();                                                         /*  ROM FS device driver        */
    nfsDrv();                                                           /*  nfs    device driver        */
    yaffsDrv();                                                         /*  yaffs  device driver        */
    canDrv();                                                           /*  CAN    device driver        */

    //boardDrvInit();
}

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
/*********************************************************************************************************
** 函数名称: halDevInit
** 功能描述: 初始化目标系统静态设备组件
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0

static VOID  halDevInit (VOID)
{
    SIO_CHAN    *psio;
    /*
     *  创建根文件系统时, 将自动创建 dev, mnt, var 目录.
     */
    rootFsDevCreate();                                                  /*  创建根文件系统              */
    procFsDevCreate();                                                  /*  创建 proc 文件系统          */
    shmDevCreate();                                                     /*  创建共享内存设备            */
    randDevCreate();                                                    /*  创建随机数文件              */

#define __TTY_RD_BUF_SIZE      256
#define __TTY_WR_BUF_SIZE      256

#if 0 //l4
    psio = sioChanCreate(0);                                            /*  创建串口 0 通道             */
    if (psio) {
        ttyDevCreate("/dev/ttyS0", psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);             /*  add    tty   device         */
    }
    psio = sioChanCreate(1);                                            /*  创建串口 1 通道             */
    if (psio) {
        ttyDevCreate("/dev/ttyS1", psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);             /*  add    tty   device         */
    }
#else
    psio = sioChanCreate(BSP_CFG_COM_SHELL);                            /*  创建串口通道             */
    if (psio) {
        ttyDevCreate(BSP_CFG_STD_FILE, psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);             /*  add    tty   device         */
    }
#endif

    //boardDevInit();
}

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
/*********************************************************************************************************
** 函数名称: halLogInit
** 功能描述: 初始化目标系统日志系统
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_LOG_LIB_EN > 0

static VOID  halLogInit (VOID)
{
    fd_set      fdLog;

    FD_ZERO(&fdLog);
    FD_SET(STD_OUT, &fdLog);
    API_LogFdSet(STD_OUT + 1, &fdLog);                                  /*  初始化日志                  */
}

#endif                                                                  /*  LW_CFG_LOG_LIB_EN > 0       */
/*********************************************************************************************************
** 函数名称: halStdFileInit
** 功能描述: 初始化目标系统标准文件系统
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0

static VOID  halStdFileInit (VOID)
{
    INT     iFd = open(BSP_CFG_STD_FILE, O_RDWR, 0);

    if (iFd >= 0) {
        ioctl(iFd, FIOBAUDRATE,   SIO_BAUD_115200);
        ioctl(iFd, FIOSETOPTIONS, (OPT_TERMINAL & (~OPT_7_BIT)));       /*  system terminal 8 bit mode  */

        ioGlobalStdSet(STD_IN,  iFd);
        ioGlobalStdSet(STD_OUT, iFd);
        ioGlobalStdSet(STD_ERR, iFd);
    }
}

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
/*********************************************************************************************************
** 函数名称: halShellInit
** 功能描述: 初始化目标系统 shell 环境, (getopt 使用前一定要初始化 shell 环境)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_SHELL_EN > 0

static VOID  halShellInit (VOID)
{
    API_TShellInit();

    /*
     *  初始化 appl 中间件 shell 接口
     */
    zlibShellInit();
    viShellInit();

    /*
     *  初始化 GDB 调试器
     */
    gdbInit();
}

#endif                                                                  /*  LW_CFG_SHELL_EN > 0         */
/*********************************************************************************************************
** 函数名称: halNetInit
** 功能描述: 网络组件初始化
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_NET_EN > 0

static VOID  halNetInit (VOID)
{
    API_NetInit();                                                      /*  初始化网络系统              */
    API_NetSnmpInit();

    /*
     *  初始化网络附加工具
     */
#if LW_CFG_NET_PING_EN > 0
    API_INetPingInit();
    API_INetPing6Init();
#endif                                                                  /*  LW_CFG_NET_PING_EN > 0      */

#if LW_CFG_NET_NETBIOS_EN > 0
    API_INetNetBiosInit();
    API_INetNetBiosNameSet("sylixos");
#endif                                                                  /*  LW_CFG_NET_NETBIOS_EN > 0   */

#if LW_CFG_NET_TFTP_EN > 0
    API_INetTftpServerInit("/tmp");
#endif                                                                  /*  LW_CFG_NET_TFTP_EN > 0      */

#if LW_CFG_NET_FTPD_EN > 0
    API_INetFtpServerInit("/");
#endif                                                                  /*  LW_CFG_NET_FTP_EN > 0       */

#if LW_CFG_NET_TELNET_EN > 0
    API_INetTelnetInit(LW_NULL);
#endif                                                                  /*  LW_CFG_NET_TELNET_EN > 0    */

#if LW_CFG_NET_NAT_EN > 0
    API_INetNatInit();
#endif                                                                  /*  LW_CFG_NET_NAT_EN > 0       */

#if LW_CFG_NET_NPF_EN > 0
    API_INetNpfInit();
#endif                                                                  /*  LW_CFG_NET_NPF_EN > 0       */

#if LW_CFG_NET_VPN_EN > 0
    API_INetVpnInit();
#endif                                                                  /*  LW_CFG_NET_VPN_EN > 0       */
}
/*********************************************************************************************************
** 函数名称: halNetifAttch
** 功能描述: 网络接口连接
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  halNetifAttch (VOID)
{
    //boardNetifAttch();
}

#endif                                                                  /*  LW_CFG_NET_EN > 0           */
/*********************************************************************************************************
** 函数名称: halMonitorInit
** 功能描述: 内核监控器上传初始化
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_MONITOR_EN > 0

static VOID  halMonitorInit (VOID)
{
    /*
     *  可以再这里创建内核监控器上传通道, 也可以使用 shell 命令操作.
     */
}

#endif                                                                  /*  LW_CFG_MONITOR_EN > 0       */
/*********************************************************************************************************
** 函数名称: halPosixInit
** 功能描述: 初始化 posix 子系统 (如果系统支持 proc 文件系统, 则必须放在 proc 文件系统安装之后!)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_POSIX_EN > 0

static VOID  halPosixInit (VOID)
{
    API_PosixInit();
}

#endif                                                                  /*  LW_CFG_POSIX_EN > 0         */
/*********************************************************************************************************
** 函数名称: halSymbolInit
** 功能描述: 初始化目标系统符号表环境, (为 module loader 提供环境)
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_SYMBOL_EN > 0

static VOID  halSymbolInit (VOID)
{
#ifdef __GNUC__
    void *__aeabi_read_tp();
#endif                                                                  /*  __GNUC__                    */

    API_SymbolInit();

#ifdef __GNUC__
    symbolAddAll();

    /*
     *  GCC will emit calls to this routine under -mtp=soft.
     */
    API_SymbolAdd("__aeabi_read_tp", (caddr_t)__aeabi_read_tp, LW_SYMBOL_FLAG_XEN);
#endif                                                                  /*  __GNUC__                    */
    //imx6qUsbSymbolInit();
}

#endif                                                                  /*  LW_CFG_SYMBOL_EN > 0        */
/*********************************************************************************************************
** 函数名称: halLoaderInit
** 功能描述: 初始化目标系统程序或模块装载器
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
#if LW_CFG_MODULELOADER_EN > 0

static VOID  halLoaderInit (VOID)
{
    API_LoaderInit();
}

#endif                                                                  /*  LW_CFG_SYMBOL_EN > 0        */
/*********************************************************************************************************
** 函数名称: halBootThread
** 功能描述: 多任务状态下的初始化启动任务
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static PVOID  halBootThread (PVOID  pvBootArg)
{
    LW_CLASS_THREADATTR     threakattr = API_ThreadAttrGetDefault();    /*  使用默认属性                */
    ULONG                   ulCPUId;

    (VOID)pvBootArg;

#if LW_CFG_SHELL_EN > 0
    halShellInit();
#endif                                                                  /*  LW_CFG_SHELL_EN > 0         */

#if LW_CFG_DEVICE_EN > 0
    halBusInit();

#if LW_CFG_RTC_EN > 0
    halTimeInit();
#endif                                                                  /*  LW_CFG_RTC_EN > 0           */

    halDrvInit();
    halDevInit();
    bspDebugMsg("-------halDevInit()-------\n");
    halStdFileInit();
#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */

#if LW_CFG_LOG_LIB_EN > 0
    halLogInit();
    console_loglevel = default_message_loglevel;                        /*  设置 printk 打印信息等级    */
#endif                                                                  /*  LW_CFG_LOG_LIB_EN > 0       */

#if LW_CFG_POWERM_EN > 0
    halPmInit();
#endif                                                                  /*  LW_CFG_POWERM_EN > 0        */

#if BSP_CFG_NAND_ROOTFS_EN > 0
#ifdef __GNUC__
    nand_init();
    mtdDevCreateEx("/n");                                               /*  mount mtddevice             */
#else
    nandDevCreateEx("/n");                                              /*  mount nandflash disk(yaffs) */
#endif
#endif                                                                  /*  参考代码结束                */

#if LW_CFG_DEVICE_EN > 0                                                /*  map rootfs                  */
    rootFsMap(LW_ROOTFS_MAP_LOAD_VAR | LW_ROOTFS_MAP_SYNC_TZ | LW_ROOTFS_MAP_SET_TIME);
#endif

    /*
     *  网络初始化一般放在 shell 初始化之后, 因为初始化网络组件时, 会自动注册 shell 命令.
     */
#if LW_CFG_NET_EN > 0
    halNetInit();
    bspDebugMsg("-------halNetInit()-------\n");
    halNetifAttch();                                                    /*  wlan 网卡需要下载固件       */
    bspDebugMsg("-------halNetifAttch()-------\n");
#endif                                                                  /*  LW_CFG_NET_EN > 0           */

#if LW_CFG_POSIX_EN > 0
    halPosixInit();
#endif                                                                  /*  LW_CFG_POSIX_EN > 0         */

#if LW_CFG_SYMBOL_EN > 0
    halSymbolInit();
#endif                                                                  /*  LW_CFG_SYMBOL_EN > 0        */

#if LW_CFG_MODULELOADER_EN > 0
    halLoaderInit();
#endif                                                                  /*  LW_CFG_MODULELOADER_EN > 0  */

#if LW_CFG_MONITOR_EN > 0
    halMonitorInit();
#endif                                                                  /*  LW_CFG_MONITOR_EN > 0       */

    ioctl(STDERR_FILENO, FIOSYNC, 0);

    for (ulCPUId = 1; ulCPUId < LW_NCPUS; ulCPUId++) {                  /*  启动其它 CPU                */
        //API_CpuUp(ulCPUId);
    }

    API_CacheClear(DATA_CACHE, 0, (size_t)~0);

#if LW_CFG_SHELL_EN > 0
    tshellStartup();                                                    /*  执行启动脚本                */
#endif

    API_ThreadAttrSetStackSize(&threakattr, __LW_THREAD_MAIN_STK_SIZE); /*  设置 main 线程的堆栈大小    */
    API_ThreadCreate("t_main",
                     (PTHREAD_START_ROUTINE)t_main,
                     &threakattr,
                     LW_NULL);                                          /*  Create "t_main()" thread    */

    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: usrStartup
** 功能描述: 初始化应用相关组件, 创建操作系统的第一个任务.
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  usrStartup (VOID)
{
    LW_CLASS_THREADATTR     threakattr;

    /*
     *  注意, 不要修改该初始化顺序 (必须先初始化 vmm 才能正确的初始化 cache, 网络必须最后初始化)
     */
    halIdleInit();

    halPrimaryCpuInit();

    API_ThreadAttrBuild(&threakattr,
                        __LW_THREAD_BOOT_STK_SIZE,
                        LW_PRIO_CRITICAL,
                        LW_OPTION_THREAD_STK_CHK,
                        LW_NULL);
    API_ThreadCreate("t_boot",
                     (PTHREAD_START_ROUTINE)halBootThread,
                     &threakattr,
                     LW_NULL);                                          /*  Create boot thread          */
}
#if 0 //l4
/*********************************************************************************************************
  openocd 相关参数
*********************************************************************************************************/
#if LW_CFG_MODULELOADER_EN > 0
#include "loader/include/loader_lib.h"
#endif                                                                  /*  LW_CFG_MODULELOADER_EN > 0  */

LW_STRUCT_PACK_BEGIN
struct LW_OPENOCD_PARAM {
    LW_STRUCT_PACK_FIELD(UINT32         uiFlags);
    LW_STRUCT_PACK_FIELD(UINT32         uiUlongSize);
    LW_STRUCT_PACK_FIELD(UINT32         uiPointerSize);
    LW_STRUCT_PACK_FIELD(UINT32         uiPidSize);

    LW_STRUCT_PACK_FIELD(UINT32         uiObjectNameSize);
    LW_STRUCT_PACK_FIELD(UINT32         uiMaxThreads);
    LW_STRUCT_PACK_FIELD(UINT32         uiMaxProcessors);

    LW_STRUCT_PACK_FIELD(UINT32         CPU_ptcbTCBCur);
    LW_STRUCT_PACK_FIELD(UINT32         CPU_ptcbTCBHigh);
    LW_STRUCT_PACK_FIELD(UINT32         CPU_ulInterNesting);
    LW_STRUCT_PACK_FIELD(UINT32         CPU_ulStatus);

    LW_STRUCT_PACK_FIELD(UINT32         TCB_cThreadName);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_ulId);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_ucPriority);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_ulLastError);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_i64PageFailCounter);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_ulCPUId);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_archRegCtx);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_usStatus);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_ulOption);
    LW_STRUCT_PACK_FIELD(UINT32         TCB_pvVProcessContext);

    LW_STRUCT_PACK_FIELD(UINT32         VP_pid);
}  LW_STRUCT_PACK_STRUCT;
LW_STRUCT_PACK_END
typedef struct LW_OPENOCD_PARAM            *PLW_OPENOCD_PARAM;

struct LW_OPENOCD_PARAM  _G_sylixosOpenocdParam = { 0 };
/*********************************************************************************************************
** 函数名称: bspOpenocdInit
** 功能描述: 初始化 openocd
** 输　入  : NONE
** 输　出  : NONE
** 全局变量:
** 调用模块:
** 注  意  : 必须要在 pOpenocdParam->uiFlags = htole32(1); 语句处下一个断点,
**           并且单步执行 pOpenocdParam->uiFlags = htole32(1); 语句.
**           建议关闭 D-Cache, 屏蔽 API_CacheEnable(DATA_CACHE); 语句.
*********************************************************************************************************/
static VOID bspOpenocdInit (VOID)
{
    PLW_OPENOCD_PARAM  pOpenocdParam = &_G_sylixosOpenocdParam;

    pOpenocdParam->uiFlags                  = htole32(0);

    pOpenocdParam->uiUlongSize              = htole32(sizeof(ULONG));
    pOpenocdParam->uiPointerSize            = htole32(sizeof(PVOID));
    pOpenocdParam->uiPidSize                = htole32(sizeof(pid_t));

    pOpenocdParam->uiObjectNameSize         = htole32(LW_CFG_OBJECT_NAME_SIZE);
    pOpenocdParam->uiMaxThreads             = htole32(LW_CFG_MAX_THREADS);
    pOpenocdParam->uiMaxProcessors          = htole32(LW_CFG_MAX_PROCESSORS);

    pOpenocdParam->CPU_ptcbTCBCur           = htole32(offsetof(LW_CLASS_CPU, CPU_ptcbTCBCur));
    pOpenocdParam->CPU_ptcbTCBHigh          = htole32(offsetof(LW_CLASS_CPU, CPU_ptcbTCBHigh));
    pOpenocdParam->CPU_ulInterNesting       = htole32(offsetof(LW_CLASS_CPU, CPU_ulInterNesting));
    pOpenocdParam->CPU_ulStatus             = htole32(offsetof(LW_CLASS_CPU, CPU_ulStatus));

    pOpenocdParam->TCB_cThreadName          = htole32(offsetof(LW_CLASS_TCB, TCB_cThreadName));
    pOpenocdParam->TCB_ulId                 = htole32(offsetof(LW_CLASS_TCB, TCB_ulId));
    pOpenocdParam->TCB_ucPriority           = htole32(offsetof(LW_CLASS_TCB, TCB_ucPriority));
    pOpenocdParam->TCB_ulLastError          = htole32(offsetof(LW_CLASS_TCB, TCB_ulLastError));
    pOpenocdParam->TCB_i64PageFailCounter   = htole32(offsetof(LW_CLASS_TCB, TCB_i64PageFailCounter));
    pOpenocdParam->TCB_ulCPUId              = htole32(offsetof(LW_CLASS_TCB, TCB_ulCPUId));
    pOpenocdParam->TCB_archRegCtx           = htole32(offsetof(LW_CLASS_TCB, TCB_archRegCtx));
    pOpenocdParam->TCB_usStatus             = htole32(offsetof(LW_CLASS_TCB, TCB_usStatus));
    pOpenocdParam->TCB_ulOption             = htole32(offsetof(LW_CLASS_TCB, TCB_ulOption));
    pOpenocdParam->TCB_pvVProcessContext    = htole32(offsetof(LW_CLASS_TCB, TCB_pvVProcessContext));

#if LW_CFG_MODULELOADER_EN > 0
    pOpenocdParam->VP_pid                   = htole32(offsetof(LW_LD_VPROC,  VP_pid));
#else
    pOpenocdParam->VP_pid                   = 0;
#endif                                                                  /*  LW_CFG_MODULELOADER_EN > 0  */

    pOpenocdParam->uiFlags                  = htole32(1);
    pOpenocdParam->uiFlags                  = htole32(0);
}
#endif
/*********************************************************************************************************
** 函数名称: halPrimaryCpuMain
** 功能描述: Primary CPU C 入口
** 输　入  : NONE
** 输　出  : 0
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  halPrimaryCpuMain (VOID)
{
    /*
     *  系统内核堆与系统堆
     */
    extern UCHAR  __heap_start, __heap_end;

#if 0 //l4
    bspOpenocdInit();                                                   /*  初始化 openocd              */
#endif

    halModeInit();                                                      /*  初始化硬件                  */

    /*
     *  这里的调试端口是脱离操作系统的, 所以它应该不依赖于操作系统而存在.
     *  当系统出现错误时, 这个端口显得尤为关键. (项目成熟后可以通过配置关掉)
     *  (!!当前串口已经由bootloader初始化了, 这里无需处理.)
     */

    /*
     *  这里使用 bsp 设置启动参数, 如果 bootloader 支持, 可使用 bootloader 设置.
     *  为了兼容以前的项目, 这里 kfpu=yes 允许内核中(包括中断)使用 FPU.
     */

#if (BOARD_MARSBOARD == 1) || (BOARD_NORINCO == 1)
    API_KernelStartParam("ncpus=2 kdlog=no kderror=yes kfpu=no heapchk=yes hz=1000 hhz=1000 "
                         "rfsmap=/boot:/media/sdcard0,/:/media/sdcard1");
#elif (BOARD_SYLIXOS_EVM == 1) || (BOARD_FORLINUX == 1) || (BOARD_FORLINUX == 1) || (BOARD_SABRELITE == 1) || (BOARD_TQIMX6_BASEC == 1) || (BOARD_EMBEDSKY_E9 == 1)
    API_KernelStartParam("ncpus=4 kdlog=no kderror=yes kfpu=no heapchk=yes hz=1000 hhz=1000 "
                         "rfsmap=/boot:/media/sdcard0,/:/media/sdcard1");
#else
    API_KernelStartParam("ncpus=1 kdlog=no kderror=yes kfpu=no heapchk=yes hz=100 hhz=100 "
                         "rfsmap=/boot:/media/sdcard0,/:/media/sdcard1");
#endif
                                                                        /*  操作系统启动参数设置        */

#if 0
    lib_printf("__heap_start: %X\n"
               "  __heap_end: %X\n",
               &__heap_start,
               &__heap_end);
#endif

    API_KernelStart(usrStartup,
                    (PVOID)&__heap_start,
                    (size_t)&__heap_end - (size_t)&__heap_start,
                    LW_NULL, 0);                                        /*  启动内核                    */

    return  (0);                                                        /*  不会执行到这里              */
}
#if LW_CFG_SMP_EN > 0

/*********************************************************************************************************
** 函数名称: halSecondaryCpuMain
** 功能描述: Secondary CPU C 入口
** 输　入  : NONE
** 输　出  : 0
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  halSecondaryCpuMain (VOID)
{
    halModeInit();

    API_KernelSecondaryStart(halSecondaryCpuInit);                      /*  Secondary CPU 启动内核      */

    return  (0);                                                        /*  不会执行到这里              */
}

#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/

