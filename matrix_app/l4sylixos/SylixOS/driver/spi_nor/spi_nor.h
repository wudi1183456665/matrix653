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
** 文   件   名: spi_nor.h
**
** 创   建   人: He.Xin (何 鑫)
**
** 文件创建日期: 2016 年 10 月 27 日
**
** 描        述: SPI_NORFLASH 驱动(25VF016B = 2MB)
*********************************************************************************************************/
#ifndef SPI_NOR_H_
#define SPI_NOR_H_

INT  spiNorDrvInstall(CPCHAR pcName, UINT ulOffsetBytes);
INT  norDevCreate(PCHAR pSpiBusName, UINT uiSSPin);

#endif                                                                      /*  SPI_NOR_H_              */
/*********************************************************************************************************
  END
*********************************************************************************************************/
