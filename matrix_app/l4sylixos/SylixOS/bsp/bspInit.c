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
** ��   ��   ��: bspInit.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 03 �� 16 ��
**
** ��        ��: BSP �û� C �������.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "config.h"                                                     /*  �������� & ���������       */
/*********************************************************************************************************
  ����ϵͳ���
*********************************************************************************************************/
#include "SylixOS.h"                                                    /*  ����ϵͳ                    */
#include "stdlib.h"                                                     /*  for system() function       */
#include "gdbserver.h"                                                  /*  GDB ������                  */
#include "sys/compiler.h"                                               /*  ���������                  */
#include <linux/mtd/mtd.h>
#include "sys/ioccom.h"
/*********************************************************************************************************
  BSP �� ��������
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
  ����ϵͳ���ű�
*********************************************************************************************************/
#if LW_CFG_SYMBOL_EN > 0 && defined(__GNUC__)
#include "symbol.h"
#endif                                                                  /*  LW_CFG_SYMBOL_EN > 0        */
                                                                        /*  defined(__GNUC__)           */
/*********************************************************************************************************
  �ڴ��ʼ��ӳ���
*********************************************************************************************************/
#define  __BSPINIT_MAIN_FILE
#include "bspMap.h"
/*********************************************************************************************************
  ���߳��������̶߳�ջ (t_boot ���Դ�һ��, startup.sh �п����кܶ����Ķ�ջ�Ĳ���)
*********************************************************************************************************/
#define  __LW_THREAD_BOOT_STK_SIZE      (16 * LW_CFG_KB_SIZE)
#define  __LW_THREAD_MAIN_STK_SIZE      (16 * LW_CFG_KB_SIZE)
/*********************************************************************************************************
  �ⲿ��������
*********************************************************************************************************/
extern VOID  t_main(VOID);
extern VOID  bspCpuUpDone(VOID);
extern VOID  bspCpuDown(ULONG  ulCPUId);
extern SIO_CHAN  *sioChanCreate(INT  iChannel);
/*********************************************************************************************************
** ��������: targetInit
** ��������: ��ʼ�� i.MX6DQ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: ԭ������targetInit.c�ļ��У�����ͳһ����bspInit
*********************************************************************************************************/
VOID  targetInit (VOID)
{
#if 0 //l4
    imx6qCcmPLLInit();

    /*
     * �������쳣������ OCRAM ��ָ��λ��, ���� MMU ʱ����
     */
#define RAM_VECTORS_SIZE    72
#define OCRAM_LEN           (256 * LW_CFG_KB_SIZE)
#define OCRAM               0x00900000

    memcpy((PVOID)(OCRAM + OCRAM_LEN - RAM_VECTORS_SIZE), (PVOID)BSP_CFG_RAM_BASE, RAM_VECTORS_SIZE);
#endif
}
/*********************************************************************************************************
** ��������: halModeInit
** ��������: ��ʼ��Ŀ��ϵͳ���е�ģʽ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  halModeInit (VOID)
{
}
/*********************************************************************************************************
** ��������: halTimeInit
** ��������: ��ʼ��Ŀ��ϵͳʱ��ϵͳ (ϵͳĬ��ʱ��Ϊ: ��8��)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_RTC_EN > 0

static VOID  halTimeInit (VOID)
{
    //boardTimeInit();
}

#endif                                                                  /*  LW_CFG_RTC_EN > 0           */
/*********************************************************************************************************
** ��������: __arm_wfi
** ��������: CPU ������еȴ��ж�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
** ע  ��  : CPU0 ����ʹ�� WFI ָ��.
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
** ��������: __armErrataFix
** ��������: Fix Arm Errata
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: halIdleInit
** ��������: ��ʼ��Ŀ��ϵͳ����ʱ����ҵ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  halIdleInit (VOID)
{
#ifdef IDLE_WFI
    API_SystemHookAdd(__arm_wfi,
                      LW_OPTION_THREAD_IDLE_HOOK);                      /*  ����ʱ��ͣ CPU              */
#endif
}
/*********************************************************************************************************
** ��������: halSmpCoreInit
** ��������: ��ʼ�� SMP ����
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  halSmpCoreInit (VOID)
{
#if 0 //l4
    ULONG  ulCPUId = archMpCur();

    __armErrataFix();                                                   /*  Fix ARM Errata              */

    if (ulCPUId == 0) {
        armScuAccessCtrlSet(0xF);                                       /*  �������� CPU ���� SCU �Ĵ���*/
        armScuFeatureEnable(SCU_FEATURE_SCU);                           /*  ʹ�� SCU                    */
        armScuFeatureEnable(SCU_FEATURE_SCU_SPECULATIVE_LINEFILL);      /*  ʹ��Ͷ���������            */
        armScuFeatureEnable(SCU_FEATURE_SCU_RAMS_PARITY);               /*  ʹ��Ͷ���������            */
        armScuFeatureEnable(SCU_FEATURE_SCU_STANDBY);                   /*  ʹ��Ͷ���������            */
        armScuFeatureEnable(SCU_FEATURE_IC_STANDBY);                    /*  ʹ��Ͷ���������            */
        armScuFeatureDisable(SCU_FEATURE_ADDRESS_FILTERING);            /*  �رյ�ַ����                */
    }

    armScuSecureInvalidateAll(ulCPUId, 0xF);                            /*  ��Ч SCU TAG RAMS COPY      */

    armAuxControlFeatureDisable(AUX_CTRL_A9_L1_PREFETCH);               /*  Errorta 751473              */
    armAuxControlFeatureDisable(AUX_CTRL_A9_L2_PREFETCH);               /*  Errorta 751473              */
    armAuxControlFeatureEnable(AUX_CTRL_A9_SMP);                        /*  ���� SMP                    */
    armAuxControlFeatureEnable(AUX_CTRL_A9_CACHE_MAINTENANCE_BROADCAST);/*  ʹ�� TLB, L1 D-Cache ͬ��   */

    API_CacheEnable(INSTRUCTION_CACHE);                                 /*  ʹ�� L1 I-Cache             */
    API_CacheEnable(DATA_CACHE);                                        /*  ʹ�� L1 D-Cache             */

    KN_SMP_MB();
    API_VmmMmuEnable();                                                 /*  ���� MMU                    */

    API_InterVectorIpi(ulCPUId, ARM_SW_INT_VECTOR(ulCPUId));            /*  ��װ IPI ����               */
    armGicIntVecterEnable(ARM_SW_INT_VECTOR(ulCPUId),
                          LW_FALSE,
                          ARM_SW_INT_PRIORITY, 1 << ulCPUId);           /*  ʹ�� IPI �ж�               */
#endif

    bspCpuUpDone();                                                     /*  ֪ͨ���� CPU ������������� */
}
/*********************************************************************************************************
** ��������: halDisableOtherCpu
** ��������: ����ֹͣ����CPU����֤��ʱֻ������������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  halDisableOtherCpu (VOID)
{
    ULONG  ulCPUId;

    for (ulCPUId = 1; ulCPUId < LW_NCPUS; ulCPUId++) {                  /*  ֹͣ���� CPU                */
        bspCpuDown(ulCPUId);
    }
}
/*********************************************************************************************************
** ��������: halPrimaryCpuInit
** ��������: ��˳���ʼ��Ŀ��ϵͳ�� Primary CPU �� VFP MMU CACHE
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  halPrimaryCpuInit (VOID)
{
    halDisableOtherCpu();

    API_KernelFpuPrimaryInit(ARM_MACHINE_A9, ARM_FPU_VFPv3);            /*  ��ʼ�� FPU ϵͳ             */

    API_VmmLibPrimaryInit(_G_physicalDesc,
                          _G_virtualDesc,
                          ARM_MACHINE_A9);                              /*  ��ʼ�� VMM ϵͳ             */

    API_CacheLibPrimaryInit(CACHE_COPYBACK, CACHE_COPYBACK,
                            ARM_MACHINE_A9);                            /*  ��ʼ�� CACHE ϵͳ           */

    halSmpCoreInit();                                                   /*  ��ʼ�� SMP ����             */
}
#if LW_CFG_SMP_EN > 0
/*********************************************************************************************************
** ��������: halSecondaryCpuInit
** ��������: ��˳���ʼ��Ŀ��ϵͳ�� Secondary CPU �� VFP MMU CACHE
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  halSecondaryCpuInit (VOID)
{
    API_KernelFpuSecondaryInit(ARM_MACHINE_A9, ARM_FPU_VFPv3);          /*  ��ʼ�� FPU ϵͳ             */

    API_VmmLibSecondaryInit(ARM_MACHINE_A9);                            /*  ��ʼ�� VMM ϵͳ             */

    API_CacheLibSecondaryInit(ARM_MACHINE_A9);                          /*  ��ʼ�� CACHE ϵͳ           */

    //armGicCpuInit(LW_FALSE, 255);                                       /*  ��ʼ����ǰ CPU ʹ�� GIC �ӿ�*/

    halSmpCoreInit();                                                   /*  ��ʼ�� SMP ����             */
}

#endif
/*********************************************************************************************************
** ��������: halPmInit
** ��������: ��ʼ��Ŀ��ϵͳ��Դ����ϵͳ
** �䡡��:   NONE
** �䡡��:   NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_POWERM_EN > 0

static VOID  halPmInit (VOID)
{

}

#endif                                                                  /*  LW_CFG_POWERM_EN > 0        */
/*********************************************************************************************************
** ��������: halBusInit
** ��������: ��ʼ��Ŀ��ϵͳ����ϵͳ
** �䡡��:   NONE
** �䡡��:   NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0

static VOID  halBusInit (VOID)
{
    //boardBusInit();
}

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
/*********************************************************************************************************
** ��������: halDrvInit
** ��������: ��ʼ��Ŀ��ϵͳ��̬��������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: halDevInit
** ��������: ��ʼ��Ŀ��ϵͳ��̬�豸���
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_DEVICE_EN > 0

static VOID  halDevInit (VOID)
{
    SIO_CHAN    *psio;
    /*
     *  �������ļ�ϵͳʱ, ���Զ����� dev, mnt, var Ŀ¼.
     */
    rootFsDevCreate();                                                  /*  �������ļ�ϵͳ              */
    procFsDevCreate();                                                  /*  ���� proc �ļ�ϵͳ          */
    shmDevCreate();                                                     /*  ���������ڴ��豸            */
    randDevCreate();                                                    /*  ����������ļ�              */

#define __TTY_RD_BUF_SIZE      256
#define __TTY_WR_BUF_SIZE      256

#if 0 //l4
    psio = sioChanCreate(0);                                            /*  �������� 0 ͨ��             */
    if (psio) {
        ttyDevCreate("/dev/ttyS0", psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);             /*  add    tty   device         */
    }
    psio = sioChanCreate(1);                                            /*  �������� 1 ͨ��             */
    if (psio) {
        ttyDevCreate("/dev/ttyS1", psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);             /*  add    tty   device         */
    }
#else
    psio = sioChanCreate(BSP_CFG_COM_SHELL);                            /*  ��������ͨ��             */
    if (psio) {
        ttyDevCreate(BSP_CFG_STD_FILE, psio,
                     __TTY_RD_BUF_SIZE, __TTY_WR_BUF_SIZE);             /*  add    tty   device         */
    }
#endif

    //boardDevInit();
}

#endif                                                                  /*  LW_CFG_DEVICE_EN > 0        */
/*********************************************************************************************************
** ��������: halLogInit
** ��������: ��ʼ��Ŀ��ϵͳ��־ϵͳ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_LOG_LIB_EN > 0

static VOID  halLogInit (VOID)
{
    fd_set      fdLog;

    FD_ZERO(&fdLog);
    FD_SET(STD_OUT, &fdLog);
    API_LogFdSet(STD_OUT + 1, &fdLog);                                  /*  ��ʼ����־                  */
}

#endif                                                                  /*  LW_CFG_LOG_LIB_EN > 0       */
/*********************************************************************************************************
** ��������: halStdFileInit
** ��������: ��ʼ��Ŀ��ϵͳ��׼�ļ�ϵͳ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: halShellInit
** ��������: ��ʼ��Ŀ��ϵͳ shell ����, (getopt ʹ��ǰһ��Ҫ��ʼ�� shell ����)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_SHELL_EN > 0

static VOID  halShellInit (VOID)
{
    API_TShellInit();

    /*
     *  ��ʼ�� appl �м�� shell �ӿ�
     */
    zlibShellInit();
    viShellInit();

    /*
     *  ��ʼ�� GDB ������
     */
    gdbInit();
}

#endif                                                                  /*  LW_CFG_SHELL_EN > 0         */
/*********************************************************************************************************
** ��������: halNetInit
** ��������: ���������ʼ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_NET_EN > 0

static VOID  halNetInit (VOID)
{
    API_NetInit();                                                      /*  ��ʼ������ϵͳ              */
    API_NetSnmpInit();

    /*
     *  ��ʼ�����總�ӹ���
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
** ��������: halNetifAttch
** ��������: ����ӿ�����
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  halNetifAttch (VOID)
{
    //boardNetifAttch();
}

#endif                                                                  /*  LW_CFG_NET_EN > 0           */
/*********************************************************************************************************
** ��������: halMonitorInit
** ��������: �ں˼�����ϴ���ʼ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_MONITOR_EN > 0

static VOID  halMonitorInit (VOID)
{
    /*
     *  ���������ﴴ���ں˼�����ϴ�ͨ��, Ҳ����ʹ�� shell �������.
     */
}

#endif                                                                  /*  LW_CFG_MONITOR_EN > 0       */
/*********************************************************************************************************
** ��������: halPosixInit
** ��������: ��ʼ�� posix ��ϵͳ (���ϵͳ֧�� proc �ļ�ϵͳ, �������� proc �ļ�ϵͳ��װ֮��!)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_POSIX_EN > 0

static VOID  halPosixInit (VOID)
{
    API_PosixInit();
}

#endif                                                                  /*  LW_CFG_POSIX_EN > 0         */
/*********************************************************************************************************
** ��������: halSymbolInit
** ��������: ��ʼ��Ŀ��ϵͳ���ű���, (Ϊ module loader �ṩ����)
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: halLoaderInit
** ��������: ��ʼ��Ŀ��ϵͳ�����ģ��װ����
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
#if LW_CFG_MODULELOADER_EN > 0

static VOID  halLoaderInit (VOID)
{
    API_LoaderInit();
}

#endif                                                                  /*  LW_CFG_SYMBOL_EN > 0        */
/*********************************************************************************************************
** ��������: halBootThread
** ��������: ������״̬�µĳ�ʼ����������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static PVOID  halBootThread (PVOID  pvBootArg)
{
    LW_CLASS_THREADATTR     threakattr = API_ThreadAttrGetDefault();    /*  ʹ��Ĭ������                */
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
    console_loglevel = default_message_loglevel;                        /*  ���� printk ��ӡ��Ϣ�ȼ�    */
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
#endif                                                                  /*  �ο��������                */

#if LW_CFG_DEVICE_EN > 0                                                /*  map rootfs                  */
    rootFsMap(LW_ROOTFS_MAP_LOAD_VAR | LW_ROOTFS_MAP_SYNC_TZ | LW_ROOTFS_MAP_SET_TIME);
#endif

    /*
     *  �����ʼ��һ����� shell ��ʼ��֮��, ��Ϊ��ʼ���������ʱ, ���Զ�ע�� shell ����.
     */
#if LW_CFG_NET_EN > 0
    halNetInit();
    bspDebugMsg("-------halNetInit()-------\n");
    halNetifAttch();                                                    /*  wlan ������Ҫ���ع̼�       */
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

    for (ulCPUId = 1; ulCPUId < LW_NCPUS; ulCPUId++) {                  /*  �������� CPU                */
        //API_CpuUp(ulCPUId);
    }

    API_CacheClear(DATA_CACHE, 0, (size_t)~0);

#if LW_CFG_SHELL_EN > 0
    tshellStartup();                                                    /*  ִ�������ű�                */
#endif

    API_ThreadAttrSetStackSize(&threakattr, __LW_THREAD_MAIN_STK_SIZE); /*  ���� main �̵߳Ķ�ջ��С    */
    API_ThreadCreate("t_main",
                     (PTHREAD_START_ROUTINE)t_main,
                     &threakattr,
                     LW_NULL);                                          /*  Create "t_main()" thread    */

    return  (LW_NULL);
}
/*********************************************************************************************************
** ��������: usrStartup
** ��������: ��ʼ��Ӧ��������, ��������ϵͳ�ĵ�һ������.
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  usrStartup (VOID)
{
    LW_CLASS_THREADATTR     threakattr;

    /*
     *  ע��, ��Ҫ�޸ĸó�ʼ��˳�� (�����ȳ�ʼ�� vmm ������ȷ�ĳ�ʼ�� cache, �����������ʼ��)
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
  openocd ��ز���
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
** ��������: bspOpenocdInit
** ��������: ��ʼ�� openocd
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
** ע  ��  : ����Ҫ�� pOpenocdParam->uiFlags = htole32(1); ��䴦��һ���ϵ�,
**           ���ҵ���ִ�� pOpenocdParam->uiFlags = htole32(1); ���.
**           ����ر� D-Cache, ���� API_CacheEnable(DATA_CACHE); ���.
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
** ��������: halPrimaryCpuMain
** ��������: Primary CPU C ���
** �䡡��  : NONE
** �䡡��  : 0
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  halPrimaryCpuMain (VOID)
{
    /*
     *  ϵͳ�ں˶���ϵͳ��
     */
    extern UCHAR  __heap_start, __heap_end;

#if 0 //l4
    bspOpenocdInit();                                                   /*  ��ʼ�� openocd              */
#endif

    halModeInit();                                                      /*  ��ʼ��Ӳ��                  */

    /*
     *  ����ĵ��Զ˿����������ϵͳ��, ������Ӧ�ò������ڲ���ϵͳ������.
     *  ��ϵͳ���ִ���ʱ, ����˿��Ե���Ϊ�ؼ�. (��Ŀ��������ͨ�����ùص�)
     *  (!!��ǰ�����Ѿ���bootloader��ʼ����, �������账��.)
     */

    /*
     *  ����ʹ�� bsp ������������, ��� bootloader ֧��, ��ʹ�� bootloader ����.
     *  Ϊ�˼�����ǰ����Ŀ, ���� kfpu=yes �����ں���(�����ж�)ʹ�� FPU.
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
                                                                        /*  ����ϵͳ������������        */

#if 0
    lib_printf("__heap_start: %X\n"
               "  __heap_end: %X\n",
               &__heap_start,
               &__heap_end);
#endif

    API_KernelStart(usrStartup,
                    (PVOID)&__heap_start,
                    (size_t)&__heap_end - (size_t)&__heap_start,
                    LW_NULL, 0);                                        /*  �����ں�                    */

    return  (0);                                                        /*  ����ִ�е�����              */
}
#if LW_CFG_SMP_EN > 0

/*********************************************************************************************************
** ��������: halSecondaryCpuMain
** ��������: Secondary CPU C ���
** �䡡��  : NONE
** �䡡��  : 0
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  halSecondaryCpuMain (VOID)
{
    halModeInit();

    API_KernelSecondaryStart(halSecondaryCpuInit);                      /*  Secondary CPU �����ں�      */

    return  (0);                                                        /*  ����ִ�е�����              */
}

#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/

