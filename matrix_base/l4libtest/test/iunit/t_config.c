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
** 文   件   名: t_config.c
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: 测试用例配置
*********************************************************************************************************/
#include "../test/iunit/t_unit.h"

/*********************************************************************************************************
  测试用例组选项
*********************************************************************************************************/

//#define T_OMIT_UNIT_DEMO
#define T_OMIT_UNIT_XXX


#ifndef T_OMIT_UNIT_DEMO
/*********************************************************************************************************
  demo 测试用例
*********************************************************************************************************/
UINT32           TG_uiUnits = 1;                                        /*  测试用例数量                */
TEST_UNIT        TG_TEST_UNIT[] = {                                     /*  测试用例组                  */

        {unitDemo,      "demo_unit"},
};
#endif

#ifndef T_OMIT_UNIT_XXX
/*********************************************************************************************************
  xx 测试用例
*********************************************************************************************************/
UINT32           TG_uiUnits = 0;                                        /*  测试用例数量                */
FUNC_TEST_UNIT   TG_TEST_UNIT[] = {                                     /*  测试用例组                  */

};
#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/
