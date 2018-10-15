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
** 文   件   名: kobj.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 19 日
**
** 描        述: 内核对象管理.

** BUG
2018.01.30  新增对象检查函数 kobjCheck; TODO: 完善 _KOBJ_IPC_MSG 类型检查
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** 函数名称: kobjInit
** 功能描述: 内核对象模块初始化
** 输　入  : NONE
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  kobjInit (VOID)
{
}
/*********************************************************************************************************
** 函数名称: __kobjGetClass
** 功能描述: 获取内核对象类型
** 输　入  : ulKobjId           内核对象 ID
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static UINT8  __kobjGetClass (MX_OBJECT_ID  ulKobjId)
{
    return  ((UINT8)((ulKobjId >> 16) & 0xFF));
}
/*********************************************************************************************************
** 函数名称: __kobjGetIndex
** 功能描述: 获取内核对象类型
** 输　入  : ulKobjId           内核对象 ID
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
static UINT16  __kobjGetIndex (MX_OBJECT_ID  ulKobjId)
{
    return  ((UINT16)(ulKobjId & 0xFFFF));
}
/*********************************************************************************************************
** 函数名称: kobjAlloc
** 功能描述: 申请一个内核对象
** 输　入  : ucClass            内核对象类型
** 输　出  : 内核对象 ID
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
MX_OBJECT_ID  kobjAlloc (UINT8  ucClass)
{
    MX_OBJECT_ID      ulReturnId = MX_OBJECT_INVALID;
    UINT32            i = 0;

    switch (ucClass) {

    case _KOBJ_VCPU:
        for (i = 0; i < MX_CFG_MAX_VCPUS; i++) {                        /*  遍历查找未使用元素          */
            if (!_K_vcpuTable[i].VCPUKSTACK_vcpu.VCPU_bIsUsed) {
                _K_vcpuTable[i].VCPUKSTACK_vcpu.VCPU_bIsUsed = MX_TRUE;
                ulReturnId = (ucClass << 16) | i;
                break;
            }
        }
        break;

    case _KOBJ_IPC_MSG:
        break;

    case _KOBJ_PRTN:
        for (i = 0; i < MX_CFG_MAX_PRTNS; i++) {                        /*  遍历查找未使用元素          */
            if (!_K_prtnTable[i].PRTN_bIsUsed) {
                _K_prtnTable[i].PRTN_bIsUsed = MX_TRUE;
                ulReturnId = (ucClass << 16) | i;
                break;
            }
        }
        break;

    default:
        break;
    }

    return  (ulReturnId);
}
/*********************************************************************************************************
** 函数名称: kobjFree
** 功能描述: 释放一个内核对象
** 输　入  : ulKobjId           内核对象 ID
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  kobjFree (MX_OBJECT_ID  ulKobjId)
{
    UINT8      ucClass = __kobjGetClass(ulKobjId);
    UINT32     uiIndex = __kobjGetIndex(ulKobjId);

    switch (ucClass) {

    case _KOBJ_VCPU:
        if (uiIndex < MX_CFG_MAX_VCPUS) {
            _K_vcpuTable[uiIndex].VCPUKSTACK_vcpu.VCPU_bIsUsed = MX_FALSE;
        }
        break;

    case _KOBJ_IPC_MSG:
        break;

    case _KOBJ_PRTN:
        if (uiIndex < MX_CFG_MAX_PRTNS) {
            _K_prtnTable[uiIndex].PRTN_bIsUsed = MX_FALSE;
        }
        break;

    default:
        break;
    }

}
/*********************************************************************************************************
** 函数名称: kobjGet
** 功能描述: 获取一个内核对象结构体指针
** 输　入  : ulKobjId           内核对象 ID
** 输　出  : 内核对象结构体指针
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PVOID  kobjGet (MX_OBJECT_ID  ulKobjId)
{
    UINT8      ucClass = __kobjGetClass(ulKobjId);
    UINT32     uiIndex = __kobjGetIndex(ulKobjId);
    VOID      *pvKobj  = MX_NULL;

    switch (ucClass) {

    case _KOBJ_VCPU:
        if (uiIndex < MX_CFG_MAX_VCPUS) {
            pvKobj = &_K_vcpuTable[uiIndex];
        }
        break;

    case _KOBJ_IPC_MSG:
        break;

    case _KOBJ_PRTN:
        if (uiIndex < MX_CFG_MAX_PRTNS) {
            pvKobj = &_K_prtnTable[uiIndex];
        }
        break;

    default:
        break;
    }

    return  (pvKobj);
}
/*********************************************************************************************************
** 函数名称: kobjCheck
** 功能描述: 内核对象合法性检查
** 输　入  : ulKobjId   对象
** 输　出  : ucObjClass 对象所属类型
** 全局变量: FALSE - 对象格式错误
** 调用模块:
*********************************************************************************************************/
BOOL  kobjCheck (MX_OBJECT_ID ulKobjId, UINT8 ucObjClass)
{
    UINT8      ucClass = __kobjGetClass(ulKobjId);
    UINT32     uiIndex = __kobjGetIndex(ulKobjId);

    if (ucObjClass != ucClass) {
        return  (MX_FALSE);
    }

    switch (ucClass) {
    case _KOBJ_VCPU:
        if (uiIndex >= MX_CFG_MAX_VCPUS) {
            return  (MX_FALSE);
        }
        break;

    case _KOBJ_IPC_MSG:
        break;

    case _KOBJ_PRTN:
        if (uiIndex >= MX_CFG_MAX_PRTNS) {
            return  (MX_FALSE);
        }
        break;

    default:
        return  (MX_FALSE);
    }
    return  (MX_TRUE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
