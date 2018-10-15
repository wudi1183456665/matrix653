/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: ppcCache750.c
**
** 创   建   人: Jiao.JinXing (焦进星)
**
** 文件创建日期: 2016 年 03 月 30 日
**
** 描        述: PowerPC MPC750 体系构架 CACHE 驱动.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include <Matrix.h>
/*********************************************************************************************************
  裁剪支持
*********************************************************************************************************/
//#include "../common/ppcCache.h"
/*********************************************************************************************************
  外部接口声明
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
** 函数名称: ppc750CacheProbe
** 功能描述: CACHE 探测
** 输　入  : pcMachineName         机器名
**           pICache               I-Cache 信息
**           pDCache               D-Cache 信息
** 输　出  : ERROR or OK
** 全局变量:
** 调用模块:
*********************************************************************************************************/

/*********************************************************************************************************
  MPC750 CACHE 驱动
*********************************************************************************************************/

/*********************************************************************************************************
  END
*********************************************************************************************************/
