/**********************************************************************************************************
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
** 文   件   名: bspMap.h
**
** 创   建   人: Han.Hui (韩辉)
**
** 文件创建日期: 2008 年 12 月 23 日
**
** 描        述: C 程序入口部分. 物理分页空间与全局映射关系表定义.
*********************************************************************************************************/

#ifndef __BSPMAP_H
#define __BSPMAP_H

/*********************************************************************************************************
  physical memory zone
*********************************************************************************************************/
#ifdef  __BSPINIT_MAIN_FILE

static LW_MMU_PHYSICAL_DESC    _G_physicalDesc[] = {
        {                                                               /*  中断向量表                  */
                BSP_CFG_RAM_BASE,
                0,
                LW_CFG_VMM_PAGE_SIZE,
                LW_PHYSICAL_MEM_VECTOR,
        },

        {                                                               /*  内核代码段                  */
                BSP_CFG_RAM_BASE,
                BSP_CFG_RAM_BASE,
                BSP_CFG_TEXT_SIZE,
                LW_PHYSICAL_MEM_TEXT,
        },

        {                                                               /*  内核数据段                  */
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE,
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE,
                BSP_CFG_DATA_SIZE,
                LW_PHYSICAL_MEM_DATA,
        },

        {                                                               /*  DMA 缓冲区                  */
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE + BSP_CFG_DATA_SIZE,
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE + BSP_CFG_DATA_SIZE,
                BSP_CFG_DMA_SIZE,
                LW_PHYSICAL_MEM_DMA,
        },

        {                                                               /*  APP 通用内存                */
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE + BSP_CFG_DATA_SIZE    \
                                 + BSP_CFG_DMA_SIZE + BSP_CFG_RAM_RSV_SIZE,
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE + BSP_CFG_DATA_SIZE    \
                                 + BSP_CFG_DMA_SIZE + BSP_CFG_RAM_RSV_SIZE,
                BSP_CFG_APP_SIZE,
                LW_PHYSICAL_MEM_APP,
        },

#if 0 //l4
        /*
         * sfr
         */
        {
                0x00000000 + LW_CFG_VMM_PAGE_SIZE,                      /*  ROM 区                      */
                0x00000000 + LW_CFG_VMM_PAGE_SIZE,
                0x00900000 - LW_CFG_VMM_PAGE_SIZE,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  映射的属性                  */
        },

        {                                                               /*  OCRAM                       */
                0x00900000,
                0x00900000,
                256 * LW_CFG_KB_SIZE,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  映射的属性                  */
        },

        {                                                               /*  外设                        */
                0x00A00000,
                0x00A00000,
                0x0F600000,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  映射的属性                  */
        },

        {                                                               /*  AIPS-2 外设地址，           */
                0x02100000,
                0x02100000,
                0x00200000,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  映射的属性                  */
        },

        {                                                               /*  AIPS-1 外设地址，           */
                0x02000000,
                0x02000000,
                0x00100000,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  映射的属性                  */
        },
#endif

        {                                                               /*  结束                        */
                0,
                0,
                0,
                0
        }
};

/*********************************************************************************************************
  virtual memory
*********************************************************************************************************/

static LW_MMU_VIRTUAL_DESC    _G_virtualDesc[] = {
        {                                                               /*  应用程序虚拟空间            */
                BSP_CFG_VAPP_START,
                BSP_CFG_VAPP_SIZE,
                LW_VIRTUAL_MEM_APP
        },

#if 0 //l4
        {
                BSP_CFG_VIO_START,                                      /*  ioremap 空间                */
                BSP_CFG_VIO_SIZE,
                LW_VIRTUAL_MEM_DEV
        },
#endif

        {                                                               /*  结束                        */
                0,
                0,
                0
        }
};
#endif                                                                  /*  __BSPINIT_MAIN_FILE         */
#endif                                                                  /*  __BSPMAP_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
