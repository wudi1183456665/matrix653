/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: mem.h
**
** ��   ��   ��: Cheng.Yongbin (������)
**
** �ļ���������: 2018 �� 2 �� 1 ��
**
** ��        ��: mem
*********************************************************************************************************/
#ifndef __T_MEM_H
#define __T_MEM_H
/*********************************************************************************************************
  �ڴ�����
*********************************************************************************************************/
#define     TMEM_IPC_MSG    1
#define     TMEM_USE_STK    2

#define     TMEM_VCPU       3
#define     TMEM_SERVICE    4

PVOID   T_memObjAlloc(UINT32 uiType);

/*********************************************************************************************************
  �ж϶�ַ�Ƿ����
*********************************************************************************************************/
#define  T_MEM_ALIGN(addr, uiPower)             \
        (((addr) % (1 << (uiPower))) == 0)

#endif                                                                  /*  __T_MEM_H                   */
/*********************************************************************************************************
  END
*********************************************************************************************************/