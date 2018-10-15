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
** 文   件   名: log_server.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 3 日
**
** 描        述: 日志服务
*********************************************************************************************************/
#include "t_sys.h"
#include "service/service.h"
/*********************************************************************************************************
  函数声明
*********************************************************************************************************/
VOID T_logService (MX_OBJECT_ID  objRoot);
/*********************************************************************************************************
 *  注册服务
 *
 *  进入服务状态: T_logService()
*********************************************************************************************************/
VOID T_funcLogThread (PVOID  pvArg)
{
    PT_VCPU      pvcpuLog = (PT_VCPU)pvArg;
    MX_OBJECT_ID objRoot  = (MX_OBJECT_ID)pvcpuLog->pvArg;

    T_TITLE(T_funcLogThread);

    T_vcpuSleep(SC_TIME_MAKE(1, SC_UTIME_L));                           /*  待服务器启动                */

#if 1
    T_DEBUG(": REGISTER log_server");
    TI_rootSerRegister(objRoot, TSER_CODE_LOG);                         /*  注册日志服务                */
#endif

#if 1   /*  DEBUG: 测试查找服务         */
{
    MX_OBJECT_ID objLog;
    T_DEBUG(": FIND log_service id");
    objLog = TI_rootSerFind(objRoot, TSER_CODE_LOG);
    T_DEBUG(": log_service_id=0x%x", objLog);
}
#endif

#if 0   /*  DEBUG: 测试启动单元测试     */
    T_DEBUG(": TEST boot test_unit");
    if (!TI_rootSerTestBoot(objRoot)) {
        T_ERROR("boot test_unit fail");
    }
#endif

    T_logService(objRoot);
}

VOID T_logService (MX_OBJECT_ID objRoot)
{
#if 1   /*  DEBUG: 并发测试 IPC        */
    do {
        MX_OBJECT_ID objLog;
        T_DEBUG(": FIND log_service id**********");
        objLog = TI_rootSerFind(objRoot, TSER_CODE_LOG);
        T_DEBUG(": log_service_id=0x%x*********", objLog);
        T_vcpuSleep(SC_TIME_MAKE(4, SC_UTIME_L));
    } while(1);
#endif

    do {
        T_DEBUG("(^_^)LOG SERVICE LOOP*******************(^_^)");
        T_vcpuSleep(SC_TIME_MAKE(2, SC_UTIME_L));
    } while (1);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
