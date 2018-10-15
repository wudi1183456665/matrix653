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
** 文   件   名: elf.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 11 月 01 日
**
** 描        述: elf 解析.
*********************************************************************************************************/
#include <loader.h>
#include "elf.h"
#include "lib_memory.h"
/*********************************************************************************************************
  KIP段类型
*********************************************************************************************************/
#define PT_KIP      0x10
/*********************************************************************************************************
** 函数名称: elfCheck
** 功能描述: 检查 elf 文件头有效性
** 输　入  : pElfHdr    elf 文件头指针
** 输　出  : ERROR_NONE 成功, 其它值 失败
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __elfCheck (Elf_Ehdr  *pElfHdr)
{
    if ((pElfHdr->e_ident[EI_MAG0] != ELFMAG0) ||                       /*  检查 ELF 魔数               */
        (pElfHdr->e_ident[EI_MAG1] != ELFMAG1) ||
        (pElfHdr->e_ident[EI_MAG2] != ELFMAG2) ||
        (pElfHdr->e_ident[EI_MAG3] != ELFMAG3)) {
        return  (ERROR_ELF_FORMAT);
    }

    if (pElfHdr->e_machine != ELF_ARCH) {                               /*  检查 ELF 体系结构是否匹配   */
        return  (ERROR_ELF_FORMAT);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: elfLoad
** 功能描述: 加载 ELF 文件, 获取入口地址, 返回KIP对象地址
** 输　入  : ulElfAddr      elf 文件在内存中地址
**           pulEntry       输出 elf 文件的入口地址
**           pulKip         输出 KIP 结构体对象地址
** 输　出  : ERROR_NONE 成功, 其它值 失败
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
INT  elfLoad (addr_t  ulElfAddr, addr_t  *pulEntry, addr_t  *pulKip)
{
    Elf_Ehdr    *pElfHdr = (Elf_Ehdr *)ulElfAddr;                       /*  elf 文件头                  */
    Elf_Phdr    *pProgHdr;                                              /*  Program 段头                */
    INT          iRes    = ERROR_NONE;
    INT          i;

    if ((iRes = __elfCheck(pElfHdr)) != ERROR_NONE) {                   /*  检查 elf 文件头             */
        return  (iRes);
    }

    if (pulEntry != LW_NULL) {
        *pulEntry = (addr_t)pElfHdr->e_entry;                           /*  获取入口函数                */
    }

    pProgHdr = (Elf_Phdr *)((addr_t)pElfHdr + pElfHdr->e_phoff);        /*  定位到 prog_hdr_table 首项  */

    /*
     *  遍历 program_header_table 所有段
     */
    for (i = 0; i < pElfHdr->e_phnum; i++) {
        if ((pProgHdr->p_type == PT_KIP) && (pulKip != LW_NULL)) {
            *pulKip = (addr_t)pProgHdr->p_paddr;                        /*  获取 KIP 地址               */

        } else if (pProgHdr->p_type == PT_LOAD) {
            /*
             *  对 PT_LOAD 属性的段进行拷贝
             */
            addr_t ulLoadAddr  = pProgHdr->p_paddr;                     /*  段加载地址                  */
            addr_t ulImageAddr = ulElfAddr + pProgHdr->p_offset;        /*  段内存镜像地址              */
            size_t uiLoadSize  = pProgHdr->p_filesz;                    /*  段加载大小                  */
            size_t uiMemSize   = pProgHdr->p_memsz;                     /*  段内存占用大小              */

            if ((iRes = regionRequest(ulLoadAddr, uiMemSize)) != ERROR_NONE) {
                                                                        /*  请求指定的内存,检查其合法性 */
                return  (iRes);
            }

            lib_memcpy((PVOID)ulLoadAddr, (CPVOID)ulImageAddr, uiLoadSize);
                                                                        /*  复制段数据到其加载地址      */
            if (uiMemSize > uiLoadSize) {
                lib_memset((PVOID)(ulLoadAddr + uiLoadSize),            /*  填充剩余内存                */
                           0, uiMemSize - uiLoadSize);
            }
        }

        pProgHdr = (Elf_Phdr *)((addr_t)pProgHdr + pElfHdr->e_phentsize);
    }

    return  (iRes);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
