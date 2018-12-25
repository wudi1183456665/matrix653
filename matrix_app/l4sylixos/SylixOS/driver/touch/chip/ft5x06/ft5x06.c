/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: ft5x06.c
**
** ��   ��   ��: Lu.ZhenPing (¬��ƽ)
**
** �ļ���������: 2014 �� 07 �� 10 ��
**
** ��        ��: FT5X06 ����������Դ�ļ�.
**
** BUG:
** 2015/2/7    ������ �ع�����
** 2016/11/12  ����� �������������������
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include <input_device.h>
#include <string.h>
#include <stdlib.h>
#include <mouse.h>

#include "touch/touch.h"
/*********************************************************************************************************
  FT5X06 �������Ĵ������(power consumption mode)
*********************************************************************************************************/
#define __FT5X06_ID_G_PMODE                 (0xA5)                      /*  ��Դģʽ��ַ                */
#define __FT5X06_PMODE_ACTIVE               (0)                         /*  ����ģʽ                    */
#define __FT5X06_PMODE_MONITOR              (0x01)                      /*  ����ģʽ                    */
#define __FT5X06_PMODE_HIBERNATE            (0X03)                      /*  ˯��ģʽ                    */
/*********************************************************************************************************
** ��������: __ft5x06RegRead
** ��������: �� TOUCH ��������
** �䡡��  : pTouchDev          TOUCH �豸�ṹ
**           ucReg              FT5x06�Ĵ�����ַ
**           pucBuffer          �������ݻ�����
**           usLen              �������ݵĳ���
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __ft5x06RegWrite
** ��������: �� TOUCH ��������
** �䡡��  : pTouchDev          TOUCH �豸�ṹ
**           ucReg              FT5x06�Ĵ�����ַ
**           pucBuffer          �������ݻ�����
**           usLen              �������ݵĳ���
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __ft5x06SetRegister
** ��������: ���� FT5X06 �Ĵ���
** �䡡��  : pTouchDev         FT5X06 �豸�ṹ
**           ucRegAddr         �Ĵ�����ַ
**           ucValue           Ҫ���õ�ֵ
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __ft5x06SetRegister (PTOUCH_DEV  pTouchDev, UINT8  ucRegAddr, UINT8  ucValue)
{
    return  (__ft5x06RegWrite(pTouchDev->TOUCH_pI2cDevice, ucRegAddr, &ucValue, 1));
}
/*********************************************************************************************************
** ��������: __ft5x06RxData
** ��������: �� TOUCH ��������
** �䡡��  : pTouchDev            TOUCH �豸�ṹ
**           pucBuffer            �������ݻ�����
**           usLen                �������ݵĳ���
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __ft5x06GetTouchPoint
** ��������: ��� FT5X06 ��������ֵ��״̬
** �䡡��  : pTouchDev         TOUCH �豸�ṹ
**           events            �洢���յ��Ĵ�������Ϣ
**           pucData           ����
** �䡡��  : ���ػ�ȡ���ĵ�����
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: __ft5x06Ioctl
** ��������: ���� FT5X06 �������豸
** �䡡��  : pFdEntry              �ļ��ṹ
**           iCmd                  ����
**           lArg                  ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  ft5x06Ioctl (PTOUCH_DEV  pTouchDev, INT  iCmd, LONG  lArg)
{
    switch (iCmd) {

    case PM_ACTIVE_CMD:                                                 /*  �������                  */
        return  (__ft5x06SetRegister(pTouchDev,
                                     __FT5X06_ID_G_PMODE,
                                     __FT5X06_PMODE_ACTIVE));
        break;

    case PM_MONITOR_CMD:                                                /*  ����ģʽ                    */
        return  (__ft5x06SetRegister(pTouchDev,
                                     __FT5X06_ID_G_PMODE,
                                     __FT5X06_PMODE_MONITOR));
        break;

    case PM_HIBERNATE_CMD:                                              /*  ˯��ģʽ                    */
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
** ��������: ft5x06GetEvent
** ��������: ��� TOUCH ��������ֵ��״̬
** �䡡��  : pTouchDev         TOUCH �豸�ṹ
**           events            �洢���յ��Ĵ�������Ϣ
** �䡡��  : ���ػ�ȡ���ĵ�����
** ȫ�ֱ���:
** ����ģ��:
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
