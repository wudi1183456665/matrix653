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
** 文   件   名: unit_boot.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 6 日
**
** 描        述: 单元测试驱动代码
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
** 函数名称: T_funcUserTestThread
** 功能描述: 应用单元测试驱动线程; 用户态
** 输　入  : pvArg
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  T_funcUserTestThread(PVOID pvArg)
{
    PT_VCPU      pvcpuUnit = (PT_VCPU)pvArg;
    MX_OBJECT_ID objRoot   = (MX_OBJECT_ID)pvcpuUnit->pvArg;

    // PMX_IPC_MSG  pIpcMsg   = T_vcpuGetCurMsg();

    T_TITLE(T_funcUserTestThread);

#if 1   /*  DEBUG: 并发测试 IPC        */
    T_vcpuSleep(SC_TIME_MAKE(1, SC_UTIME_L));
    do {
        MX_OBJECT_ID objLog;
        T_DEBUG(": FIND log_service id**********");
        objLog = TI_rootSerFind(objRoot, TSER_CODE_LOG);
        T_DEBUG(": log_service_id=0x%x*********************", objLog);
        T_vcpuSleep(SC_TIME_MAKE(3, SC_UTIME_L));
    } while(1);
#endif

    do {
        T_DEBUG("(^_^)USER UNIT TEST LOOP*******************(^_^)");
        T_vcpuSleep(SC_TIME_MAKE(2, SC_UTIME_L));

    } while(1);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
