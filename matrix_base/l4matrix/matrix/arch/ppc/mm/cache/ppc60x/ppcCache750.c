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
** ��   ��   ��: ppcCache750.c
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2016 �� 03 �� 30 ��
**
** ��        ��: PowerPC MPC750 ��ϵ���� CACHE ����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include <Matrix.h>
/*********************************************************************************************************
  �ü�֧��
*********************************************************************************************************/
//#include "../common/ppcCache.h"
/*********************************************************************************************************
  �ⲿ�ӿ�����
*********************************************************************************************************/
extern VOID     ppc750DCacheDisable(VOID);
extern VOID     ppc750DCacheEnable(VOID);
extern VOID     ppc750ICacheDisable(VOID);
extern VOID     ppc750ICacheEnable(VOID);

extern VOID     ppc750DCacheClearAll(VOID);
extern VOID     ppc750DCacheFlushAll(VOID);
extern VOID     ppc750ICacheInvalidateAll(VOID);

extern VOID     ppc750DCacheClear(PVOID  pvStart, PVOID  pvEnd, UINT32  uiStep);
extern VOID     ppc750DCacheFlush(PVOID  pvStart, PVOID  pvEnd, UINT32  uiStep);
extern VOID     ppc750DCacheInvalidate(PVOID  pvStart, PVOID  pvEnd, UINT32  uiStep);
extern VOID     ppc750ICacheInvalidate(PVOID  pvStart, PVOID  pvEnd, UINT32  uiStep);

extern VOID     ppc750BranchPredictionDisable(VOID);
extern VOID     ppc750BranchPredictionEnable(VOID);
extern VOID     ppc750BranchPredictorInvalidate(VOID);

extern VOID     ppc750TextUpdate(PVOID  pvStart, PVOID  pvEnd,
                                  UINT32  uiICacheLineSize, UINT32  uiDCacheLineSize);
/*********************************************************************************************************
** ��������: ppc750CacheProbe
** ��������: CACHE ̽��
** �䡡��  : pcMachineName         ������
**           pICache               I-Cache ��Ϣ
**           pDCache               D-Cache ��Ϣ
** �䡡��  : ERROR or OK
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/

/*********************************************************************************************************
  MPC750 CACHE ����
*********************************************************************************************************/

/*********************************************************************************************************
  END
*********************************************************************************************************/
