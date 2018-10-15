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
** 文   件   名: cpu_cfg_sparc.h
**
** 创   建   人: Xu.Guizhou (徐贵洲)
**
** 文件创建日期: 2015 年 05 月 15 日
**
** 描        述: SPARC CPU 类型与功能配置.
*********************************************************************************************************/

#ifndef __CPU_CFG_SPARC_H
#define __CPU_CFG_SPARC_H

/*********************************************************************************************************
  CPU 体系结构
*********************************************************************************************************/

#define LW_CFG_CPU_ARCH_SPARC           1                               /*  CPU 架构                    */
#define LW_CFG_CPU_ARCH_FAMILY          "SPARC(R)"                      /*  SPARC family                */

/*********************************************************************************************************
  CPU 字长与整型大小端定义
*********************************************************************************************************/

#define LW_CFG_CPU_ENDIAN               1                               /*  0: 小端  1: 大端            */
#define LW_CFG_CPU_WORD_LENGHT          32                              /*  CPU 字长                    */

#endif                                                                  /*  __CPU_CFG_SPARC_H           */
/*********************************************************************************************************
  END
*********************************************************************************************************/
