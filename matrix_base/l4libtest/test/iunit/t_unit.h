/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                嵌入式 L4 微内核操作系统
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: t_unit.h
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: 应用运行时环境抽象
*********************************************************************************************************/
#ifndef __T_UNIT_H
#define __T_UNIT_H

#include <Matrix.h>
#include "../test/include/tool/debug.h"
#include "../test/include/core/core.h"
#include "../test/include/service/service.h"

/*
 * 测试用例
 */
typedef BOOL(*FUNC_TEST_UNIT)();

typedef struct {
    FUNC_TEST_UNIT  funcUnit;
    PCHAR           strUnitName;
} TEST_UNIT;

extern UINT32           TG_uiUnits;         /*  测试用例数量 */
extern TEST_UNIT        TG_TEST_UNIT[];     /*  测试用例     */

#endif                                                                  /*  __T_UNIT_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
