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
** 文   件   名: bootArmScu.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 02 月 07 日
**
** 描        述: ARM SNOOP CONTROL UNIT.
*********************************************************************************************************/
#include <Matrix.h>
#include "bootScu.h"
/*********************************************************************************************************
  ARM 外部私有设备地址
*********************************************************************************************************/
extern addr_t  bootArmPrivatePeriphBaseGet(VOID);
/*********************************************************************************************************
  Snoop Control Unit 寄存器偏移
*********************************************************************************************************/
#define SCU_REGS_BASE_OFFSET                       0    /*  Snoop Control Unit 寄存器偏移               */
/*********************************************************************************************************
  Snoop Control Unit 寄存器
*********************************************************************************************************/
typedef struct {
    volatile UINT32     SCU_uiControl;                  /*  SCU Control Register.                       */
    volatile UINT32     SCU_uiConfigure;                /*  SCU Configuration Register.                 */
    volatile UINT32     SCU_uiCpuPowerStatus;           /*  SCU CPU Power Status Register.              */
    volatile UINT32     SCU_uiInvalidateAll;            /*  SCU Invalidate All Registers in Secure State*/
    volatile UINT32     SCU_uiReserves1[12];            /*  Reserves.                                   */
    volatile UINT32     SCU_uiFilteringStart;           /*  Filtering Start Address Register.           */
    volatile UINT32     SCU_uiFilteringEnd;             /*  Filtering End Address Register.             */
    volatile UINT32     SCU_uiReserves2[2];
    volatile UINT32     SCU_uiSAC;                      /*  SCU Access Control (SAC) Register.          */
    volatile UINT32     SCU_uiSNSAC;                    /*  SCU Non-secure Access Control (SNSAC) Reg.  */
} SCU_REGS;
/*********************************************************************************************************
** 函数名称: bootArmScuGet
** 功能描述: 获得 SNOOP CONTROL UNIT
** 输　入  : NONE
** 输　出  : SNOOP CONTROL UNIT 基址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_BTXT_SEC  SCU_REGS  *bootArmScuGet (VOID)
{
    REGISTER addr_t  ulBase = bootArmPrivatePeriphBaseGet() + SCU_REGS_BASE_OFFSET;

    return  ((SCU_REGS *)ulBase);
}
/*********************************************************************************************************
** 函数名称: bootArmScuFeatureEnable
** 功能描述: SCU 特性使能
** 输　入  : uiFeatures        特性
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmScuFeatureEnable (UINT32  uiFeatures)
{
    REGISTER SCU_REGS  *pScu = bootArmScuGet();

    pScu->SCU_uiControl = pScu->SCU_uiControl | uiFeatures;
}
/*********************************************************************************************************
** 函数名称: bootArmScuFeatureDisable
** 功能描述: SCU 特性禁能
** 输　入  : uiFeatures        特性
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmScuFeatureDisable (UINT32  uiFeatures)
{
    REGISTER SCU_REGS  *pScu = bootArmScuGet();

    pScu->SCU_uiControl = pScu->SCU_uiControl & (~uiFeatures);
}
/*********************************************************************************************************
** 函数名称: bootArmScuSecureInvalidateAll
** 功能描述: Invalidates the SCU tag RAMs on a per Cortex-A9 processor and per way basis
** 输　入  : uiCPUId       CPU ID
**           uiWays        Specifies the ways that must be invalidated for CPU(ID)
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmScuSecureInvalidateAll (UINT32  uiCpuId,  UINT32  uiWays)
{
    REGISTER SCU_REGS  *pScu = bootArmScuGet();

    switch (uiCpuId) {

    case 0:
    case 1:
    case 2:
    case 3:
        uiWays &= 0xF;
        pScu->SCU_uiInvalidateAll = (uiWays << (uiCpuId * 4));
        break;

    default:
        break;
    }
}
/*********************************************************************************************************
** 函数名称: bootArmScuAccessCtrlSet
** 功能描述: 设置 SCU 访问控制
** 输　入  : uiCpuBits         CPU 位集
** 输　出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmScuAccessCtrlSet (UINT32  uiCpuBits)
{
    REGISTER SCU_REGS  *pScu = bootArmScuGet();

    pScu->SCU_uiSAC = uiCpuBits;
}
/*********************************************************************************************************
  END
*********************************************************************************************************/


