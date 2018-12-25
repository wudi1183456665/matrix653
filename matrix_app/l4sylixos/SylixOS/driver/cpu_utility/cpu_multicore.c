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
** ��   ��   ��: cpu_multicore.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2015 �� 01 �� 21 ��
**
** ��        ��: imx6q ������������ô���
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "sdk.h"
#include <assert.h>
#include "cpu_utility/cpu_utility.h"
#include "regssrc.h"
/*********************************************************************************************************
  ����
*********************************************************************************************************/
#define MAX_CORE_COUNT (4)                                              /*  ������ CPU �˵���Ŀ         */
/*********************************************************************************************************
** ��������: imx6qCpuStart2nd
** ��������: Start up a secondary CPU core.
** �䡡��  : ucCoreNum CPU index from 1 through 3
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  imx6qCpuStart2nd (UINT8 ucCoreNum)
{
    /*
     * Exit if the requested core is not available.
     */
    if (ucCoreNum == 0 || ucCoreNum >= MAX_CORE_COUNT) {
        return;
    }
    /*
     * Prepare pointers for ROM code. The entry point is always _start, which does some
     * basic preparatory work and then calls the common_cpu_entry function, which itself
     * calls the entry point saved in s_core_info.
     */
    switch (ucCoreNum) {
    case 1:
        HW_SRC_GPR3_WR((UINT32) bspInfoRamBase());
        HW_SRC_GPR4_WR((UINT32) 0);

        if (HW_SRC_SCR.B.CORE1_ENABLE == 1) {
            HW_SRC_SCR.B.CORE1_RST = 1;
        } else {
            HW_SRC_SCR.B.CORE1_ENABLE = 1;
        }
        break;

    case 2:
        HW_SRC_GPR5_WR((UINT32) bspInfoRamBase());
        HW_SRC_GPR6_WR((UINT32) 0);

        if (HW_SRC_SCR.B.CORE2_ENABLE == 1) {
            HW_SRC_SCR.B.CORE2_RST = 1;
        } else {
            HW_SRC_SCR.B.CORE2_ENABLE = 1;
        }
        break;

    case 3:
        HW_SRC_GPR7_WR((UINT32) bspInfoRamBase());
        HW_SRC_GPR8_WR((UINT32) 0);

        if (HW_SRC_SCR.B.CORE3_ENABLE == 1) {
            HW_SRC_SCR.B.CORE3_RST = 1;
        } else {
            HW_SRC_SCR.B.CORE3_ENABLE = 1;
        }
        break;
    }
}
/*********************************************************************************************************
** ��������: imx6qCpuDisable
** ��������: Places a secondary CPU core in reset.
** �䡡��  : ucCoreNum CPU index from 1 through 3
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  imx6qCpuDisable (UINT8 ucCoreNum)
{
    /*
     * Exit if the requested core is not available.
     */
    if (ucCoreNum == 0 || ucCoreNum >= MAX_CORE_COUNT) {
        return;
    }

    switch (ucCoreNum) {
    case 1:
        HW_SRC_SCR.B.CORE1_ENABLE = 0;
        break;

    case 2:
        HW_SRC_SCR.B.CORE2_ENABLE = 0;
        break;

    case 3:
        HW_SRC_SCR.B.CORE3_ENABLE = 0;
        break;
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
