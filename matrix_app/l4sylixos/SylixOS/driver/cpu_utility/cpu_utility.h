/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: cpu_utility.h
**
** ��   ��   ��: Zhang.Xu (����)
**
** �ļ���������: 2015 �� 12 �� 1 ��
**
** ��        ��: I.MX6Q ������ ��˹��������ļ�
*********************************************************************************************************/
#ifndef __CPU_UTILITY_H__
#define __CPU_UTILITY_H__

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