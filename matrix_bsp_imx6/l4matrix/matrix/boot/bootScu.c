/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: bootArmScu.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 02 �� 07 ��
**
** ��        ��: ARM SNOOP CONTROL UNIT.
*********************************************************************************************************/
#include <Matrix.h>
#include "bootScu.h"
/*********************************************************************************************************
  ARM �ⲿ˽���豸��ַ
*********************************************************************************************************/
extern addr_t  bootArmPrivatePeriphBaseGet(VOID);
/*********************************************************************************************************
  Snoop Control Unit �Ĵ���ƫ��
*********************************************************************************************************/
#define SCU_REGS_BASE_OFFSET                       0    /*  Snoop Control Unit �Ĵ���ƫ��               */
/*********************************************************************************************************
  Snoop Control Unit �Ĵ���
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
** ��������: bootArmScuGet
** ��������: ��� SNOOP CONTROL UNIT
** �䡡��  : NONE
** �䡡��  : SNOOP CONTROL UNIT ��ַ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_BTXT_SEC  SCU_REGS  *bootArmScuGet (VOID)
{
    REGISTER addr_t  ulBase = bootArmPrivatePeriphBaseGet() + SCU_REGS_BASE_OFFSET;

    return  ((SCU_REGS *)ulBase);
}
/*********************************************************************************************************
** ��������: bootArmScuFeatureEnable
** ��������: SCU ����ʹ��
** �䡡��  : uiFeatures        ����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmScuFeatureEnable (UINT32  uiFeatures)
{
    REGISTER SCU_REGS  *pScu = bootArmScuGet();

    pScu->SCU_uiControl = pScu->SCU_uiControl | uiFeatures;
}
/*********************************************************************************************************
** ��������: bootArmScuFeatureDisable
** ��������: SCU ���Խ���
** �䡡��  : uiFeatures        ����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmScuFeatureDisable (UINT32  uiFeatures)
{
    REGISTER SCU_REGS  *pScu = bootArmScuGet();

    pScu->SCU_uiControl = pScu->SCU_uiControl & (~uiFeatures);
}
/*********************************************************************************************************
** ��������: bootArmScuSecureInvalidateAll
** ��������: Invalidates the SCU tag RAMs on a per Cortex-A9 processor and per way basis
** �䡡��  : uiCPUId       CPU ID
**           uiWays        Specifies the ways that must be invalidated for CPU(ID)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
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
** ��������: bootArmScuAccessCtrlSet
** ��������: ���� SCU ���ʿ���
** �䡡��  : uiCpuBits         CPU λ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
MX_BTXT_SEC  VOID  bootArmScuAccessCtrlSet (UINT32  uiCpuBits)
{
    REGISTER SCU_REGS  *pScu = bootArmScuGet();

    pScu->SCU_uiSAC = uiCpuBits;
}
/*********************************************************************************************************
  END
*********************************************************************************************************/


