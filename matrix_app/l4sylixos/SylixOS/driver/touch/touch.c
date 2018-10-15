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
** 文   件   名: touch.c
**
** 创   建   人: Xu.Guizhou (徐贵洲)
**
** 文件创建日期: 2016 年 11 月 11 日
**
** 描        述: TOUCH 触摸屏驱动源文件.
**
** BUG:
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include <input_device.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <mouse.h>
#include <sys/select.h>
#include "sys/ioccom.h"
#include "sys/gpiofd.h"

#include "touch/touch.h"
/*********************************************************************************************************
  定义
*********************************************************************************************************/
#define __TOUCH_THREAD_SIZE                (12 * 1024)
#define __TOUCH_THREAD_PRIO                (LW_PRIO_NORMAL - 1)         /*  内部线程优先级              */
/*********************************************************************************************************
** 函数名称: __touchReset
** 功能描述: 复位 TOUCH 触摸屏硬件设备
** 输　入  : pTouchDev     触摸屏控制结构
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __touchReset (PTOUCH_DEV  pTouchDev)
{
    UINT  uiResetTime = pTouchDev->TOUCH_data.T_uiRstTime;

    if (uiResetTime == 0) {
        uiResetTime = 50;
    }

    API_GpioSetValue(pTouchDev->TOUCH_data.T_uiReset, pTouchDev->TOUCH_data.T_uiRstVal);

    API_TimeMSleep(uiResetTime);                                        /*  Reset time sleep            */

    if (pTouchDev->TOUCH_data.T_uiRstVal == LW_GPIOF_INIT_HIGH) {
        API_GpioSetValue(pTouchDev->TOUCH_data.T_uiReset, LW_GPIOF_INIT_LOW);
    } else {
        API_GpioSetValue(pTouchDev->TOUCH_data.T_uiReset, LW_GPIOF_INIT_HIGH);
    }
    API_TimeMSleep(uiResetTime);                                        /*  Wait for device ready       */

    if (pTouchDev->pDrvFunc->reset) {
        pTouchDev->pDrvFunc->reset(pTouchDev);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __touchHwInit
** 功能描述: 初始化 TOUCH 触摸屏硬件设备
** 输　入  : pTouchDev       TOUCH 触摸屏设备
**           pcName          Device名称
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __touchHwInit (PTOUCH_DEV  pTouchDev, PCHAR  pcName)
{
    INT  iError;

    pTouchDev->TOUCH_pI2cDevice =
                                API_I2cDeviceCreate(pTouchDev->TOUCH_data.T_pcBusName,
                                                    pcName,
                                                    pTouchDev->TOUCH_data.T_usAddr,
                                                    0);
    if (pTouchDev->TOUCH_pI2cDevice == LW_NULL) {
        printk(KERN_ERR "__touchHwInit(): failed to create i2c device %s, slave addr 0x%x!\n",
                        pTouchDev->TOUCH_data.T_pcBusName,
                        pTouchDev->TOUCH_data.T_usAddr);
        goto  __error_handle;
    }

    /*
     * 设置触摸屏中断
     */
    iError = API_GpioRequestOne(pTouchDev->TOUCH_data.T_uiIrq,
                                pTouchDev->TOUCH_data.T_uiIrqCfg,
                                "touch_eint");
    if (iError != ERROR_NONE) {
        printk(KERN_ERR "__touchHwInit(): failed to request gpio!\n");
        goto  __error_handle;
    }

    iError = API_GpioSetupIrq(pTouchDev->TOUCH_data.T_uiIrq, LW_FALSE, 0);
    if (iError == PX_ERROR) {
        printk(KERN_ERR "__touchHwInit(): failed to setup gpio irq!\n");
        goto  __error_handle;
    }
    pTouchDev->TOUCH_ulVector = (ULONG)iError;

    /*
     * 申请触摸屏复位管脚
     */
    iError = API_GpioRequestOne(pTouchDev->TOUCH_data.T_uiReset,
                                pTouchDev->TOUCH_data.T_uiRstVal | LW_GPIOF_DIR_OUT,
                                "touch_reset");
    if (iError != ERROR_NONE) {
        printk(KERN_ERR "__touchHwInit(): failed to request gpio!\n");
        goto  __error_handle;
    }

    __touchReset(pTouchDev);                                            /*  Reset Touch Screen          */

    if (pTouchDev->pDrvFunc->init) {                                    /*  Init Touch Screen           */
        pTouchDev->pDrvFunc->init(pTouchDev);
    }

    return  (ERROR_NONE);

__error_handle:
    API_GpioFree(pTouchDev->TOUCH_data.T_uiIrq);
    API_GpioFree(pTouchDev->TOUCH_data.T_uiReset);

    if (pTouchDev->TOUCH_pI2cDevice) {
        API_I2cDeviceDelete(pTouchDev->TOUCH_pI2cDevice);
        pTouchDev->TOUCH_pI2cDevice = LW_NULL;
    }

    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: __touchDeInit
** 功能描述: 关闭 TOUCH 触摸屏设备
** 输　入  : pTouchDev       TOUCH 触摸屏设备
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  __touchDeInit (PTOUCH_DEV  pTouchDev)
{
    if (pTouchDev->pDrvFunc->deinit) {
        pTouchDev->pDrvFunc->deinit(pTouchDev);
    }

    API_GpioFree(pTouchDev->TOUCH_data.T_uiReset);
    API_GpioFree(pTouchDev->TOUCH_data.T_uiIrq);

    if (pTouchDev->TOUCH_pI2cDevice) {
        API_I2cDeviceDelete(pTouchDev->TOUCH_pI2cDevice);
        pTouchDev->TOUCH_pI2cDevice = LW_NULL;
    }
}
/*********************************************************************************************************
** 函数名称: __touchIsr
** 功能描述: TOUCH 触摸屏设备中断服务例程
** 输　入  : pTouchDev         触摸屏设备结构
**           ulVector          中断向量号
** 输　出  : 中断返回值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static irqreturn_t  __touchIsr (PTOUCH_DEV  pTouchDev, ULONG  ulVector)
{
    irqreturn_t     irqreturn;

    irqreturn = API_GpioSvrIrq(pTouchDev->TOUCH_data.T_uiIrq);
    if (irqreturn == LW_IRQ_HANDLED) {
        API_GpioClearIrq(pTouchDev->TOUCH_data.T_uiIrq);
        API_SemaphoreBPost(pTouchDev->TOUCH_hSignal);
    }

    return  (irqreturn);
}
/*********************************************************************************************************
** 函数名称: __touchHandleEvents
** 功能描述: 获得 TOUCH 触摸坐标值和状态
** 输　入  : pTouchDev         TOUCH 设备结构
** 输　出  : 返回获取到的点数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __touchHandleEvents (PTOUCH_DEV  pTouchDev)
{
    mouse_event_notify  events[TOUCH_MAX_INPUT_POINTS];
    INT                 iEvents;

    INT                 i  = 0;

    iEvents = pTouchDev->pDrvFunc->getevent(pTouchDev, events);

    if (iEvents == TOUCH_RELEASE_NUM) {                                 /*  Send release event          */
        API_MsgQueueSend(pTouchDev->TOUCH_hEventQueue,
                        (PVOID)&events[0],
                        (ULONG)sizeof(mouse_event_notify));
        i = 1;                                                          /*  One Release                 */
    } else {

        if (iEvents > TOUCH_MAX_INPUT_POINTS) {
            iEvents = TOUCH_MAX_INPUT_POINTS;
        }

        for (i=0; i<iEvents; i++) {

            if (pTouchDev->TOUCH_data.T_iDirFlag != T_XY_NORMAL) {

                if (pTouchDev->TOUCH_data.T_iDirFlag & T_X_REVERT) {
                    events[i].xmovement = pTouchDev->TOUCH_data.T_iWidth - events[i].xmovement;
                }

                if (pTouchDev->TOUCH_data.T_iDirFlag & T_Y_REVERT) {
                    events[i].ymovement = pTouchDev->TOUCH_data.T_iHeight - events[i].ymovement;
                }

                if (pTouchDev->TOUCH_data.T_iDirFlag & T_XY_SWAP) {
                    UINT  uiTmp;

                    uiTmp = events[i].xmovement;
                    events[i].xmovement = events[i].ymovement;
                    events[i].ymovement = uiTmp;
                }
            }

            API_MsgQueueSend(pTouchDev->TOUCH_hEventQueue,
                            (PVOID)&events[i],
                            (ULONG)sizeof(mouse_event_notify));
        }

        if (i > 0) {
            pTouchDev->TOUCH_iLastX = events[i-1].xmovement;
            pTouchDev->TOUCH_iLastY = events[i-1].ymovement;
        }
    }

    return  (i);
}
/*********************************************************************************************************
** 函数名称: __touchThread
** 功能描述: TOUCH 触摸屏设备线程
** 输　入  : pvArg                 参数
** 输　出  : 返回值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static PVOID  __touchThread (PVOID  pvArg)
{
    INT                 iEventNum;
    PTOUCH_DEV        pTouchDev  = (PTOUCH_DEV)pvArg;

    while (!pTouchDev->TOUCH_bQuit) {

        API_SemaphoreBPend(pTouchDev->TOUCH_hSignal, LW_OPTION_WAIT_INFINITE);

        if (pTouchDev->TOUCH_bQuit) {
            break;
        }

        iEventNum = __touchHandleEvents(pTouchDev);
        if (iEventNum == PX_ERROR) {
            printk(KERN_ERR "touch: handle touch event fail!\n");
        } else if (iEventNum > 0) {
            SEL_WAKE_UP_ALL(&pTouchDev->TOUCH_selList, SELREAD);
        }
    }

    return  (LW_NULL);
}
/*********************************************************************************************************
** 函数名称: __touchOpen
** 功能描述: 打开 TOUCH 触摸屏设备
** 输　入  : pDev                  设备
**           pcName                设备名字
**           iFlags                标志
**           iMode                 模式
** 输　出  : 文件节点
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static LONG  __touchOpen (PLW_DEV_HDR          pDev,
                          PCHAR                pcName,
                          INT                  iFlags,
                          INT                  iMode)
{
    PLW_FD_NODE                 pFdNode;
    BOOL                        bIsNew;
    LW_CLASS_THREADATTR         threadAttr;
    INT                         iError;
    PTOUCH_DEV                  pTouchDev = (PTOUCH_DEV)pDev;

    if (pcName == LW_NULL) {
        _ErrorHandle(ERROR_IO_NO_DEVICE_NAME_IN_PATH);
        return  (PX_ERROR);

    } else {
        if (LW_DEV_INC_USE_COUNT(&pTouchDev->TOUCH_devHdr) != 1) {
            _ErrorHandle(EBUSY);
            LW_DEV_DEC_USE_COUNT(&pTouchDev->TOUCH_devHdr);
            return  (PX_ERROR);
        }

        pFdNode = API_IosFdNodeAdd(&pTouchDev->TOUCH_fdNodeHeader,
                                   (dev_t)pTouchDev, 0,
                                   iFlags, iMode, 0, 0, 0, LW_NULL, &bIsNew);
        if (pFdNode == LW_NULL) {
            printk(KERN_ERR "__touchOpen(): failed to add fd node!\n");
            goto  __error_handle;
        }

        pTouchDev->TOUCH_hEventQueue = API_MsgQueueCreate("touch_msgq",
                                                         TOUCH_MAX_INPUT_QUEUE,
                                                         (sizeof(struct mouse_event_notify) *
                                                         pTouchDev->TOUCH_data.T_iTouchNum),
                                                         LW_OPTION_OBJECT_GLOBAL, LW_NULL);
        if (pTouchDev->TOUCH_hEventQueue == LW_OBJECT_HANDLE_INVALID) {
            printk(KERN_ERR "__touchOpen(): failed to create touch_msgq!\n");
            goto  __error_handle;
        }

        pTouchDev->TOUCH_hSignal = API_SemaphoreBCreate("touch_signal",
                                                        0,
                                                        LW_OPTION_OBJECT_GLOBAL, LW_NULL);
        if (pTouchDev->TOUCH_hSignal == LW_OBJECT_HANDLE_INVALID) {
            printk(KERN_ERR "__touchOpen(): failed to create touch_signal!\n");
            goto  __error_handle;
        }

        SEL_WAKE_UP_LIST_INIT(&pTouchDev->TOUCH_selList);

        if (__touchHwInit(pTouchDev, pcName) != ERROR_NONE) {
            printk(KERN_ERR "__touchOpen(): failed to init!\n");
            goto  __error_handle;
        }

        threadAttr = API_ThreadAttrGetDefault();

        threadAttr.THREADATTR_ulOption       |= LW_OPTION_OBJECT_GLOBAL;
        threadAttr.THREADATTR_stStackByteSize = __TOUCH_THREAD_SIZE;
        threadAttr.THREADATTR_pvArg           = pTouchDev;
        threadAttr.THREADATTR_ucPriority      = __TOUCH_THREAD_PRIO;

        pTouchDev->TOUCH_bQuit   = LW_FALSE;
        pTouchDev->TOUCH_hThread = API_ThreadCreate("t_touch",
                                                    __touchThread,
                                                    &threadAttr,
                                                    LW_NULL);
        if (pTouchDev->TOUCH_hThread == LW_OBJECT_HANDLE_INVALID) {
            printk(KERN_ERR "__touchOpen(): failed to create t_touch thread!\n");
            goto  __error_handle;
        }

        iError = API_InterVectorConnect((ULONG)pTouchDev->TOUCH_ulVector,
                                        (PINT_SVR_ROUTINE)__touchIsr,
                                        (PVOID)pTouchDev, "touch_isr");
        if (iError != ERROR_NONE) {
            printk(KERN_ERR "__touchOpen(): failed to connect touch_isr!\n");
            goto  __error_handle;
        }
        API_InterVectorEnable(pTouchDev->TOUCH_ulVector);

        return  ((LONG)pFdNode);
    }

__error_handle:

    if (pTouchDev->TOUCH_ulVector) {
        API_InterVectorDisable(pTouchDev->TOUCH_ulVector);
        API_InterVectorDisconnect((ULONG)pTouchDev->TOUCH_ulVector,
                                  (PINT_SVR_ROUTINE)__touchIsr,
                                  (PVOID)pTouchDev);
        pTouchDev->TOUCH_ulVector = 0;
    }

    if (pTouchDev->TOUCH_hThread) {
        pTouchDev->TOUCH_bQuit = LW_TRUE;
        API_SemaphoreBPost(pTouchDev->TOUCH_hSignal);
        API_ThreadDelete(&pTouchDev->TOUCH_hThread, LW_NULL);
        pTouchDev->TOUCH_hThread = 0;
    }

    if (pTouchDev->TOUCH_hEventQueue) {
        API_MsgQueueDelete(&pTouchDev->TOUCH_hEventQueue);
        pTouchDev->TOUCH_hEventQueue = 0;
    }

    if (pTouchDev->TOUCH_hSignal) {
        API_SemaphoreBDelete(&pTouchDev->TOUCH_hSignal);
        pTouchDev->TOUCH_hSignal = 0;
    }

    __touchDeInit(pTouchDev);

    if (pFdNode) {
        API_IosFdNodeDec(&pTouchDev->TOUCH_fdNodeHeader, pFdNode, NULL);
        pFdNode = LW_NULL;
    }

    LW_DEV_DEC_USE_COUNT(&pTouchDev->TOUCH_devHdr);

    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: __touchClose
** 功能描述: 关闭 TOUCH 触摸屏设备
** 输　入  : pFdEntry              文件结构
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __touchClose (PLW_FD_ENTRY      pFdEntry)
{
    PTOUCH_DEV         pTouchDev  = (PTOUCH_DEV)pFdEntry->FDENTRY_pdevhdrHdr;
    PLW_FD_NODE          pFdNode    = (PLW_FD_NODE)pFdEntry->FDENTRY_pfdnode;

    if (pFdEntry && pFdNode) {
        API_IosFdNodeDec(&pTouchDev->TOUCH_fdNodeHeader, pFdNode, NULL);

        API_InterVectorDisable(pTouchDev->TOUCH_ulVector);
        API_InterVectorDisconnect((ULONG)pTouchDev->TOUCH_ulVector,
                                  (PINT_SVR_ROUTINE)__touchIsr,
                                  (PVOID)pTouchDev);
        pTouchDev->TOUCH_ulVector = 0;

        pTouchDev->TOUCH_bQuit = LW_TRUE;
        API_SemaphoreBPost(pTouchDev->TOUCH_hSignal);
        pTouchDev->TOUCH_hThread = 0;

        API_SemaphoreBDelete(&pTouchDev->TOUCH_hSignal);
        pTouchDev->TOUCH_hSignal = 0;

        API_MsgQueueDelete(&pTouchDev->TOUCH_hEventQueue);
        pTouchDev->TOUCH_hEventQueue = 0;

        SEL_WAKE_UP_LIST_TERM(&pTouchDev->TOUCH_selList);

        __touchDeInit(pTouchDev);

        LW_DEV_DEC_USE_COUNT(&pTouchDev->TOUCH_devHdr);

        return  (ERROR_NONE);
    }

    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: __touchRead
** 功能描述: 读 TOUCH 触摸屏设备
** 输　入  : pFdEntry              文件结构
**           pcBuffer              缓冲区
**           stMaxBytes            缓冲区长度
** 输　出  : 成功读取的字节数
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static ssize_t  __touchRead (PLW_FD_ENTRY      pFdEntry,
                             PCHAR             pcBuffer,
                             size_t            stMaxBytes)
{
    PTOUCH_DEV        pTouchDev = (PTOUCH_DEV)pFdEntry->FDENTRY_pdevhdrHdr;
    u_long              ulTimeout;
    size_t              stMsgLen = 0;


    if (!pFdEntry || !pcBuffer) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if (!stMaxBytes) {
        return  (0);
    }

    if (pFdEntry->FDENTRY_iFlag & O_NONBLOCK) {
        ulTimeout = 0;
    } else {
        ulTimeout = LW_OPTION_WAIT_INFINITE;
    }

    if (API_MsgQueueReceive(pTouchDev->TOUCH_hEventQueue,
                            pcBuffer,
                            stMaxBytes,
                            &stMsgLen,
                            ulTimeout)) {
        return  (PX_ERROR);
    }

    return  ((ssize_t)stMsgLen);
}
/*********************************************************************************************************
** 函数名称: __touchLstat
** 功能描述: 获得 TOUCH 触摸屏设备状态
** 输　入  : pDev                  设备
**           pcName                设备名字
**           pStat                 stat 结构指针
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __touchLstat (PLW_DEV_HDR          pDev,
                          PCHAR                pcName,
                          struct stat         *pStat)
{
    PTOUCH_DEV         pTouchDev = (PTOUCH_DEV)pDev;

    if (pStat) {
        pStat->st_dev     = (dev_t)pTouchDev;
        pStat->st_ino     = (ino_t)0;
        pStat->st_mode    = (S_IFCHR | S_IRUSR | S_IRGRP | S_IROTH);
        pStat->st_nlink   = 1;
        pStat->st_uid     = 0;
        pStat->st_gid     = 0;
        pStat->st_rdev    = 0;
        pStat->st_size    = 0;
        pStat->st_blksize = 0;
        pStat->st_blocks  = 0;
        pStat->st_atime   = pTouchDev->TOUCH_time;
        pStat->st_mtime   = pTouchDev->TOUCH_time;
        pStat->st_ctime   = pTouchDev->TOUCH_time;
        return  (ERROR_NONE);
    }

    return  (PX_ERROR);
}
/*********************************************************************************************************
** 函数名称: __touchIoctl
** 功能描述: 控制 TOUCH 触摸屏设备
** 输　入  : pFdEntry              文件结构
**           iCmd                  命令
**           lArg                  参数
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __touchIoctl (PLW_FD_ENTRY     pFdEntry,
                          INT              iCmd,
                          LONG             lArg)
{
    PTOUCH_DEV              pTouchDev = (PTOUCH_DEV)pFdEntry->FDENTRY_pdevhdrHdr;
    struct stat            *pStat;
    PLW_SEL_WAKEUPNODE      pSelNode;
    u_long                  ulCount = 0;
    size_t                  stLen   = 0;

    switch (iCmd) {

    case FIONBIO:
        API_MsgQueueStatus(pTouchDev->TOUCH_hEventQueue,
                           LW_NULL, &ulCount, &stLen, LW_NULL, LW_NULL);
        if (ulCount && lArg) {
            *(LONG *)lArg = (LONG)stLen;
        }
        return  (ERROR_NONE);

    case FIONREAD:
        API_MsgQueueStatus(pTouchDev->TOUCH_hEventQueue,
                           LW_NULL, &ulCount, &stLen, LW_NULL, LW_NULL);
        if (ulCount && lArg) {
            *(INT *)lArg = (INT)stLen;
        }
        return  (ERROR_NONE);

    case FIONREAD64:
        API_MsgQueueStatus(pTouchDev->TOUCH_hEventQueue,
                           LW_NULL, &ulCount, &stLen, LW_NULL, LW_NULL);
        if (ulCount && lArg) {
            *(off_t *)lArg = (off_t)stLen;
        }
        return  (ERROR_NONE);

    case FIOFLUSH:
        API_MsgQueueClear(pTouchDev->TOUCH_hEventQueue);
        return  (ERROR_NONE);

    case FIOFSTATGET:
        pStat = (struct stat *)lArg;
        if (pStat) {
            pStat->st_dev     = (dev_t)pTouchDev;
            pStat->st_ino     = (ino_t)0;
            pStat->st_mode    = (S_IRUSR | S_IRGRP | S_IROTH);
            pStat->st_nlink   = 1;
            pStat->st_uid     = 0;
            pStat->st_gid     = 0;
            pStat->st_rdev    = 0;
            pStat->st_size    = 0;
            pStat->st_blksize = 0;
            pStat->st_blocks  = 0;
            pStat->st_atime   = pTouchDev->TOUCH_time;
            pStat->st_mtime   = pTouchDev->TOUCH_time;
            pStat->st_ctime   = pTouchDev->TOUCH_time;
            return  (ERROR_NONE);
        }
        return  (PX_ERROR);

    case FIOSETFL:
        if ((int)lArg & O_NONBLOCK) {
            pFdEntry->FDENTRY_iFlag |= O_NONBLOCK;
        } else {
            pFdEntry->FDENTRY_iFlag &= ~O_NONBLOCK;
        }
        return  (ERROR_NONE);

    case FIOSELECT:
        pSelNode = (PLW_SEL_WAKEUPNODE)lArg;
        SEL_WAKE_NODE_ADD(&pTouchDev->TOUCH_selList, pSelNode);
        if (pSelNode->SELWUN_seltypType != SELREAD) {
            SEL_WAKE_UP(pSelNode);
        } else {
            if (API_MsgQueueStatus(pTouchDev->TOUCH_hEventQueue,
                                   LW_NULL, &ulCount, LW_NULL, LW_NULL, LW_NULL)) {
                SEL_WAKE_UP(pSelNode);
            } else if (ulCount) {
                SEL_WAKE_UP(pSelNode);
            }
        }
        return  (ERROR_NONE);

    case FIOUNSELECT:
        SEL_WAKE_NODE_DELETE(&pTouchDev->TOUCH_selList, (PLW_SEL_WAKEUPNODE)lArg);
        return  (ERROR_NONE);

    default:
        if (pTouchDev->pDrvFunc->ioctl) {
            return  (pTouchDev->pDrvFunc->ioctl(pTouchDev, iCmd, lArg));
        } else {
            _ErrorHandle(ENOSYS);
            return  (PX_ERROR);
        }
    }
}
/*********************************************************************************************************
** 函数名称: touchDrv
** 功能描述: 安装 TOUCH 触摸屏驱动
** 输　入  : NONE
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  touchDrv (VOID)
{
    struct file_operations      fileOper;
    INT                         iDrvNum;

    lib_memset(&fileOper, 0, sizeof(struct file_operations));

    fileOper.owner     = THIS_MODULE;
    fileOper.fo_create = __touchOpen;
    fileOper.fo_open   = __touchOpen;
    fileOper.fo_close  = __touchClose;
    fileOper.fo_read   = __touchRead;
    fileOper.fo_lstat  = __touchLstat;
    fileOper.fo_ioctl  = __touchIoctl;

    iDrvNum = iosDrvInstallEx2(&fileOper, LW_DRV_TYPE_NEW_1);

    DRIVER_LICENSE(iDrvNum,      "Dual BSD/GPL->Ver 1.0");
    DRIVER_AUTHOR(iDrvNum,       "Xu.Guizhou");
    DRIVER_DESCRIPTION(iDrvNum,  "touch driver.");

    return  (iDrvNum);
}
/*********************************************************************************************************
** 函数名称: touchDevCreate
** 功能描述: 创建 TOUCH 触摸屏设备
** 输　入  : pcName         TOUCH 触摸屏设备名字
**           pTouchData    TOUCH 触摸屏设备数据
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  touchDevCreate (CPCHAR             pcName,
                     INT                iDrvNum,
                     PTOUCH_DATA        pTouchData,
                     PTOUCH_DRV_FUNC    pDrvFunc)
{
    PTOUCH_DEV        pTouchDev;
    INT                 iError;

    if ((pTouchData  == LW_NULL)
        || (pcName   == LW_NULL)
        || (pDrvFunc == LW_NULL)
        || (pDrvFunc->getevent == LW_NULL)) {

        printk(KERN_ERR "touchDevCreate(): parameters invalid!\n");
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if (iDrvNum == PX_ERROR) {
        printk(KERN_ERR "touchDevCreate(): have NOT installed driver!\n");
        _ErrorHandle(ERROR_IO_NO_DRIVER);
        return  (PX_ERROR);
    }

    pTouchDev = (PTOUCH_DEV)__SHEAP_ALLOC(sizeof(TOUCH_DEV));
    if (!pTouchDev) {
        printk(KERN_ERR "touchDevCreate(): failed to alloc touch device struct!\n");
        _ErrorHandle(ERROR_SYSTEM_LOW_MEMORY);
        return  (PX_ERROR);
    }

    lib_bzero(pTouchDev, sizeof(TOUCH_DEV));

    pTouchDev->TOUCH_drvNum = iDrvNum;
    pTouchDev->pDrvFunc     = pDrvFunc;
    pTouchDev->TOUCH_time   = lib_time(LW_NULL);

    lib_memcpy(&pTouchDev->TOUCH_data, pTouchData, sizeof(*pTouchData));

    if (pTouchDev->TOUCH_data.T_usAddr == 0) {
        printk(KERN_ERR "touchDevCreate(): slave address is invalid!\n");
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    iError = iosDevAddEx(&pTouchDev->TOUCH_devHdr, pcName, iDrvNum, DT_CHR);
    if (iError) {
        _ErrorHandle(ERROR_SYSTEM_LOW_MEMORY);
        __SHEAP_FREE(pTouchDev);
        printk(KERN_ERR "touchDevCreate(): failed to create touch device %s!\n", pcName);
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
