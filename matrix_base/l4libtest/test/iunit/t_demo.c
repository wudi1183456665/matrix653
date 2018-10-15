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
** 文   件   名: test_config.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: 测试用例配置
*********************************************************************************************************/
#include "../test/iunit/t_unit.h"

/*********************************************************************************************************
  测试用例
*********************************************************************************************************/
BOOL unitDemo()
{
    return  (MX_FALSE);
}

/*********************************************************************************************************
   测试参数
*********************************************************************************************************/
#define SCT_VCPUID_KERN     0
#define SCT_VCPUID_ROOT     1
#define SCT_VCPUID_SER      2
#define SCT_VCPUID_USER     3

/*********************************************************************************************************
   测试用例1: 模拟系统线程(root线程 - sigma0 + moe)
*********************************************************************************************************/
PVOID  __SCT_kernThread(PVOID  pvArg)
{
    INT  i = 0;

    SCT_TITLE("%s---__SCT_kernThread() start. %X\n", MX_VCPU_GET_CUR());

    while (1) {
        SCT_DEBUG("-- %d, %X ------ vCPU%d @%d\n", i++, archGetCpsr(), (INT)pvArg, archMpCur());
        SCT_SLEEP(30);
    }

    return  (MX_NULL);
}


PVOID  __SCT_serverThread(PVOID  pvArg)
{
    INT  i = 0;

    SCT_TITLE("%s---__SCT_rootThread() start. %X\n", MX_VCPU_GET_CUR());

    while (1) {
        SCT_DEBUG("-- %d, %X ------ vCPU%d @%d\n", i++, archGetCpsr(), (INT)pvArg, archMpCur());
        SCT_SLEEP(30);
    }

    return  (MX_NULL);
}

PVOID  __SCT_rootThread(PVOID  pvArg)
{
    INT  i = 0;

    SCT_TITLE("%s---__SCT_rootThread() start. %X\n", MX_VCPU_GET_CUR());

    while (1) {
        SCT_DEBUG("-- %d, %X ------ vCPU%d @%d\n", i++, archGetCpsr(), (INT)pvArg, archMpCur());
        SCT_SLEEP(30);
    }

    return  (MX_NULL);
}

VOID  __SCT_sysStartUnit()
{
    MX_OBJECT_ID ulVcpuId;
    MX_VCPU_ATTR vcpuattr;

    SCT_TITLE("%s---__SCT_kernThread CREATE. \n");
    vcpuattr.VCPUATTR_uiPriority = 200;
    vcpuattr.VCPUATTR_pvArg = (PVOID) SCT_VCPUID_KERN;
    vcpuattr.VCPUATTR_bIsUser = MX_FALSE;
    ulVcpuId = vcpuCreate(__SCT_kernThread, &vcpuattr);                /*  创建 内核 线程              */
    vcpuStart(ulVcpuId);

    SCT_TITLE("%s---__SCT_rootThread CREATE. \n");
    vcpuattr.VCPUATTR_uiPriority = 200;
    vcpuattr.VCPUATTR_pvArg = (PVOID) SCT_VCPUID_ROOT;
    vcpuattr.VCPUATTR_bIsUser = MX_TRUE;
    ulVcpuId = vcpuCreate(__SCT_rootThread, &vcpuattr);                /*  创建 ROOT 线程              */
    vcpuStart(ulVcpuId);
}

/*********************************************************************************************************
   测试用例2: 模拟系统线程(root线程 - sigma0 + moe)
*********************************************************************************************************/



/*********************************************************************************************************
  END
*********************************************************************************************************/
