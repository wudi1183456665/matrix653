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
** 文   件   名: elf_type.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 11 月 01 日
**
** 描        述: 与体系结构相关的 elf 文件
*********************************************************************************************************/

#ifndef __ELF_TYPE_H
#define __ELF_TYPE_H

#include "../arch/arch_inc.h"                                           /*  体系结构相关                */

/*********************************************************************************************************
  32-bit ELF base types. 
*********************************************************************************************************/

typedef addr_t   Elf32_Addr;
typedef UINT16   Elf32_Half;
typedef UINT32   Elf32_Off;
typedef UINT32   Elf32_Word;

/*********************************************************************************************************
  64-bit ELF base types. 
*********************************************************************************************************/

typedef addr_t   Elf64_Addr;
typedef UINT16   Elf64_Half;
typedef UINT64   Elf64_Off;
typedef UINT32   Elf64_Word;
typedef UINT64   Elf64_Xword;

/*********************************************************************************************************
  These constants are for the segment types stored in the image headers  
*********************************************************************************************************/

#define PT_NULL                 0
#define PT_LOAD                 1
#define PT_DYNAMIC              2
#define PT_INTERP               3
#define PT_NOTE                 4
#define PT_SHLIB                5
#define PT_PHDR                 6
#define PT_TLS                  7                                       /* Thread local storage segment */
#define PT_LOOS                 0x60000000                              /* OS-specific                  */
#define PT_HIOS                 0x6fffffff                              /* OS-specific                  */
#define PT_LOPROC               0x70000000
#define PT_HIPROC               0x7fffffff
#define PT_GNU_EH_FRAME         0x6474e550

#define PT_GNU_STACK            (PT_LOOS + 0x474e551)

/*********************************************************************************************************
  These constants define the different elf file types 
*********************************************************************************************************/

#define ET_NONE                 0
#define ET_REL                  1                                       /*  REL (Relocatable file)      */
#define ET_EXEC                 2                                       /*  EXEC (Executable file)      */
#define ET_DYN                  3
#define ET_CORE                 4
#define ET_LOPROC               0xff00
#define ET_HIPROC               0xffff

/*********************************************************************************************************
  e_machine 
*********************************************************************************************************/

#define EM_NONE                 0                                       /* e_machine                    */
#define EM_M32                  1                                       /* AT&T WE 32100                */
#define EM_SPARC                2                                       /* Sun SPARC                    */
#define EM_386                  3                                       /* Intel 80386                  */
#define EM_68K                  4                                       /* Motorola 68000               */
#define EM_88K                  5                                       /* Motorola 88000               */
#define EM_486                  6                                       /* Intel 80486                  */
#define EM_860                  7                                       /* Intel i860                   */
#define EM_MIPS                 8                                       /* MIPS family                  */
#define EM_PARISC               1                                       /* HPPA                         */
#define EM_SPARC32PLUS          1                                       /* Sun's "v8plus"               */
#define EM_PPC                  2                                       /* PowerPC family               */
#define EM_PPC64                2                                       /* PowerPC64                    */
#define EM_S390                 2                                       /* IBM S/390                    */
#define EM_SPU                  2                                       /* Cell BE SPU                  */
#define EM_ARM                  4                                       /* ARM/Thumb family             */
#define EM_SH                   4                                       /* SuperH                       */
#define EM_SPARCV9              4                                       /* SPARC v9 64-bit              */
#define EM_H8_300               4                                       /* Renesas H8/300,300H,H8S      */
#define EM_IA_64                5                                       /* HP/Intel IA-64               */
#define EM_X86_64               6                                       /* AMD x86-64                   */
#define EM_CRIS                 7                                       /* Axis 32-bit processor        */
#define EM_V850                 8                                       /* NEC v850                     */
#define EM_M32R                 8                                       /* Renesas M32R                 */
#define EM_MN10300              8                                       /* Panasonic/MEI MN10300, AM33  */
#define EM_BLACKFIN             1                                       /* ADI Blackfin Processor       */
#define EM_ALTERA_NIOS2         1                                       /* ALTERA NIOS2                 */
#define EM_TI_C6000             1                                       /* TI DSP C6x                   */
#define EM_AARCH64              1                                       /* ARM 64 bit                   */
#define EM_FRV                  0                                       /* Fujitsu FR-V                 */
#define EM_AVR32                0                                       /* Atmel AVR32                  */
#define EM_ALPHA                0                                       /* Alpha                        */

/*********************************************************************************************************
  elf header
*********************************************************************************************************/

#define EI_NIDENT   16

typedef struct elf32_hdr {
    unsigned char   e_ident[EI_NIDENT];
    Elf32_Half      e_type;
    Elf32_Half      e_machine;
    Elf32_Word      e_version;
    Elf32_Addr      e_entry;                                /* Entry point                              */
    Elf32_Off       e_phoff;
    Elf32_Off       e_shoff;
    Elf32_Word      e_flags;
    Elf32_Half      e_ehsize;
    Elf32_Half      e_phentsize;
    Elf32_Half      e_phnum;
    Elf32_Half      e_shentsize;
    Elf32_Half      e_shnum;
    Elf32_Half      e_shstrndx;
} Elf32_Ehdr;

typedef struct elf64_hdr {
    unsigned char   e_ident[EI_NIDENT];                     /* ELF "magic number"                       */
    Elf64_Half      e_type;
    Elf64_Half      e_machine;
    Elf64_Word      e_version;
    Elf64_Addr      e_entry;                                /* Entry point virtual address              */
    Elf64_Off       e_phoff;                                /* Program header table file offset         */
    Elf64_Off       e_shoff;                                /* Section header table file offset         */
    Elf64_Word      e_flags;
    Elf64_Half      e_ehsize;
    Elf64_Half      e_phentsize;
    Elf64_Half      e_phnum;
    Elf64_Half      e_shentsize;
    Elf64_Half      e_shnum;
    Elf64_Half      e_shstrndx;
} Elf64_Ehdr;

/*********************************************************************************************************
  These constants define the permissions on sections in the program
  header, p_flags. 
*********************************************************************************************************/

#define PF_R        0x4
#define PF_W        0x2
#define PF_X        0x1

typedef struct elf32_phdr {
    Elf32_Word      p_type;
    Elf32_Off       p_offset;
    Elf32_Addr      p_vaddr;
    Elf32_Addr      p_paddr;
    Elf32_Word      p_filesz;
    Elf32_Word      p_memsz;
    Elf32_Word      p_flags;
    Elf32_Word      p_align;
} Elf32_Phdr;

typedef struct elf64_phdr {
    Elf64_Word      p_type;
    Elf64_Word      p_flags;
    Elf64_Off       p_offset;                               /* Segment file offset                      */
    Elf64_Addr      p_vaddr;                                /* Segment virtual address                  */
    Elf64_Addr      p_paddr;                                /* Segment physical address                 */
    Elf64_Xword     p_filesz;                               /* Segment size in file                     */
    Elf64_Xword     p_memsz;                                /* Segment size in memory                   */
    Elf64_Xword     p_align;                                /* Segment alignment, file & memory         */
} Elf64_Phdr;

#define EI_MAG0         0                                   /* e_ident[] indexes                        */
#define EI_MAG1         1
#define EI_MAG2         2
#define EI_MAG3         3
#define EI_CLASS        4
#define EI_DATA         5
#define EI_VERSION      6
#define EI_OSABI        7
#define EI_PAD          8

#define ELFMAG0         0x7f                                /* EI_MAG                                   */
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'
#define ELFMAG          "\177ELF"
#define SELFMAG         4

#define ELFCLASSNONE    0                                   /* EI_CLASS                                 */
#define ELFCLASS32      1
#define ELFCLASS64      2
#define ELFCLASSNUM     3

#define ELFDATANONE     0                                   /* e_ident[EI_DATA]                         */
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2

/*********************************************************************************************************
  32bit elf
*********************************************************************************************************/
#if ELF_CLASS == ELFCLASS32

typedef Elf32_Ehdr      Elf_Ehdr;
typedef Elf32_Phdr      Elf_Phdr;

/*********************************************************************************************************
  64bit elf
*********************************************************************************************************/
#else                                                       /* ELF_CLASS == ELFCLASS32                  */

typedef Elf64_Ehdr      Elf_Ehdr;
typedef Elf64_Phdr      Elf_Phdr;

#endif                                                      /* ELF_CLASS == ELFCLASS32                  */
#endif                                                      /* __ELF_TYPE_H                             */
/*********************************************************************************************************
  END
*********************************************************************************************************/
