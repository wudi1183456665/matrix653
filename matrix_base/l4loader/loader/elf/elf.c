/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: elf.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 11 �� 01 ��
**
** ��        ��: elf ����.
*********************************************************************************************************/
#include <loader.h>
#include "elf.h"
#include "lib_memory.h"
/*********************************************************************************************************
  KIP������
*********************************************************************************************************/
#define PT_KIP      0x10
/*********************************************************************************************************
** ��������: elfCheck
** ��������: ��� elf �ļ�ͷ��Ч��
** �䡡��  : pElfHdr    elf �ļ�ͷָ��
** �䡡��  : ERROR_NONE �ɹ�, ����ֵ ʧ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __elfCheck (Elf_Ehdr  *pElfHdr)
{
    if ((pElfHdr->e_ident[EI_MAG0] != ELFMAG0) ||                       /*  ��� ELF ħ��               */
        (pElfHdr->e_ident[EI_MAG1] != ELFMAG1) ||
        (pElfHdr->e_ident[EI_MAG2] != ELFMAG2) ||
        (pElfHdr->e_ident[EI_MAG3] != ELFMAG3)) {
        return  (ERROR_ELF_FORMAT);
    }

    if (pElfHdr->e_machine != ELF_ARCH) {                               /*  ��� ELF ��ϵ�ṹ�Ƿ�ƥ��   */
        return  (ERROR_ELF_FORMAT);
    }

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** ��������: elfLoad
** ��������: ���� ELF �ļ�, ��ȡ��ڵ�ַ, ����KIP�����ַ
** �䡡��  : ulElfAddr      elf �ļ����ڴ��е�ַ
**           pulEntry       ��� elf �ļ�����ڵ�ַ
**           pulKip         ��� KIP �ṹ������ַ
** �䡡��  : ERROR_NONE �ɹ�, ����ֵ ʧ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
INT  elfLoad (addr_t  ulElfAddr, addr_t  *pulEntry, addr_t  *pulKip)
{
    Elf_Ehdr    *pElfHdr = (Elf_Ehdr *)ulElfAddr;                       /*  elf �ļ�ͷ                  */
    Elf_Phdr    *pProgHdr;                                              /*  Program ��ͷ                */
    INT          iRes    = ERROR_NONE;
    INT          i;

    if ((iRes = __elfCheck(pElfHdr)) != ERROR_NONE) {                   /*  ��� elf �ļ�ͷ             */
        return  (iRes);
    }

    if (pulEntry != LW_NULL) {
        *pulEntry = (addr_t)pElfHdr->e_entry;                           /*  ��ȡ��ں���                */
    }

    pProgHdr = (Elf_Phdr *)((addr_t)pElfHdr + pElfHdr->e_phoff);        /*  ��λ�� prog_hdr_table ����  */

    /*
     *  ���� program_header_table ���ж�
     */
    for (i = 0; i < pElfHdr->e_phnum; i++) {
        if ((pProgHdr->p_type == PT_KIP) && (pulKip != LW_NULL)) {
            *pulKip = (addr_t)pProgHdr->p_paddr;                        /*  ��ȡ KIP ��ַ               */

        } else if (pProgHdr->p_type == PT_LOAD) {
            /*
             *  �� PT_LOAD ���ԵĶν��п���
             */
            addr_t ulLoadAddr  = pProgHdr->p_paddr;                     /*  �μ��ص�ַ                  */
            addr_t ulImageAddr = ulElfAddr + pProgHdr->p_offset;        /*  ���ڴ澵���ַ              */
            size_t uiLoadSize  = pProgHdr->p_filesz;                    /*  �μ��ش�С                  */
            size_t uiMemSize   = pProgHdr->p_memsz;                     /*  ���ڴ�ռ�ô�С              */

            if ((iRes = regionRequest(ulLoadAddr, uiMemSize)) != ERROR_NONE) {
                                                                        /*  ����ָ�����ڴ�,�����Ϸ��� */
                return  (iRes);
            }

            lib_memcpy((PVOID)ulLoadAddr, (CPVOID)ulImageAddr, uiLoadSize);
                                                                        /*  ���ƶ����ݵ�����ص�ַ      */
            if (uiMemSize > uiLoadSize) {
                lib_memset((PVOID)(ulLoadAddr + uiLoadSize),            /*  ���ʣ���ڴ�                */
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
