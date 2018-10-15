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
** 文   件   名: ft5x06.c
**
** 创   建   人: Lu.ZhenPing (卢振平)
**
** 文件创建日期: 2014 年 07 月 10 日
**
** 描        述: FT5X06 触摸屏驱动源文件.
**
** BUG:
** 2015/2/7    焦进星 重构代码
** 2016/11/12  徐贵洲 重新整理触摸屏驱动框架
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include <input_device.h>
#include <string.h>
#include <stdlib.h>
#include <mouse.h>

#include "touch/touch.h"
/*********************************************************************************************************
  FT5X06 触摸屏寄存器相关(power consumption mode)
*********************************************************************************************************/
#define __FT5X06_ID_G_PMODE                 (0xA5)                      /*  电源模式地址                */
#define __FT5X06_PMODE_ACTIVE               (0)                         /*  激活模式                    */
#define __FT5X06_PMODE_MONITOR              (0x01)                      /*  监视模式                    */
#define __FT5X06_PMODE_HIBERNATE            (0X03)                      /*  睡眠模式                    */
/*********************************************************************************************************
** 函数名称: __ft5x06RegRead
** 功能描述: 从 TOUCH 接收数据
** 输　入  : pTouchDev          TOUCH 设备结构
**           ucReg              FT5x06寄存器地址
**           pucBuffer          接收数据缓冲区
**           usLen              接收数据的长度
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ft5x06RegRead (PLW_I2C_DEVICE      pI2cDev,
                             UINT8               ucReg,
                             UINT8              *pucBuffer,
                             UINT16              usLen)
{
    INT             iError;
    LW_I2C_MESSAGE  i2cMsgs[] = {
        {
            .I2CMSG_usAddr    = pI2cDev->I2CDEV_usAddr,
            .I2CMSG_usFlag    = 0,
            .I2CMSG_usLen     = 1,
            .I2CMSG_pucBuffer = (PUCHAR)&ucReg,
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
        printk(KERN_ERR "__ft5x06RxData(): failed to i2c transfer!\n");
        return  (PX_ERROR);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __ft5x06RegWrite
** 功能描述: 从 TOUCH 接收数据
** 输　入  : pTouchDev          TOUCH 设备结构
**           ucReg              FT5x06寄存器地址
**           pucBuffer          发送数据缓冲区
**           usLen              发送数据的长度
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ft5x06RegWrite (PLW_I2C_DEVICE      pI2cDev,
                              UINT8               ucReg,
                              UINT8              *pucBuffer,
                              UINT16              usLen)
{
    INT             iError;
    PUCHAR          pucWrBuf;
    LW_I2C_MESSAGE  i2cMsgs;

    pucWrBuf  = mallocalign(usLen + 1, 4);
    if (pucWrBuf == NULL) {
        printk(KERN_ERR "__ft5x06RegWrite(): insufficient mem!\n");
        return  (PX_ERROR);
    }

    pucWrBuf[0]  = ucReg;

    memcpy(pucWrBuf + 1, pucBuffer, usLen);

    i2cMsgs.I2CMSG_usAddr       = pI2cDev->I2CDEV_usAddr;
    i2cMsgs.I2CMSG_usFlag       = 0;
    i2cMsgs.I2CMSG_usLen        = usLen + 1;
    i2cMsgs.I2CMSG_pucBuffer    = pucWrBuf;

    iError = API_I2cDeviceTransfer(pI2cDev, &i2cMsgs, 1);
    if (iError != 1) {
        printk(KERN_ERR "__ft5x06RegWrite(): failed to i2c transfer!\n");
        free(pucWrBuf);
        return  (PX_ERROR);
    }

    free(pucWrBuf);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __ft5x06SetRegister
** 功能描述: 设置 FT5X06 寄存器
** 输　入  : pTouchDev         FT5X06 设备结构
**           ucRegAddr         寄存器地址
**           ucValue           要设置的值
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ft5x06SetRegister (PTOUCH_DEV  pTouchDev, UINT8  ucRegAddr, UINT8  ucValue)
{
    return  (__ft5x06RegWrite(pTouchDev->TOUCH_pI2cDevice, ucRegAddr, &ucValue, 1));
}
/*********************************************************************************************************
** 函数名称: __ft5x06RxData
** 功能描述: 从 TOUCH 接收数据
** 输　入  : pTouchDev            TOUCH 设备结构
**           pucBuffer            接收数据缓冲区
**           usLen                接收数据的长度
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ft5x06RxData (PTOUCH_DEV  pTouchDev, UINT8  *pucBuffer, UINT16  usLen)
{
    INT     iError;

    iError = __ft5x06RegRead(pTouchDev->TOUCH_pI2cDevice,
                             0,
                             pucBuffer,
                             usLen);
    return  (iError);
}
/*********************************************************************************************************
** 函数名称: __ft5x06GetTouchPoint
** 功能描述: 获得 FT5X06 触摸坐标值和状态
** 输　入  : pTouchDev         TOUCH 设备结构
**           events            存储接收到的触摸点信息
**           pucData           数据
** 输　出  : 返回获取到的点数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ft5x06GetTouchPoint (PTOUCH_DEV            pTouchDev,
                                   mouse_event_notify    events[],
                                   UINT8                *pucData)
{
    INT  iTouchPoint;

    iTouchPoint = pucData[2] & 0x07;
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

        events[0].xmovement = (INT16)(pucData[3] & 0x0F) << 8 | (INT16)pucData[4];
        events[0].ymovement = (INT16)(pucData[5] & 0x0F) << 8 | (INT16)pucData[6];
        events[0].ctype     = MOUSE_CTYPE_ABS;
        events[0].kstat     = MOUSE_LEFT;

        pTouchDev->TOUCH_iLastX = events[0].xmovement;
        pTouchDev->TOUCH_iLastY = events[0].ymovement;

        iTouchPoint = 1;
    }

    return  (iTouchPoint);
}
/*********************************************************************************************************
** 函数名称: __ft5x06Ioctl
** 功能描述: 控制 FT5X06 触摸屏设备
** 输　入  : pFdEntry              文件结构
**           iCmd                  命令
**           lArg                  参数
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  ft5x06Ioctl (PTOUCH_DEV  pTouchDev, INT  iCmd, LONG  lArg)
{
    switch (iCmd) {

    case PM_ACTIVE_CMD:                                                 /*  激活触摸屏                  */
        return  (__ft5x06SetRegister(pTouchDev,
                                     __FT5X06_ID_G_PMODE,
                                     __FT5X06_PMODE_ACTIVE));
        break;

    case PM_MONITOR_CMD:                                                /*  监视模式                    */
        return  (__ft5x06SetRegister(pTouchDev,
                                     __FT5X06_ID_G_PMODE,
                                     __FT5X06_PMODE_MONITOR));
        break;

    case PM_HIBERNATE_CMD:                                              /*  睡眠模式                    */
        return  (__ft5x06SetRegister(pTouchDev,
                                     __FT5X06_ID_G_PMODE,
                                     __FT5X06_PMODE_HIBERNATE));
        break;

    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** 函数名称: ft5x06GetEvent
** 功能描述: 获得 TOUCH 触摸坐标值和状态
** 输　入  : pTouchDev         TOUCH 设备结构
**           events            存储接收到的触摸点信息
** 输　出  : 返回获取到的点数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  ft5x06GetEvent(PTOUCH_DEV  pTouchDev, mouse_event_notify  events[])
{
    INT             iError;
    UCHAR           ucBuffer[32];

    INT             iTouchMax = pTouchDev->TOUCH_data.T_iTouchNum;

    iError = __ft5x06RxData(pTouchDev, ucBuffer, 3 + 6 * iTouchMax);
    if (iError == PX_ERROR) {
        printk(KERN_WARNING "touch: get touch point error!\n");
        return  (PX_ERROR);
    }

    iError = __ft5x06GetTouchPoint(pTouchDev, events, ucBuffer);

    return  (iError);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
