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
** 文   件   名: k_syscall.h
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 01 月 05 日
**
** 描        述: IPC 数据结构与接口定义
*********************************************************************************************************/
/*********************************************************************************************************
   
   系统调用使用 4 个寄存器传递参数


   +------------+      +-------------------------------------+
   |    ARG0    |----> | Reserved | Result | OpCode | CallId |
   | (寄存器R0) |      +-------------------------------------+
   +------------+
   +------------+
   |    ARG1    |----> 一般存放内核对象 ID
   | (寄存器R1) |
   +------------+
   +------------+
   |    ARG2    |----+
   | (寄存器R2) |    |
   +------------+    |-> 一般存放超时时间
   +------------+    |
   |    ARG3    |----+
   | (寄存器R3) |
   +------------+

*********************************************************************************************************/

#ifndef __K_SYSCALL_H
#define __K_SYSCALL_H

/*********************************************************************************************************
  系统调用参数结构体
*********************************************************************************************************/
typedef struct {
    UINT32            SCPARAM_ui32Arg0;
    UINT32            SCPARAM_ui32Arg1;
    UINT32            SCPARAM_ui32Arg2;
    UINT32            SCPARAM_ui32Arg3;
} MX_SC_PARAM;
typedef MX_SC_PARAM  *PMX_SC_PARAM;
/*********************************************************************************************************
  获取 VCPU 控制块中的系统参数结构体指针
*********************************************************************************************************/
#define MX_SC_PARAM_GET(pvcpu)        (&pvcpu->VCPU_scparam)
/*********************************************************************************************************
  系统调用的首个参数内部结构
*********************************************************************************************************/
typedef struct {
    UINT8             SCARG0_ucCallId;                                  /*  系统调用号                  */
    UINT8             SCARG0_ucOpCode;                                  /*  系统调用操作码              */
    UINT8             SCARG0_ucResult;                                  /*  系统调用结果                */
    UINT8             SCARG0_ucReserved;                                /*  保留                        */
} MX_SC_ARG0;
typedef MX_SC_ARG0   *PMX_SC_ARG0;
/*********************************************************************************************************
  设置 Arg0 中的系统调用号
*********************************************************************************************************/
#define MX_SC_CALLID_SET(pscparam, ucCallId)        \
                                    ((PMX_SC_ARG0)&pscparam->SCPARAM_ui32Arg0)->SCARG0_ucCallId = ucCallId
/*********************************************************************************************************
  获取 Arg0 中的系统调用号
*********************************************************************************************************/
#define MX_SC_CALLID_GET(pscparam)                  \
                                    ((PMX_SC_ARG0)&pscparam->SCPARAM_ui32Arg0)->SCARG0_ucCallId
/*********************************************************************************************************
  设置 Arg0 中的操作码
*********************************************************************************************************/
#define MX_SC_OPCODE_SET(pscparam, ucOpCode)        \
                                    ((PMX_SC_ARG0)&pscparam->SCPARAM_ui32Arg0)->SCARG0_ucOpCode = ucOpCode
/*********************************************************************************************************
  获取 Arg0 中的操作码
*********************************************************************************************************/
#define MX_SC_OPCODE_GET(pscparam)                  \
                                    ((PMX_SC_ARG0)&pscparam->SCPARAM_ui32Arg0)->SCARG0_ucOpCode
/*********************************************************************************************************
  设置 Arg0 中的错误码
*********************************************************************************************************/
#define MX_SC_RESULT_SET(pscparam, ucResult)        \
                                    ((PMX_SC_ARG0)&pscparam->SCPARAM_ui32Arg0)->SCARG0_ucResult = ucResult
/*********************************************************************************************************
  获取 Arg0 中的错误码
*********************************************************************************************************/
#define MX_SC_RESULT_GET(pscparam)                  \
                                    ((PMX_SC_ARG0)&pscparam->SCPARAM_ui32Arg0)->SCARG0_ucResult
/*********************************************************************************************************
  设置 Arg1 中的内核对象 ID
*********************************************************************************************************/
#define MX_SC_KOBJ_SET(pscparam, kobjId)           (pscparam)->SCPARAM_ui32Arg1 = kobjId
/*********************************************************************************************************
  获取 Arg1 中的内核对象 ID
*********************************************************************************************************/
#define MX_SC_KOBJ_GET(pscparam)                   (pscparam)->SCPARAM_ui32Arg1
/*********************************************************************************************************
  设置 Arg2, Arg3 中的时间值
*********************************************************************************************************/
#define MX_SC_TIME_SET(pscparam, ui64Time)                              \
            do {                                                        \
                pscparam->SCPARAM_ui32Arg2 = (UINT32)(ui64Time);        \
                pscparam->SCPARAM_ui32Arg3 = (UINT32)(ui64Time >> 32);  \
            } while (0)
/*********************************************************************************************************
  获取 Arg2, Arg3 中的时间值
*********************************************************************************************************/
#define MX_SC_TIME_GET(pscparam)                                        \
            (((UINT64)(pscparam->SCPARAM_ui32Arg3) << 32) | (pscparam->SCPARAM_ui32Arg2))

/*********************************************************************************************************
  调用号类型
*********************************************************************************************************/
typedef UINT8         MX_SC_CALLID_T;
/*********************************************************************************************************
  系统调用号.  参数 0 中的 CALLID
*********************************************************************************************************/
#define MX_SC_CALLID_IPC              1
#define MX_SC_CALLID_IRQ              2
#define MX_SC_CALLID_PRTN             3
#define MX_SC_CALLID_VCPU             4
#define MX_SC_CALLID_DEBUG            5
/*********************************************************************************************************
  操作码类型
*********************************************************************************************************/
typedef UINT8         MX_SC_OPCODE_T;
/*********************************************************************************************************
  IRQ 系统调用操作码.  参数 0 中的 OPCODE
*********************************************************************************************************/
#define MX_SC_OPCODE_IRQ_REGISTER     1
#define MX_SC_OPCODE_IRQ_ATTACH       2
#define MX_SC_OPCODE_IRQ_ENABLE       3
#define MX_SC_OPCODE_IRQ_DISABLE      4
#define MX_SC_OPCODE_SWI_REGISTER     9
/*********************************************************************************************************
  PRTN 系统调用操作码.  参数 0 中的 OPCODE
*********************************************************************************************************/
#define MX_SC_OPCODE_PRTN_CREATE      1
#define MX_SC_OPCODE_PRTN_RESTART     2
/*********************************************************************************************************
  VCPU 系统调用操作码.  参数 0 中的 OPCODE
*********************************************************************************************************/
#define MX_SC_OPCODE_VCPU_CREATE      1
#define MX_SC_OPCODE_VCPU_START       2
#define MX_SC_OPCODE_VCPU_SLEEP       3
#define MX_SC_OPCODE_VCPU_WAKEUP      4
#define MX_SC_OPCODE_VCPU_DIS_INT     5
#define MX_SC_OPCODE_VCPU_EN_INT      6
/*********************************************************************************************************
  DEBUG 系统调用操作码.  参数 0 中的 OPCODE
*********************************************************************************************************/
#define MX_SC_OPCODE_DEBUG_PRINT      1
#define MX_SC_OPCODE_DEBUG_READ       2
#define MX_SC_OPCODE_DEBUG_WRITE      3
/*********************************************************************************************************
  错误码类型
*********************************************************************************************************/
typedef UINT8         MX_SC_RESULT_T;
/*********************************************************************************************************
  系统调用结果(通用).  参数 0 中的 RESULT
*********************************************************************************************************/
#define MX_SC_RESULT_OK               0                                 /*  操作成功                    */
#define MX_SC_RESULT_FAIL             1                                 /*  其它错误                    */
#define MX_SC_RESULT_ILLEGAL          2                                 /*  非法的系统调用              */
#define MX_SC_RESULT_PARAM            3                                 /*  系统调用参数错误            */
/*********************************************************************************************************
  系统调用结果(IPC).  参数 0 中的 RESULT
*********************************************************************************************************/
#define MX_SC_RESULT_SER_SIGNAL       249                               /*  服务 IPC 请求计数错误       */
#define MX_SC_RESULT_MEM              250                               /*  内存错误                    */
#define MX_SC_RESULT_STATE            251                               /*  IPC 状态错误                */
#define MX_SC_RESULT_NEED_REPLY       252                               /*  需回复 IPC 请求             */
#define MX_SC_RESULT_INVALID_OBJ      253                               /*  目标 VCPU 无效              */
#define MX_SC_RESULT_NOT_SER          254                               /*  目标 VCPU 不在接收状态      */
#define MX_SC_RESULT_TIMEOUT          255                               /*  IPC 超时                    */

/*********************************************************************************************************
  系统调用时间, 用于表示请求超时或休眠时间
*********************************************************************************************************/
typedef UINT64      SC_TIME;
/*********************************************************************************************************
  SC_TIME 位域所表示含义
*********************************************************************************************************/
#define SC_TIME_UNIT                  ((0x7 | 0ULL) << 56)              /*  BIT: 56-58: 时间单位        */
#define SC_TIME_VALUE                 ( ~0 | 0ULL)                      /*  BIT:  0-31: 自定义时间值    */
/*********************************************************************************************************
  获取系统调用时间单位
*********************************************************************************************************/
#define SC_TIME_UNIT_GET(ui64Time)    ((UINT8)((ui64Time & SC_TIME_UNIT) >> 56))
/*********************************************************************************************************
  获取系统调用时间值
*********************************************************************************************************/
#define SC_TIME_VALUE_GET(ui64Time)   ((ULONG)(ui64Time & SC_TIME_VALUE))
/*********************************************************************************************************
  自定义时间: uiValue - 时间数值 ucUnit  - 时间单位
*********************************************************************************************************/
#define SC_TIME_MAKE(uiValue, ucUnit) ((SC_TIME)((uiValue & SC_TIME_VALUE) | ((ucUnit & 0x7ULL) << 56)))
/*********************************************************************************************************
  时间单位
*********************************************************************************************************/
#define SC_UTIME_TICK                 0                                 /*  选本单位时 VALUE 就是 Tick  */
#define SC_UTIME_DEF                  1                                 /*  10us                        */
#define SC_UTIME_SS                   2                                 /*  100us                       */
#define SC_UTIME_S                    3                                 /*  1ms                         */
#define SC_UTIME_M                    4                                 /*  10ms                        */
#define SC_UTIME_MM                   5                                 /*  100ms                       */
#define SC_UTIME_L                    6                                 /*  1s                          */
#define SC_UTIME_FOREVER              7                                 /*  永久, 仅由 wakeup 操作唤醒  */
/*********************************************************************************************************
  一个 Tick 周期需要多少个 SC_UTIME_DEF(10us)
*********************************************************************************************************/
#define SC_TICK_TIME                  1000                              /*  由 BSP 决定                 */
/*********************************************************************************************************
  预定义时间
*********************************************************************************************************/
#define SC_DTIME_SYN                  0                                  /*  同步请求, 无阻塞           */
#define SC_DTIME_DEF                  SC_TIME_MAKE(2000000, SC_UTIME_DEF)/*  20s                        */
#define SC_DTIME_SS                   SC_TIME_MAKE(2      , SC_UTIME_DEF)/*  20us                       */
#define SC_DTIME_S                    SC_TIME_MAKE(20     , SC_UTIME_DEF)/*  200us                      */
#define SC_DTIME_M                    SC_TIME_MAKE(1000   , SC_UTIME_DEF)/*  10ms                       */
#define SC_DTIME_STD                  SC_TIME_MAKE(50000  , SC_UTIME_DEF)/*  500ms                      */
#define SC_DTIME_L                    SC_TIME_MAKE(100000 , SC_UTIME_DEF)/*  1s                         */
#define SC_DTIME_LL                   SC_TIME_MAKE(1000000, SC_UTIME_DEF)/*  10s                        */
#define SC_DTIME_TICK(ticks)          SC_TIME_MAKE(ticks  , SC_UTIME_TICK)/* ticks                      */

#endif                                                                  /*  SYSCALL_H_                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
