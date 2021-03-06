/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                嵌入式 L4 微内核操作系统
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: Matrix.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 09 日
**
** 描        述: 这里是系统统一头文件.
*********************************************************************************************************/

#ifndef  __MATRIX_H
#define  __MATRIX_H

/*********************************************************************************************************
  系统配置头文件
*********************************************************************************************************/
#include "../config/cpu/cpu_cfg.h"
#include "../config/core/core_cfg.h"
/*********************************************************************************************************
  体系结构配置
*********************************************************************************************************/
#include "../arch/arch_inc.h"
/*********************************************************************************************************
  链表
*********************************************************************************************************/
#include "../list/list.h"
/*********************************************************************************************************
  内核头文件
*********************************************************************************************************/
#include "k_types.h"
#include "k_error.h"
#include "k_value.h"
#include "k_object.h"
#include "k_resource.h"
#include "k_partition.h"
#include "k_syscall.h"
#include "k_heap.h"
#include "k_ipc.h"
#include "k_vcpu.h"
#include "k_sched.h"
#include "k_cpu.h"
#include "k_globalvar.h"
#include "k_priority.h"
#include "k_api.h"
#include "k_debug.h"
/*********************************************************************************************************
  体系结构支持代码
*********************************************************************************************************/
#include "../arch/arch_support.h"                                       /*  体系架构支持                */
#include "../arch/arch_io.h"                                            /*  体系架构支持                */
#include "../arch/arch_inc.h"                                           /*  体系架构支持                */
//#include "../arch/arch_assembler.h"
#endif                                                                  /*  __MATRIX_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/


