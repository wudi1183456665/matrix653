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
** 文   件   名: gt911.c
**
** 创   建   人: Xu.Guizhou (徐贵洲)
**
** 文件创建日期: 2016 年 11 月 11 日
**
** 描        述: Goodix GT9xx 触摸屏驱动源文件.
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
  TOUCH 触摸屏寄存器相关
*********************************************************************************************************/
#define GOODIX_MAX_HEIGHT                   4096
#define GOODIX_MAX_WIDTH                    4096
#define GOODIX_INT_TRIGGER                  1
#define GOODIX_CONTACT_SIZE                 8
#define GOODIX_MAX_CONTACTS                 10
#define GOODIX_CONFIG_MAX_LENGTH            240

#define GOODIX_READ_COOR_ADDR               0x814E
#define GOODIX_REG_CONFIG_DATA              0x8047
#define GOODIX_REG_ID                       0x8140
#define GOODIX_REG_PNUM                     0x804C
#define GOODIX_REG_SPEED                    0x8056
#define GOODIX_REG_CMD                      0x8040

#define RESOLUTION_LOC                      1
#define MAX_CONTACTS_LOC                    5
#define TRIGGER_LOC                         6
/*********************************************************************************************************
** 函数名称: __gt911RxData
** 功能描述: 从 TOUCH 接收数据
** 输　入  : pTouchDev          TOUCH 设备结构
**           usReg              GTxx寄存器地址
**           pucBuffer          接收数据缓冲区
**           usLen              接收数据的长度
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __gt911RegRead (PLW_I2C_DEVICE      pI2cDev,
                            UINT16              usReg,
                            UINT8              *pucBuffer,
                            UINT16              usLen)
{
    INT             iError;
    UINT16          usRegAddr = htobe16(usReg);
    LW_I2C_MESSAGE  i2cMsgs[] = {
        {
            .I2CMSG_usAddr    = pI2cDev->I2CDEV_usAddr,
            .I2CMSG_usFlag    = 0,
            .I2CMSG_usLen     = 2,
            .I2CMSG_pucBuffer = (PUCHAR)&usRegAddr,
        },
        {
            .I2CMSG_usAddr    = pI2cDev->I2CDEV_usAddr,
            .I2CMSG_usFlag    = LW_I2C_M_RD,
            .I2CMSG_usLen     = usLen,
            .I2CMSG_pucBuffer = pucBuffer,
        },
    };

    iError = API_I2cDeviceTransfer(pI2cDev, i2cMsgs, 2);
    if (iError != 2) {
        printk(KERN_ERR "__gt911RxData(): failed to i2c transfer!\n");
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __gt911RxData
** 功能描述: 从 TOUCH 接收数据
** 输　入  : pTouchDev          TOUCH 设备结构
**           usReg              GTxx寄存器地址
**           pucBuffer          发送数据缓冲区
**           usLen              发送数据的长度
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __gt911RegWrite (PLW_I2C_DEVICE      pI2cDev,
                             UINT16              usReg,
                             UINT8              *pucBuffer,
                             UINT16              usLen)
{
    INT             iError;
    PUCHAR          pucWrBuf;
    LW_I2C_MESSAGE  i2cMsgs;

    pucWrBuf  = mallocalign(usLen + 2, 4);
    if (pucWrBuf == NULL) {
        printk(KERN_ERR "__gt911RegWrite(): insufficient mem!\n");
        return  (PX_ERROR);
    }

    pucWrBuf[0]  = (usReg >> 8) & 0xFF;
    pucWrBuf[1]  = usReg & 0xFF;

    memcpy(pucWrBuf + 2, pucBuffer, usLen);

    i2cMsgs.I2CMSG_usAddr       = pI2cDev->I2CDEV_usAddr;
    i2cMsgs.I2CMSG_usFlag       = 0;
    i2cMsgs.I2CMSG_usLen        = usLen + 2;
    i2cMsgs.I2CMSG_pucBuffer    = pucWrBuf;

    iError = API_I2cDeviceTransfer(pI2cDev, &i2cMsgs, 1);
    if (iError != 1) {
        printk(KERN_ERR "__gt911RegWrite(): failed to i2c transfer!\n");
        free(pucWrBuf);
        return  (PX_ERROR);
    }

    free(pucWrBuf);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __gt911RxData
** 功能描述: 从 TOUCH 接收数据
** 输　入  : pTouchDev            TOUCH 设备结构
**           pucBuffer            接收数据缓冲区
**           usLen                接收数据的长度
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __gt911RxData (PTOUCH_DEV  pTouchDev, UINT8  *pucBuffer, UINT16  usLen)
{
    INT     iError;

    iError = __gt911RegRead(pTouchDev->TOUCH_pI2cDevice,
                            GOODIX_READ_COOR_ADDR,
                            pucBuffer,
                            usLen);
    return  (iError);
}
/*********************************************************************************************************
** 函数名称: __goodixPointSet
** 功能描述: 设置 GT9xx 触摸屏最大触摸点数
** 输　入  : pTouchDev          触摸屏控制结构
**           iPointNum          最大点数
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __goodixPointSet(PTOUCH_DEV  pTouchDev, INT  iPointNum)
{
    INT     iError;
    UINT8   ucValue = iPointNum - 1;

    if (iPointNum <= 0) {
        return  (PX_ERROR);
    }

    iError = __gt911RegWrite(pTouchDev->TOUCH_pI2cDevice, GOODIX_REG_PNUM, &ucValue, 1);
    if (iError != ERROR_NONE) {
        printk(KERN_WARNING "__goodixPointSet set max point to %d error\n", iPointNum);
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __goodixSpeedSet
** 功能描述: 设置 GT9xx 触摸屏扫描周期
** 输　入  : pTouchDev         TOUCH 设备结构
**           iMiliSecond       扫描周期（iMiliSecond + 5ms）
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __goodixSpeedSet(PTOUCH_DEV  pTouchDev, INT  iMiliSecond)
{
    INT     iError;
    UINT8   ucValue = iMiliSecond;

    iError = __gt911RegWrite(pTouchDev->TOUCH_pI2cDevice,
                             GOODIX_REG_SPEED,
                             &ucValue,
                             1);
    if (iError != ERROR_NONE) {
        printk(KERN_WARNING "__goodixPointSet set report period to %d error\n", iMiliSecond);
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __goodixVerCheck
** 功能描述: 检查 GT9xx 触摸屏ID号
** 输　入  : pTouchDev         TOUCH 设备结构
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __goodixVerCheck(PTOUCH_DEV  pTouchDev)
{
    INT     iError;
    UINT8   version[8];

    iError = __gt911RegRead(pTouchDev->TOUCH_pI2cDevice,
                            GOODIX_REG_ID,
                            version,
                            GOODIX_CONFIG_MAX_LENGTH);
    if (iError != 2) {
        printk(KERN_INFO "get gtxx[0x%x] version error\n",
                         pTouchDev->TOUCH_pI2cDevice->I2CDEV_usAddr);
        return  (PX_ERROR);
    } else {
        printk(KERN_INFO "goodix version: 0x%2x 0x%2x 0x%2x 0x%2x, firmware: 0x%2x 0x%2x\n",
                         version[0], version[1], version[2],
                         version[3], version[4], version[5]);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __goodixReadConfig
** 功能描述: 读取配置信息
** 输　入  : pTouchDev         TOUCH 设备结构
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  __goodixReadConfig(PTOUCH_DEV  pTouchDev)
{
    UINT8  config[GOODIX_CONFIG_MAX_LENGTH];
    INT    iError;
    INT    iWidth;
    INT    iHeight;
    INT    iTouchNum;
    INT    iIrqTrigger;

    iError = __gt911RegRead(pTouchDev->TOUCH_pI2cDevice,
                            GOODIX_REG_CONFIG_DATA,
                            config,
                            GOODIX_CONFIG_MAX_LENGTH);

    if (iError != ERROR_NONE) {
        return;
    }

    iWidth      = config[RESOLUTION_LOC]     + (config[RESOLUTION_LOC + 1] << 8);
    iHeight     = config[RESOLUTION_LOC + 2] + (config[RESOLUTION_LOC + 3] << 8);
    iTouchNum   = config[MAX_CONTACTS_LOC];
    iIrqTrigger = config[TRIGGER_LOC];

    printk(KERN_INFO "Goodix Touch Screen config: [%d, %d], num: %d, trigger: 0x%02x\n",
                     iWidth, iHeight, iTouchNum + 1, iIrqTrigger);
}
/*********************************************************************************************************
** 函数名称: __gt911IrqAck
** 功能描述: TOUCH 触摸屏通知芯片中断已处理
** 输　入  : pTouchDev         TOUCH 设备结构
** 输　出  : 返回值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static VOID  __gt911IrqAck (PTOUCH_DEV  pTouchDev)
{
    UINT8  ucValue = 0;

    __gt911RegWrite(pTouchDev->TOUCH_pI2cDevice,
                    GOODIX_READ_COOR_ADDR,
                    &ucValue,
                    1);
}
/*********************************************************************************************************
** 函数名称: __gt911GetTouchPoint
** 功能描述: 获得 TOUCH 触摸坐标值和状态
** 输　入  : pTouchDev         TOUCH 设备结构
**           events            存储接收到的触摸点信息
**           pucData           数据
** 输　出  : 返回获取到的点数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __gt911GetTouchPoint (PTOUCH_DEV            pTouchDev,
                                  mouse_event_notify    events[],
                                  UINT8                *pucData)
{
    INT  iTouchPoint;

    iTouchPoint = pucData[0] & 0x0F;
    if (iTouchPoint > TOUCH_MAX_INPUT_POINTS) {
        iTouchPoint = TOUCH_MAX_INPUT_POINTS;
    }

    if (iTouchPoint == 0) {

        events[0].xmovement = pTouchDev->TOUCH_iLastX;
        events[0].ymovement = pTouchDev->TOUCH_iLastY;
        events[0].ctype     = MOUSE_CTYPE_ABS;
        events[0].kstat     = 0;

        iTouchPoint = TOUCH_RELEASE_NUM;

    } else {                                                            /*  Support Only 1 pointer now  */

        events[0].xmovement = (INT16) (pucData[3]  << 8) | (INT16)pucData[2];
        events[0].ymovement = (INT16)((pucData[5]) << 8) | (INT16)pucData[4];
        events[0].ctype     = MOUSE_CTYPE_ABS;
        events[0].kstat     = MOUSE_LEFT;

        pTouchDev->TOUCH_iLastX = events[0].xmovement;
        pTouchDev->TOUCH_iLastY = events[0].ymovement;

        iTouchPoint = 1;
    }

    return  (iTouchPoint);
}
/*********************************************************************************************************
** 函数名称: gt911GetEvent
** 功能描述: 获得 TOUCH 触摸坐标值和状态
** 输　入  : pTouchDev         TOUCH 设备结构
**           events            存储接收到的触摸点信息
** 输　出  : 返回获取到的点数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  gt911GetEvent (PTOUCH_DEV  pTouchDev, mouse_event_notify  events[])
{
    INT             iError;
    UCHAR           ucBuffer[36];

    iError = __gt911RxData(pTouchDev, ucBuffer, GOODIX_CONTACT_SIZE + 1);
    if (iError == PX_ERROR) {
        printk(KERN_WARNING "touch: get touch point error!\n");
        return  (PX_ERROR);
    }

    if (ucBuffer[0] & 0x80) {
        iError = __gt911GetTouchPoint(pTouchDev, events, ucBuffer);
    } else {
        iError = 0;
    }

    __gt911IrqAck(pTouchDev);

    return  (iError);
}
/*********************************************************************************************************
** 函数名称: gt911Init
** 功能描述: 初始化时进行的操作
** 输　入  : pTouchDev         TOUCH 设备结构
** 输　出  : 返回获取到的点数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  gt911Init (PTOUCH_DEV  pTouchDev)
{
    __goodixVerCheck(pTouchDev);

    __goodixSpeedSet(pTouchDev, 10);
    __goodixPointSet(pTouchDev, pTouchDev->TOUCH_data.T_iTouchNum);

    __goodixReadConfig(pTouchDev);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
