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
** 文   件   名: core_cfg.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 09 日
**
** 描        述: 这是微内核基本配置文件.
*********************************************************************************************************/

#ifndef __CORE_CFG_H
#define __CORE_CFG_H

/*********************************************************************************************************
  内核能力
*********************************************************************************************************/
#define MX_CFG_MAX_PROCESSORS           32                              /*  支持最大物理核个数, 1-2048  */ 
#define MX_CFG_MAX_PRTNS                16                              /*  支持最大进程个数            */
#define MX_CFG_MAX_VCPUS                256                             /*  支持最大线程个数            */
#define MX_CFG_MAX_IRQS                 256                             /*  支持最大逻辑中断个数        */
/*********************************************************************************************************
  微内核堆管理的段个数
*********************************************************************************************************/
#define MX_CFG_MAX_SEGMENTS             100
/*********************************************************************************************************
  内存裁剪
*********************************************************************************************************/
#define MX_CFG_PAGE_SIZE                (1<<12)                         /*  页面大小(IPC_MSG 计算消息数)*/
/*********************************************************************************************************
  VCPU 优先级
*********************************************************************************************************/
#define MX_PRIO_HIGHEST                 0                               /*  VCPU 最高优先级             */
#define MX_PRIO_LOWEST                  1023                            /*  VCPU 最低优先级             */
/*********************************************************************************************************
  IPC 配置
*********************************************************************************************************/
#define MX_CFG_IPC_PRIO_MAX             (MX_PRIO_LOWEST+1)              /*  IPC 优先级请求表大小        */
/*********************************************************************************************************
  中断嵌套最大层数
*********************************************************************************************************/
#define MX_CFG_MAX_INTER_NESTING        10                              /*  最多允许中断嵌套层数        */
/*********************************************************************************************************
  内核栈占用空间
*********************************************************************************************************/
#define MX_CFG_KSTACK_SIZE              0x4000                          /*  低地址空间是 MX_VCPU 结构体 */
/*********************************************************************************************************
  默认用户栈个数
*********************************************************************************************************/
#define MX_CFG_MAX_USTACKS              4
/*********************************************************************************************************
  默认用户堆栈占用空间
*********************************************************************************************************/
#define MX_CFG_USTACK_SIZE              0x400                           /*  每个 USTACK 4KB 字节        */
/*********************************************************************************************************
  内核地址空间
*********************************************************************************************************/
#define MX_CFG_KERN_VIRT_ADDR           0xC0000000                      /*  内核虚拟地址起始            */
/*********************************************************************************************************
  TICK 逻辑中断号
*********************************************************************************************************/
#define MX_CFG_TICK_VEC                 27                              /*  TICK 逻辑中断号             */

#endif                                                                  /*  __CORE_CFG_H                */
/*********************************************************************************************************
  END
*********************************************************************************************************/

