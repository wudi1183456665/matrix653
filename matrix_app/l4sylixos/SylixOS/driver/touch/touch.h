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
** ��   ��   ��: touch.h
**
** ��   ��   ��: Xu.Guizhou (�����)
**
** �ļ���������: 2016 �� 11 �� 11 ��
**
** ��        ��: ����������ͨ�ÿ��ͷ�ļ�.
*********************************************************************************************************/
#ifndef __TOUCH_H
#define __TOUCH_H

#include <sys/ioccom.h>
/*********************************************************************************************************
  ����
*********************************************************************************************************/
#define TOUCH_MAX_INPUT_QUEUE               (5)                         /*  max data packets in queue   */
#define TOUCH_MAX_INPUT_POINTS              (5)                         /*  max input points at once    */
#define TOUCH_RELEASE_NUM                   (0xFFFFFF00)                /*  Indicate Release Operation  */
/*********************************************************************************************************
  IOCTL ����
*********************************************************************************************************/
#define PM_ACTIVE_CMD                       (_IOW('p', 0, INT))
#define PM_MONITOR_CMD                      (_IOW('p', 1, INT))
#define PM_HIBERNATE_CMD                    (_IOW('p', 2, INT))
/*********************************************************************************************************
  TOUCH ��������������
  ˵����
  init: ��ʼ��Ӳ���ܽţ���ʼ���ж�
*********************************************************************************************************/
typedef struct touch_drv_func {
    FUNCPTR                 init;
    FUNCPTR                 deinit;
    FUNCPTR                 getevent;
    FUNCPTR                 ioctl;
    VOIDFUNCPTR             reset;
} TOUCH_DRV_FUNC, *PTOUCH_DRV_FUNC;
/*********************************************************************************************************
  TOUCH �������豸����
*********************************************************************************************************/
typedef struct {
    PCHAR                   T_pcBusName;                                /*  i2c ������                  */
    UINT32                  T_uiIrq;                                    /*  IRQ GPIO                    */
    UINT32                  T_uiIrqCfg;                                 /*  IRQ GPIO ����               */
    UINT32                  T_uiReset;                                  /*  Reset GPIO                  */
    UINT32                  T_uiRstVal;                                 /*  Reset GPIO Assert value     */
    UINT32                  T_uiRstTime;                                /*  Time interval for reset (ms)*/
    UINT16                  T_usAddr;                                   /*  I2C Slave Address           */

    UINT                    T_iWidth;                                   /*  Resulotion Width            */
    UINT                    T_iHeight;                                  /*  Resulotion Height           */
    UINT                    T_iTouchNum;                                /*  Max Touch Point             */

#define T_XY_NORMAL         0
#define T_X_REVERT          (1 << 0)
#define T_Y_REVERT          (1 << 1)
#define T_XY_SWAP           (1 << 2)
    UINT                    T_iDirFlag;                                 /*  Direction Flag              */
} TOUCH_DATA, *PTOUCH_DATA;
/*********************************************************************************************************
  TOUCH �������豸���Ͷ���
*********************************************************************************************************/
typedef struct {
    LW_DEV_HDR                  TOUCH_devHdr;
    LW_LIST_LINE_HEADER         TOUCH_fdNodeHeader;
    LW_SEL_WAKEUPLIST           TOUCH_selList;
    time_t                      TOUCH_time;
    INT                         TOUCH_drvNum;

    LW_HANDLE                   TOUCH_hEventQueue;
    LW_HANDLE                   TOUCH_hSignal;
    LW_HANDLE                   TOUCH_hThread;
    volatile BOOL               TOUCH_bQuit;
    PLW_I2C_DEVICE              TOUCH_pI2cDevice;                       /*  I2C �豸                    */
    TOUCH_DATA                  TOUCH_data;                             /*  �������豸����              */
    ULONG                       TOUCH_ulVector;

    INT                         TOUCH_iLastX;
    INT                         TOUCH_iLastY;

    PTOUCH_DRV_FUNC             pDrvFunc;
} TOUCH_DEV, *PTOUCH_DEV;
/*********************************************************************************************************
  ��������
*********************************************************************************************************/
INT  touchDrv(VOID);
INT  touchDevCreate(CPCHAR             pcName,
                    INT                iDrvNum,
                    PTOUCH_DATA        pTouchData,
                    PTOUCH_DRV_FUNC    pDrvFunc);

#endif                                                                  /*  __TOUCH_H                   */
/*********************************************************************************************************
  END
*********************************************************************************************************/
