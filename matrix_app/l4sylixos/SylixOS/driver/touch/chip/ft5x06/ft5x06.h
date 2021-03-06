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
** 文   件   名: ft5x06.h
**
** 创   建   人: Lu.ZhenPing (卢振平)
**
** 文件创建日期: 2014 年 07 月 10 日
**
** 描        述: FT5X06 触摸屏驱动头文件.
*********************************************************************************************************/
#ifndef __FT5X06_H
#define __FT5X06_H

/*********************************************************************************************************
  函数声明
*********************************************************************************************************/
INT  ft5x06GetEvent(PTOUCH_DEV  pTouchDev, mouse_event_notify  events[]);
INT  ft5x06Ioctl(PTOUCH_DEV  pTouchDev, INT  iCmd, LONG  lArg);

#endif                                                                  /*  __FT5X06_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
