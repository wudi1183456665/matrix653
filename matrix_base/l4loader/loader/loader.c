/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: loader.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 10 月 31 日
**
** 描        述: loader .
*********************************************************************************************************/
#include <loader.h>
#include "./elf/elf.h"
/*********************************************************************************************************
  声明链接脚本中的变量
*********************************************************************************************************/
extern UINT8 _matrix_elf_start;                                         /*  微内核镜像起始位置          */
/*********************************************************************************************************
  matrix 入口函数类型定义
*********************************************************************************************************/
typedef void (* ENTRY_FUNC)(void);
/*********************************************************************************************************
** 函数名称: loaderStart
** 功能描述: 加载各个模块，并启动 matrix 微内核
** 输　入  : pcBootCmdLine     参数字符串
**           prgMemory         内存描述区指针
**           uiRgNum           内存描述区个数
** 输　出  : 
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  loaderStart (CPCHAR  pcBootCmdLine, PREGION  prgMemory, UINT32  uiRgNum)
{
    addr_t   ulMatrixAddr   = (addr_t)(&_matrix_elf_start);           /*  matrix elf 起始地址           */
    addr_t   ulMatrixEntry  = 0;                                      /*  matrix 入口地址               */
    addr_t   ulKip          = 0;                                      /*  KIP 地址                      */

    if ((prgMemory == LW_NULL) || (uiRgNum < 2)) {                    /*  至少有 ROM, RAM 2 个内存块    */
        return;
    }

    if (regionInit(prgMemory, uiRgNum) != ERROR_NONE) {               /*  内存块描述数据初始化          */
        return;
    }

    if (elfLoad(ulMatrixAddr, &ulMatrixEntry, &ulKip) != ERROR_NONE) {/*  elf 地址解析, 段加载          */
        return;
    }

    if (regionWrite((PKIP)ulKip) != ERROR_NONE) {                     /*  内存块描述数据保存到 KIP      */
        return;
    }

    ((ENTRY_FUNC)ulMatrixEntry)();                                    /*  运行 l4matrix                 */
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
