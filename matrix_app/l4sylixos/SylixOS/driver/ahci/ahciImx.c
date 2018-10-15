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
** 文   件   名: ahciImx.c
**
** 创   建   人: Gong.YuJian (弓羽箭)
**
** 文件创建日期: 2016 年 03 月 17 日
**
** 描        述: Freescale IMX 系列 AHCI 驱动.
*********************************************************************************************************/
#define  __SYLIXOS_AHCI_DRV
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
/*********************************************************************************************************
  裁剪宏
*********************************************************************************************************/
#if (LW_CFG_DEVICE_EN > 0) && (LW_CFG_AHCI_EN > 0)
#include "linux/compat.h"
#include "ahciImx.h"
/*********************************************************************************************************
  驱动名称
*********************************************************************************************************/
#define AHCI_IMX_DRV_NAME                   "ahci_imx6"                 /* i.MX 6Dual/6Quad 类型        */
#define AHCI_IMX_DRV_VER_NUM                0x01000001                  /* 驱动版本数值                 */
/*********************************************************************************************************
  控制器参数
*********************************************************************************************************/
#define AHCI_IMX_CTRL_MAX                   1                           /* 控制器数量最大值             */
#define AHCI_IMX_DRIVE_MAX                  1                           /* 驱动器数量最大值             */

#define AHCI_IMX_BASE_ADDR                  0x02200000
#define AHCI_IMX_BASE_SIZE                  0x0180
#define AHCI_IMX_IRQ_VECTOR                 71
/*********************************************************************************************************
  IMX 控制器私有寄存器
*********************************************************************************************************/
#define AHCI_IMX_BISTAFR                    0xa0
#define AHCI_IMX_BISTCR                     0xa4
#define AHCI_IMX_BISTFCTR                   0xa8
#define AHCI_IMX_BISTSR                     0xac
#define AHCI_IMX_OOBR                       0xbc
#define AHCI_IMX_GPCR                       0xd0
#define AHCI_IMX_GPSR                       0xd4
#define AHCI_IMX_TIMER1MS                   0xe0
#define AHCI_IMX_TESTR                      0xf4
#define AHCI_IMX_VERSIONR                   0xf8

#define AHCI_IMX_OOBR_WE                    0x80000000

#define AHCI_IMX_PxDMACR                    0x70
#define AHCI_IMX_PxPHYCR                    0x78
#define AHCI_IMX_PxPHYSR                    0x7c
/*********************************************************************************************************
  GPIO 引脚定义
*********************************************************************************************************/
#define AHCI_IMX_GPIO_NUMR(bank, gpio)      (32 * (bank - 1) + (gpio))
/*********************************************************************************************************
  平台相关
*********************************************************************************************************/
typedef union ahci_imx_ccm_pll
{
    UINT32      U;
    struct imx_pll_enet_bitfields
    {
        unsigned DIV_SELECT : 2;
        unsigned RESERVED0 : 10;
        unsigned POWERDOWN : 1;
        unsigned ENABLE : 1;
        unsigned BYPASS_CLK_SRC : 2;
        unsigned BYPASS : 1;
        unsigned RESERVED1 : 1;
        unsigned PFD_OFFSET_EN : 1;
        unsigned ENABLE_125M : 1;
        unsigned ENABLE_100M : 1;
        unsigned RESERVED2 : 10;
        unsigned LOCK : 1;
    } B;
} AHCI_IMX_CCM_PLL_TCB;

#ifndef AHCI_IMX_REGS_BASE
#define AHCI_IMX_INSTANCE_COUNT             (1)
#define AHCI_IMX_REGS_BASE                  (0x020c8000)
#endif
#define AHCI_IMX_PLL_ENET_ADDR              (AHCI_IMX_REGS_BASE + 0xe0)
#define AHCI_IMX_PLL_ENET_SET_ADDR          (AHCI_IMX_PLL_ENET_ADDR + 0x4)
#define AHCI_IMX_PLL_ENET_CLR_ADDR          (AHCI_IMX_PLL_ENET_ADDR + 0x8)
#define AHCI_IMX_PLL_ENET_TOG_ADDR          (AHCI_IMX_PLL_ENET_ADDR + 0xC)

#ifndef __LANGUAGE_ASM__
#define AHCI_IMX_PLL_ENET                   (*(volatile AHCI_IMX_CCM_PLL_TCB *)AHCI_IMX_PLL_ENET_ADDR)
#define AHCI_IMX_PLL_ENET_RD()              (AHCI_IMX_PLL_ENET.U)
#define AHCI_IMX_PLL_ENET_WR(v)             (AHCI_IMX_PLL_ENET.U = (v))
#define AHCI_IMX_PLL_ENET_SET(v)            ((*(volatile UINT32 *)AHCI_IMX_PLL_ENET_SET_ADDR) = (v))
#define AHCI_IMX_PLL_ENET_CLR(v)            ((*(volatile UINT32 *)AHCI_IMX_PLL_ENET_CLR_ADDR) = (v))
#define AHCI_IMX_PLL_ENET_TOG(v)            ((*(volatile UINT32 *)AHCI_IMX_PLL_ENET_TOG_ADDR) = (v))
#endif

#define AHCI_IMX_BP_PLL_ENET_POWERDOWN      (12)
#define AHCI_IMX_BM_PLL_ENET_POWERDOWN      (0x00001000)

#define AHCI_IMX_BP_PLL_ENET_ENABLE         (13)
#define AHCI_IMX_BM_PLL_ENET_ENABLE         (0x00002000)

#define AHCI_IMX_BP_PLL_ENET_BYPASS         (16)
#define AHCI_IMX_BM_PLL_ENET_BYPASS         (0x00010000)

#define AHCI_IMX_BP_PLL_ENET_ENABLE_100M    (20)
#define AHCI_IMX_BM_PLL_ENET_ENABLE_100M    (0x00100000)

/*********************************************************************************************************
** 函数名称: imxAhciCtrlOpt
** 功能描述: 驱动相关的选项操作
** 输　入  : hCtrl      控制器句柄
**           uiDrive    驱动器索引
**           iCmd       命令 (AHCI_OPT_CMD_xxxx)
**           lArg       参数
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imxAhciCtrlOpt (AHCI_CTRL_HANDLE  hCtrl, UINT  uiDrive, INT  iCmd, LONG  lArg)
{
    if (!hCtrl) {
        return  (PX_ERROR);
    }

    if ((hCtrl->AHCICTRL_uiUnitIndex != 0) ||
        (uiDrive != 0)) {
        return  (PX_ERROR);
    }

    switch (iCmd) {

    case AHCI_OPT_CMD_BASE_ADDR_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_IMX_BASE_ADDR;
        }
        break;

    case AHCI_OPT_CMD_BASE_SIZE_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_IMX_BASE_SIZE;
        }
        break;

    case AHCI_OPT_CMD_VECTOR_NUM_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_IMX_IRQ_VECTOR;
        }
        break;

    case AHCI_OPT_CMD_SECTOR_START_GET:
        if (lArg) {
            *(ULONG *)lArg = 0;
        }
        break;

    case AHCI_OPT_CMD_SECTOR_SIZE_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_SECTOR_SIZE;
        }
        break;

    case AHCI_OPT_CMD_CACHE_BURST_RD_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_CACHE_BURST_RD;
        }
        break;

    case AHCI_OPT_CMD_CACHE_BURST_WR_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_CACHE_BURST_WR;
        }
        break;

    case AHCI_OPT_CMD_CACHE_SIZE_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_CACHE_SIZE;
        }
        break;

    case AHCI_OPT_CMD_PROB_TIME_UNIT_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_DRIVE_PROB_TIME_UNIT;
        }
        break;

    case AHCI_OPT_CMD_PROB_TIME_COUNT_GET:
        if (lArg) {
            *(ULONG *)lArg = AHCI_DRIVE_PROB_TIME_COUNT;
        }
        break;

    default:
        return  (PX_ERROR);
        break;
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorDriveInfoShow
** 功能描述: 驱动器设备厂商自定义信息
** 输　入  : hCtrl      控制器句柄
**           uiDrive    驱动器号
**           hParam     参数控制块句柄
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorDriveInfoShow (AHCI_CTRL_HANDLE  hCtrl, UINT  uiDrive, AHCI_PARAM_HANDLE  hParam)
{
    return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __imxAhciVerdorDriveRegNameGet
** 功能描述: 获取驱动器设备厂商自定义端口寄存器名
** 输　入  : hDrive     驱动器句柄
**           uiOffset   端口寄存器偏移
** 输　出  : 寄存器名称缓冲区
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static PCHAR  imxAhciVendorDriveRegNameGet (AHCI_DRIVE_HANDLE  hDrive, UINT32  uiOffset)
{
    switch (uiOffset) {

    case AHCI_IMX_PxDMACR:
        return  ("Port x DMA Control Register (AHCI_IMX_PxDMACR)");
        break;

    case AHCI_IMX_PxPHYCR:
        return  ("Port x PHY Control Register (AHCI_IMX_PxPHYCR)");
        break;

    case AHCI_IMX_PxPHYSR:
        return  ("Port x PHY Status Register (AHCI_IMX_PxPHYSR)");
        break;

    default:
        break;
    }

    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorDriveInit
** 功能描述: 厂商驱动器特殊处理
** 输　入  : hDrive      驱动器句柄
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorDriveInit (AHCI_DRIVE_HANDLE  hDrive)
{
    INT         iRet;
    UINT32      uiReg;

    AHCI_PORT_REG_MSG(hDrive, AHCI_PxCMD);
    uiReg  = AHCI_PORT_READ(hDrive, AHCI_PxCMD);
    uiReg &= ~(AHCI_PCMD_SUD);
    uiReg &= ~(AHCI_PCMD_POD);
    uiReg &= ~(AHCI_PCMD_ST);
    AHCI_PORT_WRITE(hDrive, AHCI_PxCMD, uiReg);
    AHCI_PORT_REG_MSG(hDrive, AHCI_PxCMD);

    iRet = API_AhciDriveRegWait(hDrive, AHCI_PxCMD, AHCI_PCMD_CR, LW_TRUE, AHCI_PCMD_CR, 20, 50, &uiReg);
    if (iRet != ERROR_NONE) {
        return (PX_ERROR);
    }

    AHCI_PORT_REG_MSG(hDrive, AHCI_PxCMD);
    uiReg  = AHCI_PORT_READ(hDrive, AHCI_PxCMD);
    uiReg |= AHCI_PCMD_ST;
    AHCI_PORT_WRITE(hDrive, AHCI_PxCMD, uiReg);
    AHCI_PORT_REG_MSG(hDrive, AHCI_PxCMD);

    AHCI_PORT_REG_MSG(hDrive, AHCI_IMX_PxDMACR);
    AHCI_PORT_WRITE(hDrive, AHCI_IMX_PxDMACR, 0x0000aaaa);
    AHCI_PORT_REG_MSG(hDrive, AHCI_IMX_PxDMACR);

    return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorCtrlInfoShow
** 功能描述: 厂商自定义控制器信息打印
** 输　入  : hCtrl      控制器句柄
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorCtrlInfoShow (AHCI_CTRL_HANDLE  hCtrl)
{
    return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __imxAhciVendorCtrlRegNameGet
** 功能描述: 获取厂商自定义控制器寄存器名
** 输　入  : hCtrl      控制器句柄
**           uiOffset   全局寄存器偏移
** 输　出  : 寄存器名称缓冲区
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static PCHAR  imxAhciVendorCtrlRegNameGet (AHCI_CTRL_HANDLE  hCtrl, UINT32  uiOffset)
{
    switch (uiOffset) {

    case AHCI_IMX_BISTAFR:
        return  ("BIST Activate FIS Register (AHCI_IMX_BISTAFR)");
        break;

    case AHCI_IMX_BISTCR:
        return  ("BIST Control Register (AHCI_IMX_BISTCR)");
        break;

    case AHCI_IMX_BISTFCTR:
        return  ("BIST FIS Count Register (AHCI_IMX_BISTFCTR)");
        break;

    case AHCI_IMX_BISTSR:
        return  ("BIST Status Register (AHCI_IMX_BISTSR)");
        break;

    case AHCI_IMX_OOBR:
        return  ("OOB Register (AHCI_IMX_OOBR)");
        break;

    case AHCI_IMX_GPCR:
        return  ("General Purpose Control Register (AHCI_IMX_GPCR)");
        break;

    case AHCI_IMX_GPSR:
        return  ("General Purpose Status Register (AHCI_IMX_GPSR)");
        break;

    case AHCI_IMX_TIMER1MS:
        return  ("Timer 1-ms Register (AHCI_IMX_TIMER1MS)");
        break;

    case AHCI_IMX_TESTR:
        return  ("Test Register (AHCI_IMX_TESTR)");
        break;

    case AHCI_IMX_VERSIONR:
        return  ("Version Register (AHCI_IMX_VERSIONR)");
        break;

    default:
        break;
    }

    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorCtrlTypeNameGet
** 功能描述: 获取控制器类型名称
** 输　入  : hCtrl      控制器句柄
** 输　出  : 控制器类型名称
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static PCHAR  imxAhciVendorCtrlTypeNameGet (AHCI_CTRL_HANDLE  hCtrl)
{
    PCHAR       pcType;

    pcType = "SATA";

    return  (pcType);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorCtrlIntEnable
** 功能描述: 使能控制器中断
** 输　入  : hCtrl      控制器句柄
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorCtrlIntEnable (AHCI_CTRL_HANDLE  hCtrl)
{
    API_InterVectorEnable(hCtrl->AHCICTRL_ulIrqVector);

    return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorCtrlIntConnect
** 功能描述: 链接控制器中断
** 输　入  : hCtrl      控制器句柄
**           pfuncIsr   中断服务函数
**           cpcName    中断服务名称
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorCtrlIntConnect (AHCI_CTRL_HANDLE  hCtrl,
                                         PINT_SVR_ROUTINE  pfuncIsr,
                                         CPCHAR            cpcName)
{
    API_InterVectorConnect(hCtrl->AHCICTRL_ulIrqVector, pfuncIsr, hCtrl, cpcName);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorCtrlInit
** 功能描述: 厂商控制器特殊处理
** 输　入  : hCtrl    控制器句柄
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorCtrlInit (AHCI_CTRL_HANDLE  hCtrl)
{
    REGISTER INT    i;
    UINT32          uiReg;
    UINT32          uiPortCount;

    AHCI_CTRL_REG_MSG(hCtrl, AHCI_IMX_TIMER1MS);
    AHCI_CTRL_WRITE(hCtrl, AHCI_IMX_TIMER1MS, 133000);
    AHCI_CTRL_REG_MSG(hCtrl, AHCI_IMX_TIMER1MS);

    AHCI_CTRL_REG_MSG(hCtrl, AHCI_IMX_OOBR);
    AHCI_CTRL_WRITE(hCtrl, AHCI_IMX_OOBR, AHCI_IMX_OOBR_WE);
    AHCI_CTRL_REG_MSG(hCtrl, AHCI_IMX_OOBR);
    AHCI_CTRL_WRITE(hCtrl, AHCI_IMX_OOBR, 0x02060b14);
    AHCI_CTRL_REG_MSG(hCtrl, AHCI_IMX_OOBR);

    /*
     *  标准 AHCI 控制器 PI 字段是只读的
     */
    AHCI_CTRL_REG_MSG(hCtrl, AHCI_CAP);
    uiReg = AHCI_CTRL_READ(hCtrl, AHCI_CAP);
    uiPortCount = (uiReg & AHCI_CAP_NP) + 1;
    for (i = 0; i < uiPortCount; i++) {
        AHCI_CTRL_REG_MSG(hCtrl, AHCI_PI);
        AHCI_CTRL_WRITE(hCtrl, AHCI_PI, (1 << i));
        AHCI_CTRL_REG_MSG(hCtrl, AHCI_PI);
    }

    AHCI_CTRL_REG_MSG(hCtrl, AHCI_GHC);
    uiReg = AHCI_CTRL_READ(hCtrl, AHCI_GHC);
    uiReg &= ~(AHCI_GHC_AE);
    AHCI_CTRL_WRITE(hCtrl, AHCI_GHC, uiReg);
    AHCI_CTRL_REG_MSG(hCtrl, AHCI_GHC);

    return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorCtrlReadyWork
** 功能描述: 控制器准备操作
** 输　入  : hCtrl    控制器句柄
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorCtrlReadyWork (AHCI_CTRL_HANDLE  hCtrl)
{
    INT                 iRet = PX_ERROR;
    AHCI_DRV_HANDLE     hDrv = LW_NULL;

    hDrv = hCtrl->AHCICTRL_hDrv;
    iRet = hDrv->AHCIDRV_pfuncOptCtrl(hCtrl, 0, AHCI_OPT_CMD_BASE_ADDR_GET,
                                      (LONG)((ULONG *)&hCtrl->AHCICTRL_pvRegAddr));
    if (iRet != ERROR_NONE) {
        return  (PX_ERROR);
    }
    iRet = hDrv->AHCIDRV_pfuncOptCtrl(hCtrl, 0, AHCI_OPT_CMD_BASE_SIZE_GET,
                                      (LONG)((ULONG *)&hCtrl->AHCICTRL_stRegSize));
    if (iRet != ERROR_NONE) {
        return  (PX_ERROR);
    }

    hCtrl->AHCICTRL_pvRegAddr = API_VmmIoRemapNocache(hCtrl->AHCICTRL_pvRegAddr,
                                                      hCtrl->AHCICTRL_stRegSize);
    if (!hCtrl->AHCICTRL_pvRegAddr) {
        return  (PX_ERROR);
    }

    iRet = hDrv->AHCIDRV_pfuncOptCtrl(hCtrl, 0, AHCI_OPT_CMD_VECTOR_NUM_GET,
                                      (LONG)((ULONG *)&hCtrl->AHCICTRL_ulIrqVector));
    if (iRet != ERROR_NONE) {
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorPlatformInit
** 功能描述: 控制器平台相关初始化
** 输　入  : hCtrl     控制器句柄
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorPlatformInit (AHCI_CTRL_HANDLE  hCtrl)
{
    UINT32  uiReg;

    write32(0xFFFFFFFF, 0x020C407C);                                    /* enable SATA_CLK in CCGR5     */

    AHCI_IMX_PLL_ENET_CLR(AHCI_IMX_BM_PLL_ENET_POWERDOWN);              /* clr PWDN bit 12              */
    AHCI_IMX_PLL_ENET_SET(AHCI_IMX_BM_PLL_ENET_ENABLE);                 /* set PLL enable bit 13        */
    API_TimeMSleep(1);
    AHCI_IMX_PLL_ENET_CLR(AHCI_IMX_BM_PLL_ENET_BYPASS);                 /* clr BYPASS bit 16            */
    AHCI_IMX_PLL_ENET_SET(AHCI_IMX_BM_PLL_ENET_ENABLE_100M);
    API_TimeMSleep(1);
    AHCI_IMX_PLL_ENET.B.DIV_SELECT = 0x2;                               /* 0b10-100MHz                  */

    uiReg = read32(0x020E0034);                                         /* read GPR13                   */
    uiReg &= 0x07FF7FFF;
    uiReg |= 0x0593A4C6;
    write32(uiReg, 0x020E0034);                                         /* 设置 sata 为 3G              */

    return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imxAhciVendorDrvReadyWork
** 功能描述: 驱动注册前准备操作
** 输　入  : hDrv      驱动控制句柄
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
static INT  imxAhciVendorDrvReadyWork (AHCI_DRV_HANDLE  hDrv)
{
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: imxAhciDevInit
** 功能描述: IMX 类型控制器驱动相关初始化
** 输　入  : NONE
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
                                           API 函数
*********************************************************************************************************/
LW_API INT  imxAhciDevInit (VOID)
{
    AHCI_DRV_CB         tDrvReg;
    AHCI_DRV_HANDLE     hDrvReg = &tDrvReg;

    API_AhciDrvInit();

    lib_strlcpy(&hDrvReg->AHCIDRV_cDrvName[0], AHCI_IMX_DRV_NAME, AHCI_DRV_NAME_MAX);

    hDrvReg->AHCIDRV_uiDrvVer                   = AHCI_IMX_DRV_VER_NUM;
    hDrvReg->AHCIDRV_pfuncOptCtrl               = imxAhciCtrlOpt;
    hDrvReg->AHCIDRV_pfuncVendorDriveInfoShow   = imxAhciVendorDriveInfoShow;
    hDrvReg->AHCIDRV_pfuncVendorDriveRegNameGet = imxAhciVendorDriveRegNameGet;
    hDrvReg->AHCIDRV_pfuncVendorDriveInit       = imxAhciVendorDriveInit;
    hDrvReg->AHCIDRV_pfuncVendorCtrlInfoShow    = imxAhciVendorCtrlInfoShow;
    hDrvReg->AHCIDRV_pfuncVendorCtrlRegNameGet  = imxAhciVendorCtrlRegNameGet;
    hDrvReg->AHCIDRV_pfuncVendorCtrlTypeNameGet = imxAhciVendorCtrlTypeNameGet;
    hDrvReg->AHCIDRV_pfuncVendorCtrlIntEnable   = imxAhciVendorCtrlIntEnable;
    hDrvReg->AHCIDRV_pfuncVendorCtrlIntConnect  = imxAhciVendorCtrlIntConnect;
    hDrvReg->AHCIDRV_pfuncVendorCtrlInit        = imxAhciVendorCtrlInit;
    hDrvReg->AHCIDRV_pfuncVendorCtrlReadyWork   = imxAhciVendorCtrlReadyWork;
    hDrvReg->AHCIDRV_pfuncVendorPlatformInit    = imxAhciVendorPlatformInit;
    hDrvReg->AHCIDRV_pfuncVendorDrvReadyWork    = imxAhciVendorDrvReadyWork;

    API_AhciDrvRegister(hDrvReg);

    API_AhciCtrlCreate(AHCI_IMX_DRV_NAME, 0, LW_NULL);

    return  (ERROR_NONE);
}
#endif                                                                  /*  (LW_CFG_DEVICE_EN > 0) &&   */
                                                                        /*  (LW_CFG_AHCI_EN > 0)        */
/*********************************************************************************************************
  END
*********************************************************************************************************/
