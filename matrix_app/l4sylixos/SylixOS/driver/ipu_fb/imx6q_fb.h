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
** 文   件   名: imx6q_fb.h
**
** 创   建   人: zhang.xu (张旭)
**
** 文件创建日期: 2015 年 03 月 06 日
**
** 描        述: imx6q LCD 驱动
*********************************************************************************************************/
#ifndef IMX6Q_FB_H_
#define IMX6Q_FB_H_

#include "ipu_fb/ipu_common.h"

/*********************************************************************************************************
  LCD 并行接口显示参数设置结构体 此参数配合 飞凌嵌入式 OKMX6Q 的
*********************************************************************************************************/
//ips_dev_panel_t disp_vga = {
//   "VGA OUT",                                                         /* name                         */
//	 VGAOUT_XGA,                                                        /* panel id flag                */
//	 DISP_DEV_VGA,                                                      /* panel type                   */
//	 DCMAP_RGB888,                                                      /* data format for panel        */
//     60,                                                                /* refresh rate                 */
//     800,                                                               /* panel width                  */
//     600,                                                               /* panel height                 */
//     33300000,                                                          /* pixel clock frequency        */
//     210,                                                               /* hsync start width            */
//     46,                                                                /* hsync width                  */
//     22,                                                                /* hsyn back width              */
//     23,                                                                /* vysnc start width            */
//     10,                                                                /* vsync width                  */
//     10,                                                                /* vsync back width             */
//     0,                                                                 /* delay from hsync to vsync    */
//     0,                                                                 /* interlaced mode              */
//     0,                                                                 /* clock selection, internal    */
//     0,                                                                 /* clock polarity               */
//     0,                                                                 /* hsync polarity               */
//     0,                                                                 /* vync polarity                */
//     1,                                                                 /* drdy polarity                */
//     0,                                                                 /* data polarity                */
//     NULL,                                                              /* initialization               */
//     NULL,                                                              /* deinit                       */
//     };
///*********************************************************************************************************
//  lvds 显示参数设置结构体，将其中的lvds初始化函数删除了，需要注意
//*********************************************************************************************************/
//ips_dev_panel_t disp_lvds = {
//     "HannStar XGA LVDS",                                               /* name                         */
//     HannStar_XGA_LVDS,                                                 /* panel id flag                */
//     DISP_DEV_LVDS,                                                     /* panel type                   */
//     DCMAP_RGB666,                                                      /* data format for panel        */
//     60,                                                                /* refresh rate                 */
//     1024,                                                              /* panel width                  */
//     600,                                                               /* panel height                 */
//     65000000,                                                          /* pixel clock frequency        */
//     220,                                                               /* hsync start width            */
//     60,                                                                /* hsync width                  */
//     40,                                                                /* hsyn back width              */
//     21,                                                                /* vysnc start width            */
//     10,                                                                /* vsync width                  */
//     7,                                                                 /* vsync back width             */
//     0,                                                                 /* delay from hsync to vsync    */
//     0,                                                                 /* interlaced mode              */
//     1,                                                                 /* clock selection, external    */
//     1,                                                                 /* clock polarity               */
//     1,                                                                 /* hsync polarity               */
//     1,                                                                 /* vync polarity                */
//     1,                                                                 /* drdy polarity                */
//     0,                                                                 /* data polarity                */
//     NULL,
//     NULL,
// };
/*********************************************************************************************************
   功能函数
*********************************************************************************************************/
INT  imx6qIpuDevCreate(UINT  uiPwrPin, CPCHAR  ucpBlkDev, ips_dev_panel_t  *disp_panel);
#endif                                                                  /* IMX6Q_FB_H_                  */
/*********************************************************************************************************
   END
*********************************************************************************************************/
