/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: iomux_config.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2015 年 01 月 21 日
**
** 描        述: imx6q 处理器 PINMUX 处理驱动文件，这里仅仅是存放通用代码，对具体管脚的分配在相应 board
**               文件中处理
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "pinmux/iomux_config.h"
#include "gpio/imx6q_gpio.h"

/*********************************************************************************************************
  END
*********************************************************************************************************/
