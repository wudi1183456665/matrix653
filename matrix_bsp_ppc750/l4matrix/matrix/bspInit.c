/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: bspInit.c
**
** ��   ��   ��: Li.Jing (�)
**
** �ļ���������: 2018 �� 1 �� 18 ��
**
** ��        ��: BSP �û� C �������
*********************************************************************************************************/
#include <matrix.h>
#include "config.h"                                                     /*  �������� & ���������       */
#include "driver/gic/gic.h"                                             /*  i.MX6  �жϿ�����           */
#include "driver/timer/timer.h"                                         /*  i.MX6  ��ʱ��               */
#include "driver/ccm_pll/ccm_pll.h"                                     /*  i.MX6  PLL ʱ��             */
#include "driver/cpu_utility/cpu_utility.h"                             /*  i.MX6  ��˿���             */
#include "arch/arm/common/cp15/armCp15.h"
/*********************************************************************************************************
** ��������: targetInit
** ��������: ��ʼ�� i.MX6DQ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: start.S
*********************************************************************************************************/
VOID  targetInit (VOID)
{
    imx6qCcmPLLInit();
}
/*********************************************************************************************************
** ��������: halTickInit
** ��������: ��ʼ�� tick ʱ��
** ��  ��  : NONE
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��: halPrimaryCpuMain()
*********************************************************************************************************/
static VOID  halTickInit (VOID)
{
    REGISTER UINT32      uiIncrementValue, uiPrescaler;

    /*
     *                         (PRESCALER_value+1) x (Load_value+1) x 2
     *  The timer interval = --------------------------------------------
     *                                         PERIPHCLK
     */
    uiIncrementValue = (imx6qMainClkGet(CPU_CLK) / 2 ) / BSP_CFG_TICK_HZ;
    uiPrescaler      = 0;

    /*
     *  ��ʼ�� Tick ��ʱ��Ӳ��
     */
    armGlobalTimerInit(MX_TRUE, uiIncrementValue, uiPrescaler, MX_TRUE);
    armGlobalTimerCounterSet(0);
    armGlobalTimerComparatorSet(uiIncrementValue);
    armGlobalTimerStart();

    /*
     *  ʹ�� Tick �ж�
     */
    armGicIntVecterEnable(GIC_TICK_INT_VEC, MX_FALSE, GIC_TICK_INT_PRIO, 1 << 0);
}
/*********************************************************************************************************
** ��������: halSmpCoreInit
** ��������: ��ʼ�� SMP ����
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: halPrimaryCpuMain(), halSecondaryCpuMain()
*********************************************************************************************************/
static VOID  halSmpCoreInit (VOID)
{
    ULONG  ulCpuId = archMpCur();

    irqIpiVecSet(ulCpuId, GIC_IPI_INT_VEC(ulCpuId));                    /*  ��װ IPI ����               */
}
/*********************************************************************************************************
** ��������: halPrimaryCpuMain
** ��������: Primary CPU C ���
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: startup.S
*********************************************************************************************************/
VOID  halPrimaryCpuMain (VOID)
{
    bspDebugMsg("-----------halPrimaryCpuMain(%d)------------------\n", archMpCur());

    armGicInit();
    armGicCpuInit(MX_FALSE, 255);
    armVectorBaseAddrSet(BSP_CFG_VECTOR_BASE);

    halTickInit();
    halSmpCoreInit();

    initMatrix("ncpus=4 tick=1000");                                    /*  ���� Matrix ΢�ں�          */
}
/*********************************************************************************************************
** ��������: halSecondaryCpuMain
** ��������: Secondary CPU C ���
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��: startup.S
*********************************************************************************************************/
VOID  halSecondaryCpuMain (VOID)
{
    //bspDebugMsg("-----------halSecondaryCpuMain(%d)----------------\n", archMpCur());

    armGicCpuInit(MX_FALSE, 255);
    armVectorBaseAddrSet(BSP_CFG_VECTOR_BASE);

    halSmpCoreInit();
    
    initSmpMatrix();                                                    /*  ΢�ں��������              */
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
