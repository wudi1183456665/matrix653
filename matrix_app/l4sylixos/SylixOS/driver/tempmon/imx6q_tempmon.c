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
** 文   件   名: imx6q_tempmon.c
**
** 创   建   人: Zhang.Xu
**
** 文件创建日期: 2016 年 03 月 05 日
**
** 描        述: imx6q 处理器的温度测试功能，能够实现单次读取或报警设置
**               int  fd, ret, read_temp;
**               fd = open("/dev/temp0", O_RDWR, 0666);
**               ret = read(fd, &read_temp, 4);      单次读取温度设置
**               ioctl(fd, TEMP_ALARMEN_CMD, 75);    设置报警温度是75度
**               ioctl(fd, TEMP_ALARMDIS_CMD, NULL); 禁止温度报警
**               close(fd);
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"

#include "tempmon/imx6q_tempmon.h"
#include "sdk.h"
/*********************************************************************************************************
  WATCH_DOG 寄存器定义,寄存器为 16bits，而不是 32bits，
*********************************************************************************************************/
#define TEMPMON_BASE           (0x020C8000)
#define PMU_MISC1_BASE         (0x020C8160)

#define TEMPMONCTL0            (0x180)
#define TEMPMONCTL0_SET        (0x184)
#define TEMPMONCTL0_CLR        (0x188)
#define TEMPMONCTL0_TOG        (0x18C)
#define TEMPMONCTL1            (0x190)
#define TEMPMONCTL1_SET        (0x194)
#define TEMPMONCTL1_CLR        (0x198)
#define TEMPMONCTL1_TOG        (0x19C)
#define PMU_MISC1              (0x160)
#define PMU_MISC1_SET          (0x164)
#define PMU_MISC1_CLR          (0x168)
#define PMU_MISC1_TOG          (0x16C)

#define BM_ROOM_COUNT          (0xfff00000)
#define BP_ROOM_COUNT          (20)
#define BM_HOT_COUNT           (0x000fff00)
#define BP_HOT_COUNT           (8)
#define BM_HOT_TEMP            (0x000000ff)
#define BP_HOT_TEMP            (0)
#define ROOM_TEMP              (25.0f)
#define DEFAULT_TEMP_CAL_DATA  (0x57d4df7d)
/*********************************************************************************************************
  WATCH_DOG 控制器类型定义
*********************************************************************************************************/
typedef struct {
    LW_DEV_HDR                  TEMP_devHdr;                            /* 必须是第一个结构体成员       */
    LW_LIST_LINE_HEADER         TEMP_fdNodeHeader;
	addr_t                      TEMP_BaseAddr;
	INT                         TEMP_iIrqNum;
} __IMX6Q_TEMP_CONTROLER, *__PIMX6Q_TEMP_CONTROLER;

static UINT32                  _G_iTempDrvNum = 0;
static __IMX6Q_TEMP_CONTROLER  _G_tempctller;
/*********************************************************************************************************
** 函数名称: imx6qComputeTemp
** 功能描述: 将读取的 TEMPMON 数值转换成温度值
** 输　入  : fTempCnt   读取的温度传感器原始数值
** 输　出  : 返回当前 CPU 的温度值，单位 C
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static float imx6qComputeTemp (float fTempCnt)
{
	float fRoomCount = (DEFAULT_TEMP_CAL_DATA & BM_ROOM_COUNT) >> BP_ROOM_COUNT;
	float fHotCount  = (DEFAULT_TEMP_CAL_DATA & BM_HOT_COUNT) >> BP_HOT_COUNT;
	float fHotTemp   = (DEFAULT_TEMP_CAL_DATA & BM_HOT_TEMP) >> BP_HOT_TEMP;

    float fA = (fHotTemp - ROOM_TEMP);
    float fB = (fRoomCount - fHotCount);
    float fC = fA / fB;
    float fD = (fTempCnt - fHotCount);
    float fE = fD * fC;
    float fF = fHotTemp - fE;
    return fF;
}
/*********************************************************************************************************
** 函数名称: imx6qComputeAlarm
** 功能描述: 将设置的的报警温度值转换成寄存器的设置值
** 输　入  : fAlarmTemp   设置的报警值
** 输　出  : 返回需要设置的寄存器设置值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static UINT32  imx6qComputeAlarm (float fAlarmTemp)
{
	float  fRoomCount = (DEFAULT_TEMP_CAL_DATA & BM_ROOM_COUNT) >> BP_ROOM_COUNT;
	float  fHotCount  = (DEFAULT_TEMP_CAL_DATA & BM_HOT_COUNT) >> BP_HOT_COUNT;
	float  fHotTemp   = (DEFAULT_TEMP_CAL_DATA & BM_HOT_TEMP) >> BP_HOT_TEMP;

    float  fA = (fAlarmTemp - fHotTemp);
    float  fB = (fHotTemp - ROOM_TEMP);
    float  fC = (fRoomCount - fHotCount);
    float  fD = (fB / fC);

    return (fHotCount + fA / fD);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonIsr
** 功能描述: 温度监控报警中断函数
** 输　入  : pvArg   :  中断参数
**           uiVector:  中断向量号
** 输　出  : 中断返回值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static irqreturn_t  imx6qTempMonIsr (PVOID  pvArg, UINT32  uiVector)
{
	__PIMX6Q_TEMP_CONTROLER  pTempDev   = (__PIMX6Q_TEMP_CONTROLER)pvArg;
    addr_t                   atBaseAddr = pTempDev->TEMP_BaseAddr;

    writel((1 << 29), (atBaseAddr + PMU_MISC1));                        /*  清除tempsense的中断        */

    return  (LW_IRQ_HANDLED);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonAlarmInit
** 功能描述: 温度监控报警中断初始化函数
** 输　入  : pTempDev   :  控制器私有数据结构体
**           uiAlarmLvl :  需要设置的报警温度
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  imx6qTempMonAlarmInit (__PIMX6Q_TEMP_CONTROLER  pTempDev, UINT32 uiAlarmLvl)
{
	addr_t  atBaseAddr = pTempDev->TEMP_BaseAddr;
	UINT32  uiRegVal;
	INT     iRet;

    /*
     * Wake up the temp monitor
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0x000FFFFC;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);

    /*
     * set the measure frequency 0.5Hz,because temp change slowly
     */
    writel(0xFFFF, atBaseAddr + TEMPMONCTL1);

    /*
     * Calculate and fill in the alarm value.
     */
    uiRegVal = imx6qComputeAlarm((float)uiAlarmLvl);
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) | uiRegVal;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);

    /*
     * clear irq and init it
     */
    writel((1 << 29), atBaseAddr + PMU_MISC1);
    iRet = API_InterVectorConnect((ULONG)pTempDev->TEMP_iIrqNum,
                                  (PINT_SVR_ROUTINE)imx6qTempMonIsr,
                                  (PVOID)pTempDev,
                                  "temp_isr");
    if (iRet != ERROR_NONE) {
        printk(KERN_ERR "failed to connect temp_isr!\n");
    }
    API_InterVectorEnable(pTempDev->TEMP_iIrqNum);

    /*
     * Start  measurement
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) | 0x2;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonAlarmInit
** 功能描述: 温度监控报警中断初始化函数
** 输　入  : pTempDev   :  控制器私有数据结构体
**           uiAlarmLvl :  需要设置的报警温度
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  imx6qTempMonAlarmDisable (__PIMX6Q_TEMP_CONTROLER  pTempDev)
{
	addr_t  atBaseAddr = pTempDev->TEMP_BaseAddr;
	UINT32  uiRegVal;

	/*
	 * 禁用中断，并取消中断函数注册
	 */
    API_InterVectorDisable(pTempDev->TEMP_iIrqNum);
    API_InterVectorDisconnect((ULONG)pTempDev->TEMP_iIrqNum,
    		                  (PINT_SVR_ROUTINE)imx6qTempMonIsr,
    		                  (PVOID)pTempDev);

	/*
	 * 清除温度测量频率设置，并禁用温度测量模块
	 */
    writel(0x0, atBaseAddr + TEMPMONCTL1);
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0x000FFFFD;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonOpen
** 功能描述: 打开 WATCH_DOG 设备
** 输　入  : pDev                  设备
**           pcName                设备名字
**           iFlags                标志
**           iMode                 模式
** 输　出  : 文件节点
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LONG  imx6qTempMonOpen (__PIMX6Q_TEMP_CONTROLER pTempDev, PCHAR pcName, INT iFlags, INT iMode)
{
    PLW_FD_NODE   pFdNode;
    BOOL          bIsNew;

    if (pcName == LW_NULL) {
        _ErrorHandle(ERROR_IO_NO_DEVICE_NAME_IN_PATH);
        return  (PX_ERROR);

    } else {
        pFdNode = API_IosFdNodeAdd(&pTempDev->TEMP_fdNodeHeader, (dev_t)pTempDev, 0,
                                   iFlags, iMode, 0, 0, 0, LW_NULL, &bIsNew);
        if (pFdNode == LW_NULL) {
            printk(KERN_ERR "imx6qWdtOpen(): failed to add fd node!\n");
            return  (PX_ERROR);
        }

        if (LW_DEV_INC_USE_COUNT(&pTempDev->TEMP_devHdr) == 1) {
            /*
             * 添加设备初始化和使能操作,temp 似乎不需要什么初始化
             */

            return  ((LONG)pFdNode);
        }
    }
    /*
     * 如果设备打开失败进行如下操作
     */
    if (pFdNode) {
        API_IosFdNodeDec(&pTempDev->TEMP_fdNodeHeader, pFdNode, NULL);
        pFdNode = LW_NULL;
    }
    LW_DEV_DEC_USE_COUNT(&pTempDev->TEMP_devHdr);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonClose
** 功能描述: 关闭 WATCH_DOG 设备
** 输　入  : pFdEntry              文件结构
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT imx6qTempMonClose (PLW_FD_ENTRY   pFdEntry)
{
	__PIMX6Q_TEMP_CONTROLER   pTempDev  = (__PIMX6Q_TEMP_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;
    PLW_FD_NODE               pFdNode = (PLW_FD_NODE)pFdEntry->FDENTRY_pfdnode;

    API_IosFdNodeDec(&pTempDev->TEMP_fdNodeHeader, pFdNode, NULL);
    LW_DEV_DEC_USE_COUNT(&pTempDev->TEMP_devHdr);

    return (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonIoctl
** 功能描述: 控制 WATCH_DOG 设备
** 输　入  : pFdEntry              文件结构
**           iCmd                  命令
**           lArg                  参数  这里是温度报警数值
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  imx6qTempMonIoctl (PLW_FD_ENTRY  pFdEntry, INT  iCmd, LONG lArg)
{
	__PIMX6Q_TEMP_CONTROLER pTempDev = (__PIMX6Q_TEMP_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;

    switch (iCmd) {
    case TEMP_ALARMEN_CMD:                                              /*  使能温度测量Alarm中断       */
    	imx6qTempMonAlarmInit(pTempDev, (UINT32)lArg);
        return  (ERROR_NONE);

    case TEMP_ALARMDIS_CMD:                                             /*  禁止温度测量Alarm中断       */
    	imx6qTempMonAlarmDisable(pTempDev);
        return  (ERROR_NONE);

    default:
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonWrite
** 功能描述: 驱动 write 函数，实现看门狗喂狗功能，写入字符为0xAAAA5555
** 输　入  : peeprom      eeprom 结构
**           pvBuf        写缓冲
**           stLen        写数据长度
** 输　出  : 成功读取数据字节数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  imx6qTempMonWrite (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
   /*
    *  对温度的写入没有任何意义
    */
    return  (stLen);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonRead
** 功能描述: 驱动 write 函数
** 输　入  :
** 输　出  : 成功读取数据字节数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  imx6qTempMonRead (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
	__PIMX6Q_TEMP_CONTROLER   pTempDev   = (__PIMX6Q_TEMP_CONTROLER)pFdentry->FDENTRY_pdevhdrHdr;
	addr_t                    atBaseAddr = pTempDev->TEMP_BaseAddr;

	UINT32 uiRegVal;

    /*
     * Wake up the temp monitor.
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0xFFFFFFFC;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);

    /*
     * Clear the measure frequency so we only get single measurements.
     */
    writel(0x0, atBaseAddr + TEMPMONCTL1);

    /*
     * Start a measurement cycle.
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) | 0x2;
    writel(uiRegVal, atBaseAddr + TEMPMONCTL0);

    /*
     * Wait until the measurement is ready.
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0x4;
    while (uiRegVal == 0) {
        udelay(1);
        uiRegVal = readl(atBaseAddr + TEMPMONCTL0) & 0x4;
    }

    /*
     *  disable tempmon and back the val to user
     */
    uiRegVal = readl(atBaseAddr + TEMPMONCTL0);
    writel(uiRegVal | 0x1, atBaseAddr + TEMPMONCTL0);

    float fTempVal = (uiRegVal & 0x000FFF00) >> 8;
    INT iTempVal = (INT)imx6qComputeTemp(fTempVal);

    *(INT *)pvBuf = iTempVal;

    return  (4);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonDrv
** 功能描述: 安装 WATCH_DOG 驱动
** 输　入  : NONE
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  imx6qTempMonDrv (VOID)
{
    struct file_operations  fileop;

    if (_G_iTempDrvNum) {
        return  (ERROR_NONE);
    }

    lib_memset(&fileop, 0, sizeof(struct file_operations));

    fileop.owner     = THIS_MODULE;
    fileop.fo_create = imx6qTempMonOpen;
    fileop.fo_open   = imx6qTempMonOpen;
    fileop.fo_close  = imx6qTempMonClose;
    fileop.fo_ioctl  = imx6qTempMonIoctl;
    fileop.fo_write  = imx6qTempMonWrite;
    fileop.fo_read   = imx6qTempMonRead;

    _G_iTempDrvNum = iosDrvInstallEx2(&fileop, LW_DRV_TYPE_NEW_1);

    DRIVER_LICENSE(_G_iTempDrvNum,     "Dual BSD/GPL->Ver 1.0");
    DRIVER_AUTHOR(_G_iTempDrvNum,      "Zhang.Xu");
    DRIVER_DESCRIPTION(_G_iTempDrvNum, "TempMon driver.");

    return  (_G_iTempDrvNum > 0) ? (ERROR_NONE) : (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: imx6qTempMonDevAdd
** 功能描述: 创建 WATCH_DOG 设备
** 输　入  : uiIndex           ID
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  imx6qTempMonDevAdd (VOID)
{
	__PIMX6Q_TEMP_CONTROLER   pTempDev;

    pTempDev = &_G_tempctller;
    pTempDev ->TEMP_BaseAddr = TEMPMON_BASE;
    pTempDev ->TEMP_iIrqNum  = IMX_INT_TEMPERATURE;

    if (API_IosDevAddEx(&pTempDev->TEMP_devHdr, "/dev/temp0", _G_iTempDrvNum, DT_CHR) != ERROR_NONE) {
        printk(KERN_ERR "imx6qTempDevAdd(): can not add device : %s.\n", strerror(errno));
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
