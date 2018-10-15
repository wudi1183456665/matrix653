;/*********************************************************************************************************
;**
;**                                    中国软件开源组织
;**
;**                                   嵌入式实时操作系统
;**
;**                                       SylixOS(TM)
;**
;**                               Copyright  All Rights Reserved
;**
;**--------------文件信息--------------------------------------------------------------------------------
;**
;** 文   件   名: ppcCache750Asm.S
;**
;** 创   建   人: Jiao.JinXing (焦进星)
;**
;** 文件创建日期: 2016 年 03 月 30 日
;**
;** 描        述: PowerPC MPC750 体系构架 CACHE 驱动.
;*********************************************************************************************************/

#define __SYLIXOS_PPC603__                  0x603
#define __SYLIXOS_PPCEC603__                0xEC603
#define __SYLIXOS_PPC604__                  0x604

#define __SYLIXOS_PPC_CPU__                 __SYLIXOS_PPC604__           /*  MPC745X 是 604 的变种       */

#define __SYLIXOS_PPC750__
#define __SYLIXOS_PPC_HAVE_ALTIVEC          1

/*********************************************************************************************************
  重新定义接口名
*********************************************************************************************************/

#define ppc60xICacheEnable                  ppc750ICacheEnable
#define ppc60xDCacheEnable                  ppc750DCacheEnable
#define ppc60xICacheDisable                 ppc750ICacheDisable
#define ppc60xDCacheDisable                 ppc750DCacheDisable
#define ppc60xICacheInvalidate              ppc750ICacheInvalidate
#define ppc60xDCacheInvalidate              ppc750DCacheInvalidate
#define ppc60xDCacheClear                   ppc750DCacheClear
#define ppc60xDCacheFlush                   ppc750DCacheFlush
#define ppc60xICacheInvalidateAll           ppc750ICacheInvalidateAll
#define ppc60xDCacheInvalidateAll           ppc750DCacheInvalidateAll
#define ppc60xDCacheFlushAll                ppc750DCacheFlushAll
#define ppc60xDCacheClearAll                ppc750DCacheClearAll
#define ppc60xBranchPredictionDisable       ppc750BranchPredictionDisable
#define ppc60xBranchPredictionEnable        ppc750BranchPredictionEnable
#define ppc60xBranchPredictorInvalidate     ppc750BranchPredictorInvalidate
#define ppc60xTextUpdate                    ppc750TextUpdate

/*********************************************************************************************************
  包含 60X CACHE 驱动
*********************************************************************************************************/

#include "ppcCache60xAsm.S"

;/*********************************************************************************************************
;  END
;*********************************************************************************************************/
