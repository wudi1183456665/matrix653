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
** 文   件   名: ecspi.h
**
** 创   建   人: Hu.Hanzhang (胡瀚璋)
**
** 文件创建日期: 2016 年 06 月 12 日
**
** 描        述: IMX6 处理器 ECSPI驱动
*********************************************************************************************************/
#ifndef ECSPI_SPI_H_
#define ECSPI_SPI_H_

/*********************************************************************************************************
  control cmd
*********************************************************************************************************/
#define    BAUDRATE_MAX         60000000
#define    BAUDRATE_SET         0x01
#define    SPI_MODE_SET         0x02                                    /*  lArg  0-3模式   4-7通道     */
/*********************************************************************************************************
** 函数名称: spiBusDrv
** 功能描述: 初始化 spi 总线并获取驱动程序
** 输　入  : uiChannel         通道号
**           pPinMux           管脚复用
** 输　出  : spi 总线驱动程序
** 全局变量:
** 调用模块:
*********************************************************************************************************/
PLW_SPI_FUNCS  spiBusDrv(UINT  uiChannel, VOIDFUNCPTR  pvHwPinInit);

#endif                                                                  /*  ECSPI_SPI_H_                */
/*********************************************************************************************************
  END
*********************************************************************************************************/
