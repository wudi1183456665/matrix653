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
** 文   件   名: imx6q_wdt.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 12 月 13 日
**
** 描        述: 看门狗定时器驱动，看门狗的使用流程为,看门狗1和2的用途不同，使用时需要使用看门狗1
**               int  fd;
**               unsigned int  weigou = 0xAAAA5555;
**               fd = open("/dev/wdt0", O_RDWR, 0666);
**               write(fd, &weigou, 4);
**               sleep(2);
**               write(fd, &weigou, 4);
**               sleep(2);
**               close(fd);
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"

#include "wdt/imx6q_wdt.h"
#include "sdk.h"
/*********************************************************************************************************
  WATCH_DOG 寄存器定义,寄存器为 16bits，而不是 32bits，
*********************************************************************************************************/
#define WCR             (0x0)                                           /* control                      */
#define WSR             (0x2)                                           /* service                      */
#define WRSR            (0x4)                                           /* reset status                 */
#define WICR            (0x6)                                           /* interrupt control            */
#define WMCR            (0x8)                                           /* misc control                 */

#define WCR_WDE         (0x0004)                                        /* write once                   */
#define WCR_WDT         (0x0008)                                        /* write once                   */
#define WCR_SRS         (0x0010)                                        /* software reset               */
#define WCR_WDA         (0x0020)                                        /* assert output                */
#define WCR_WDBG        (0x0002)                                        /* suspend time while in debug  */
#define WCR_WT          (0x0500)                                        /* 3 seconds                    */

#define WDT_SEQ1        (0x5555)		                                /* -> service sequence 1        */
#define WDT_SEQ2        (0xAAAA)		                                /* -> service sequence 2        */

#define WDT_NUM         (2)
#define WDTTIME         (103)                                           /*  ioctl 函数命令字            */
#define WDTSTOP         (104)
/*********************************************************************************************************
  WATCH_DOG 控制器类型定义
*********************************************************************************************************/
typedef struct {
    LW_DEV_HDR                  WDTC_devHdr;                            /* 必须是第一个结构体成员       */
    LW_LIST_LINE_HEADER         WDTC_fdNodeHeader;
	addr_t                      WDTC_BaseAddr;
} __IMX6Q_WDT_CONTROLER, *__PIMX6Q_WDT_CONTROLER;

UINT32                        _G_iWdtDrvNum = 0;
static __IMX6Q_WDT_CONTROLER  _G_wdt[WDT_NUM];
/*********************************************************************************************************
** 函数名称: __imx6qWatchDogOpen
** 功能描述: 打开 WATCH_DOG 设备
** 输　入  : pDev                  设备
**           pcName                设备名字
**           iFlags                标志
**           iMode                 模式
** 输　出  : 文件节点
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LONG  __imx6qWatchDogOpen (__PIMX6Q_WDT_CONTROLER   pWdtDev, PCHAR pcName, INT iFlags, INT iMode)
{
    PLW_FD_NODE   pFdNode;
    BOOL          bIsNew;
	addr_t        atBaseAddr;

    if (pcName == LW_NULL) {
        _ErrorHandle(ERROR_IO_NO_DEVICE_NAME_IN_PATH);
        return  (PX_ERROR);

    } else {
        pFdNode = API_IosFdNodeAdd(&pWdtDev->WDTC_fdNodeHeader, (dev_t)pWdtDev, 0,
                                   iFlags, iMode, 0, 0, 0, LW_NULL, &bIsNew);
        if (pFdNode == LW_NULL) {
            printk(KERN_ERR "imx6qWdtOpen(): failed to add fd node!\n");
            return  (PX_ERROR);
        }

        if (LW_DEV_INC_USE_COUNT(&pWdtDev->WDTC_devHdr) == 1) {
            /*
             * 添加设备初始化和使能操作,看门狗默认复位时间为3s
             */
            atBaseAddr = pWdtDev->WDTC_BaseAddr;
            writew(WCR_WDA | WCR_SRS | WCR_WT | WCR_WDBG | WCR_WDE, atBaseAddr + WCR);
            return  ((LONG)pFdNode);
        }
    }
    /*
     * 如果设备打开失败进行如下操作
     */
    if (pFdNode) {
        API_IosFdNodeDec(&pWdtDev->WDTC_fdNodeHeader, pFdNode, NULL);
        pFdNode = LW_NULL;
    }
    LW_DEV_DEC_USE_COUNT(&pWdtDev->WDTC_devHdr);
    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: __imx6qWatchDogClose
** 功能描述: 关闭 WATCH_DOG 设备
** 输　入  : pFdEntry              文件结构
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __imx6qWatchDogClose (PLW_FD_ENTRY   pFdEntry)
{
	/*
	 * 看门狗一旦被使能，就不能够禁止，
	 */
    return (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: __imx6qWatchDogIoctl
** 功能描述: 控制 WATCH_DOG 设备
** 输　入  : pFdEntry              文件结构
**           iCmd                  命令
**           lArg                  参数
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6qWatchDogIoctl (PLW_FD_ENTRY  pFdEntry, INT  iCmd, LONG lArg)
{
	__PIMX6Q_WDT_CONTROLER   pWdtDev    = (__PIMX6Q_WDT_CONTROLER)pFdEntry->FDENTRY_pdevhdrHdr;
	addr_t                   atBaseAddr = pWdtDev->WDTC_BaseAddr;
    UINT16                   usRegVal;

    switch (iCmd) {
    case WDTTIME:                                                       /*  设置看门狗复位周期,         */
    	   usRegVal = readw(atBaseAddr + WCR) & 0x00FF;                 /*  参数需要小于0xFF            */
           writew(usRegVal | (lArg << 8), atBaseAddr + WCR);
        return  (ERROR_NONE);
    case WDTSTOP:                                                       /*  使看门狗处于暂停状态        */
    	   usRegVal = readw(atBaseAddr + WCR) | (1 << 7);
           writew(usRegVal, atBaseAddr + WCR);
        return  (ERROR_NONE);
    default:
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** 函数名称: __imx6qWatchDogWrite
** 功能描述: 驱动 write 函数，实现看门狗喂狗功能，写入字符为0xAAAA5555
** 输　入  : peeprom      eeprom 结构
**           pvBuf        写缓冲
**           stLen        写数据长度
** 输　出  : 成功读取数据字节数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  __imx6qWatchDogWrite (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
	__PIMX6Q_WDT_CONTROLER   pWdtDev    = (__PIMX6Q_WDT_CONTROLER)pFdentry->FDENTRY_pdevhdrHdr;
	addr_t                   atBaseAddr = pWdtDev->WDTC_BaseAddr;
    UINT32                   usRegVal;

    usRegVal = *(UINT32 *)pvBuf;
    writew(usRegVal, atBaseAddr + WSR);

    usRegVal = usRegVal >> 16;
    writew(usRegVal, atBaseAddr + WSR);

   /*
    * ----------喂狗的顺序------------
    * writew(0x5555, atBaseAddr + WSR);
    * writew(0xAAAA, atBaseAddr + WSR);
    */
    return  (stLen);
}
/*********************************************************************************************************
** 函数名称: __imx6qWatchDogRead
** 功能描述: 驱动 write 函数
** 输　入  :
** 输　出  : 成功读取数据字节数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  __imx6qWatchDogRead (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
	__PIMX6Q_WDT_CONTROLER   pWdtDev    = (__PIMX6Q_WDT_CONTROLER)pFdentry->FDENTRY_pdevhdrHdr;
	addr_t                   atBaseAddr = pWdtDev->WDTC_BaseAddr;

    /*
     * 返回看门狗状态，4:上电复位，1:看门狗复位，0:软件复位
     */
    *(INT *)pvBuf = readw(atBaseAddr + WSR);

    return  (2);
}
/*********************************************************************************************************
** 函数名称: imx6qWatchDogDrv
** 功能描述: 安装 WATCH_DOG 驱动
** 输　入  : NONE
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  imx6qWatchDogDrv (VOID)
{
    struct file_operations  fileop;

    if (_G_iWdtDrvNum) {
        return  (ERROR_NONE);
    }

    lib_memset(&fileop, 0, sizeof(struct file_operations));

    fileop.owner     = THIS_MODULE;
    fileop.fo_create = __imx6qWatchDogOpen;
    fileop.fo_open   = __imx6qWatchDogOpen;
    fileop.fo_close  = __imx6qWatchDogClose;
    fileop.fo_ioctl  = __imx6qWatchDogIoctl;
    fileop.fo_write  = __imx6qWatchDogWrite;
    fileop.fo_read   = __imx6qWatchDogRead;

    _G_iWdtDrvNum = iosDrvInstallEx2(&fileop, LW_DRV_TYPE_NEW_1);

    DRIVER_LICENSE(_G_iWdtDrvNum,     "Dual BSD/GPL->Ver 1.0");
    DRIVER_AUTHOR(_G_iWdtDrvNum,      "Zhang.Xu");
    DRIVER_DESCRIPTION(_G_iWdtDrvNum, "WDT driver.");

    return  (_G_iWdtDrvNum > 0) ? (ERROR_NONE) : (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: imx6qWatchDogDevAdd
** 功能描述: 创建 WATCH_DOG 设备
** 输　入  : uiIndex           ID
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  imx6qWatchDogDevAdd (UINT  uiIndex)
{
	__PIMX6Q_WDT_CONTROLER   pWdtDev;
    CHAR                     cBuffer[64];

    if (uiIndex >= WDT_NUM) {
        printk(KERN_ERR "imx6qWdtDevAdd(): wdt index invalid!\n");
        return  (PX_ERROR);
    }
    pWdtDev = &_G_wdt[uiIndex];

    if(uiIndex == 0) {
        pWdtDev ->WDTC_BaseAddr = WDOG1_BASE_ADDR;
    } else {
        pWdtDev ->WDTC_BaseAddr = WDOG2_BASE_ADDR;
    }

    snprintf(cBuffer, sizeof(cBuffer), "/dev/wdt%d", uiIndex);

    if (API_IosDevAddEx(&pWdtDev->WDTC_devHdr, cBuffer, _G_iWdtDrvNum, DT_CHR) != ERROR_NONE) {
        printk(KERN_ERR "imx6qWdtDevAdd(): can not add device : %s.\n", strerror(errno));
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
