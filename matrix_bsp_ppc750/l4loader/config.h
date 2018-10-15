/*
 * config.h
 *
 *  Created on: 2018年10月11日
 *      Author: Lee
 */

#ifndef __BSP_CONFIG_H
#define __BSP_CONFIG_H

/*********************************************************************************************************
  ROM RAM 相关配置
*********************************************************************************************************/

#define BSP_CFG_ROM_BASE (0xFF000000)
#define BSP_CFG_ROM_SIZE (16 * 1024 * 1024)

#define BSP_CFG_RAM_BASE (0x00000000)
#define BSP_CFG_RAM_SIZE (256 * 1024 * 1024)
#define BSP_CFG_RAM_RESERVED_SIZE (16 * 1024 * 1024)

#define BSP_CFG_OS_BASE (0x1000000)
#define BSP_CFG_OS_SIZE (240 * 1024 * 1024)

#define BSP_CFG_TEXT_SIZE (6 * 1024 * 1024)
#define BSP_CFG_DATA_SIZE (42 * 1024 * 1024)
#define BSP_CFG_DMA_SIZE (6 * 1024 * 1024)
#define BSP_CFG_APP_SIZE (186 * 1024 * 1024)

#define BSP_CFG_BOOT_STACK_SIZE (128 * 1024)

#define BSP_CFG_DEFAULT_START_PARAM         ""

#endif                                                                  /*  __BSP_CONFIG_H              */
