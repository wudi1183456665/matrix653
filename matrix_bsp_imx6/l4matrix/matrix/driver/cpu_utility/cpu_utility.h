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
** ��   ��   ��: cpu_utility.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 01 �� 19 ��
**
** ��        ��: I.MX6Q ������ ��˹��������ļ�.
*********************************************************************************************************/

#ifndef __CPU_UTILITY_H
#define __CPU_UTILITY_H

/*********************************************************************************************************
  ϵͳ������غ궨��
*********************************************************************************************************/
/*
 * Number of cores available.
 */
enum _get_cores_results
{
    GET_CORES_ERROR    = 0,                                             /* Failed determine num of cores*/
    FOUR_CORES_ACTIVE  = 4,                                             /* Four available CPU cores.    */
    TWO_CORES_ACTIVE   = 2,                                             /* Two available CPU cores.     */
    ONE_CORE_ACTIVE    = 1                                              /* One available CPU core.      */
};
/*
 * List of all the available CPU work point
 */
typedef enum {
    CPU_WORKPOINT_1P2GHZ = 0,
    CPU_WORKPOINT_1GHZ   = 1,
    CPU_WORKPOINT_800MHZ = 2,
    CPU_WORKPOINT_400MHZ = 3,
    CPU_WORKPOINT_OUTOFRANGE
} cpu_wp_e;

typedef VOID (*cpu_entry_point_t)(VOID * arg);

/*********************************************************************************************************
  functions
*********************************************************************************************************/
VOID  imx6qCpuStart2nd(UINT8 ucCoreNum);
VOID  imx6qCpuDisable(UINT8 ucCoreNum);

#endif                                                                  /*  __CPU_UTILITY_H__           */
/*********************************************************************************************************
  END
*********************************************************************************************************/

