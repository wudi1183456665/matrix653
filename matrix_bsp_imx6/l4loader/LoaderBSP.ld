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
** 文   件   名: SylixOSBSP.ld
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017-11-12
**
** 描        述: arm-mini2440 链接脚本文件
*********************************************************************************************************/

/*********************************************************************************************************
    包含配置文件
*********************************************************************************************************/

INCLUDE "config.lds"

/*********************************************************************************************************
    链接配置
*********************************************************************************************************/

OUTPUT_FORMAT(elf32-littlearm)                                          /*  binary file format          */
OUTPUT_ARCH(ARM)                                                        /*  target select               */
ENTRY(_start)

/*********************************************************************************************************
    段定义
*********************************************************************************************************/

SECTIONS
{
    . = ORIGIN(RAM);
    
    __b_load = .;
    __b_kernel = .;
    _loader_start = .;
  
/*********************************************************************************************************
    代码段  LMA == VMA == ORIGIN(RAM)
*********************************************************************************************************/

    .text : {
        PROVIDE(_text = .);
        *(.bsp.header)
        *(.text)
        *(.text.*)                                                      /*  cpp namespace function      */
        *(.romrun)                                                      /*  ROM 中必须的函数            */
        *(.rodata)                                                      /*  read-only data (constants)  */
        *(.rodata*)
        *(.glue_7)
        *(.glue_7t)

        *(.got.plt) 
        *(.got) 

        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        PROVIDE(_etext = .);
    }
    
/*********************************************************************************************************
     数据段
*********************************************************************************************************/
    .data : {
        PROVIDE(_data = .);
        
        *(.data*) 
        
        PROVIDE(_edata = .);
    }

/*********************************************************************************************************
    matrix.elf 文件数据
*********************************************************************************************************/

    .matrix_elf_section : {
        . = ALIGN(8);
        PROVIDE(_matrix_elf_start = .);

        *(.matrix_elf_data);

        . = ALIGN(8);
        PROVIDE(_matrix_elf_end = .);
    }

    __e_load = .;

/*********************************************************************************************************
    清零段

    NOLOAD 表示不装载, 启动程序只需要借助 __bss_start 和 __bss_end 两个符号指定的起始地址和结束地址
    将 .bss 区域清零即可. (注意 *.noinit 可以不进行清零)
*********************************************************************************************************/

    .bss (NOLOAD): {   
        . = ALIGN(4);
        *(.noinit)

        . = ALIGN(4);
        PROVIDE(__bss_start = .);
        
        *(.bss)

        . = ALIGN(4);
        *(COMMON)

        . = ALIGN(4);
        PROVIDE(__bss_end = .);
    }

/*********************************************************************************************************
    栈段
    
    Loader 启动时使用
*********************************************************************************************************/

    .stack (NOLOAD) : {
        . = ALIGN(8);
        PROVIDE (__stack_start = .);
        
        . += BOOT_STACK_SIZE;
        
        . = ALIGN(8);
        PROVIDE (__stack_end = .);
    }

    __e_kernel = .;
    _loader_end = .;
}

/*********************************************************************************************************
  END
*********************************************************************************************************/
