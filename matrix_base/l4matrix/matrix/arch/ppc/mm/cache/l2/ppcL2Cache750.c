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
** ��   ��   ��: ppcL2Cache750.c
**
** ��   ��   ��: Yang.HaiFeng (���)
**
** �ļ���������: 2016 �� 03 �� 08 ��
**
** ��        ��: MPC750 ��ϵ���� L2 CACHE ����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include <Matrix.h>
/*********************************************************************************************************
  �ü�����
*********************************************************************************************************/
//#include "../ppcL2.h"
#include "ppcL2Cache750.h"
/*********************************************************************************************************
  �ⲿ�ӿ�����
*********************************************************************************************************/
extern VOID  ppc750L2CacheInit(UINT32  uiL2CR);
extern VOID  ppc750L2CacheEnable(VOID);
extern VOID  ppc750L2CacheDisable(VOID);
extern BOOL  ppc750L2CacheIsEnable(VOID);
extern VOID  ppc750L2CacheInvalidateAll(VOID);
extern VOID  ppc750L2CacheFlushAllSW(size_t  stSize, UINT8  *pucReadBuffer);
extern VOID  ppc750L2CacheFlushAllHW(VOID);

extern VOID  ppc745xL2CacheInit(UINT32  uiL2CR);
extern VOID  ppc745xL2CacheEnable(VOID);
extern VOID  ppc745xL2CacheDisable(VOID);
extern BOOL  ppc745xL2CacheIsEnable(VOID);
extern VOID  ppc745xL2CacheInvalidateAll(VOID);
extern VOID  ppc745xL2CacheFlushAllSW(size_t  stSize, UINT8  *pucReadBuffer);
extern VOID  ppc745xL2CacheFlushAllHW(VOID);
/*********************************************************************************************************
  Pointer of a page-aligned cacheable region to use as a flush buffer.
*********************************************************************************************************/
static UINT8                   *_G_pucL2CacheReadBuffer;
/*********************************************************************************************************
  L2 CACHE ����(Ĭ�ϲ����� L2 CACHE������ֵΪ����ʾ��)
*********************************************************************************************************/
static PPC750_L2CACHE_CONFIG    _G_l2Config = {
        .CFG_bPresent   = LW_FALSE,
        .CFG_stSize     = L2RAM_SIZE_1M,
        .CFG_uiL2CR     = L2CR_SIZE_1MB |
                          L2CR_RAM_PB2  |
                          L2CR_OH_0_5ns,
};
/*********************************************************************************************************
** ��������: ppcL2Cache750Config
** ��������: L2 CACHE ����
** �䡡��  : l2Config          L2 CACHE ����
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  ppcL2Cache750Config (PPC750_L2CACHE_CONFIG  *pL2Config)
{
    if (pL2Config) {
        _G_l2Config = *pL2Config;
    }
}
/*********************************************************************************************************
** ��������: __ppc750L2CacheFlushAllSW
** ��������: L2 CACHE �����д����Ч
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static VOID  __ppc750L2CacheFlushAllSW (VOID)
{
    ppc750L2CacheFlushAllSW((_G_l2Config.CFG_stSize * 2) / L1CACHE_ALIGN_SIZE,
                            _G_pucL2CacheReadBuffer);
}
/*********************************************************************************************************
** ��������: ppcL2Cache750Init
** ��������: ��ʼ�� L2 CACHE ������
** �䡡��  : pl2cdrv            �����ṹ
**           uiInstruction      ָ�� CACHE ����
**           uiData             ���� CACHE ����
**           pcMachineName      ��������
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
                                                            /*  LW_CFG_CACHE_EN > 0         */
                                                                        /*  LW_CFG_PPC_CACHE_L2 > 0     */
/*********************************************************************************************************
  END
*********************************************************************************************************/
