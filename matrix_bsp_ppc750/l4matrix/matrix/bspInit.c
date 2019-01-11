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
#include "driver/int/i8259a.h"											/*	�жϿ�����					*/
#include "driver/timer/ppcTimer.h"										/*	��ʱ��						*/
#include "driver/uart/uart_1.h"											/*	����						*/

//#include "driver/gic/gic.h"                                             /*  i.MX6  �жϿ�����           */
//#include "driver/timer/timer.h"                                         /*  i.MX6  ��ʱ��               */
//#include "driver/ccm_pll/ccm_pll.h"                                     /*  i.MX6  PLL ʱ��             */
//#include "driver/cpu_utility/cpu_utility.h"                             /*  i.MX6  ��˿���             */
//#include "arch/arm/common/cp15/armCp15.h"

static PPC_TIME _G_i8254Data = {
    .iobase  = BSP_CFG_8254_IO_BASE,
    .qcofreq = 1193182,
};
static UINT32   _G_uiFullCnt;
static UINT64   _G_ui64NSecPerCnt7;                                     /*  ��� 7bit ����              */


static I8259A_CTL   _G_i8259aData = {
        .iobase_master  = BSP_CFG_8259A_IO_BASE_MASTER,
        .iobase_slave   = BSP_CFG_8259A_IO_BASE_SLAVE,
        .trigger        = 0,
        .vector_base    = 0,
};

/*********************************************************************************************************
** ��������: bspTickInit
** ��������: ��ʼ�� tick ʱ��
** ��  ��  : NONE
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID	halTickInit	(VOID)
{
    ULONG                ulVector = BSP_CFG_8254_VECTOR;
    _G_uiFullCnt          = (_G_i8254Data.qcofreq / 100);
    _G_ui64NSecPerCnt7    = ((1000 * 1000 * 1000 / 100) << 7) / _G_uiFullCnt;

    i8254InitAsTick(&_G_i8254Data);

}


INT bspInit (VOID)
{
	bspDebugMsg("-----------------bspInit(%d)--------------------\n");

	i8259aInit(&_G_i8259aData);
    extern VOID  vector(VOID);
//    lib_memcpy((PVOID)BSP_CFG_RAM_BASE, (CPVOID)vector, 0x2000);


}


/*********************************************************************************************************
  END
*********************************************************************************************************/
