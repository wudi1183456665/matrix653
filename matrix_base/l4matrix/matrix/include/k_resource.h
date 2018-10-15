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
** 文   件   名: k_resource.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 28 日
**
** 描        述: 各分区资源分配.
*********************************************************************************************************/

#ifndef __K_RESOURCE_H
#define __K_RESOURCE_H

/*********************************************************************************************************
  内存区资源
*********************************************************************************************************/
typedef struct {
#define MX_PRTN_MEM_AUTO  0xFFFFFFFF                                    /*  不指定物理地址              */
    ULONG              MA_ulPhysAddr;                                   /*  内存区起始物理地址          */

    ULONG              MA_ulVirtAddr;                                   /*  映射后的虚拟起始地址        */
    ULONG              MA_ulSize;                                       /*  内存区大小                  */

#define MX_PRTN_MEM_NORM  0
#define MX_PRTN_MEM_DMA   1
    ULONG              MA_ulType;                                       /*  内存区类型                  */
} MX_MEM_AREA;
typedef MX_MEM_AREA   *PMX_MEM_AREA;
/*********************************************************************************************************
  分区镜像文件信息
*********************************************************************************************************/
typedef struct {
    CPCHAR             PI_cpcId;
    CPCHAR             PI_cpcFile;
    CPCHAR             PI_cpcStart;
    CPCHAR             PI_cpcEnd;

    ULONG              PI_ulEntryAddr;                                  /*  本分区镜像入口虚拟地址      */
    UINT32             PI_uiPriority;                                   /*  进程主线程优先级            */
    PMX_MEM_AREA       PI_pmaMems;                                      /*  内存区资源表                */
    UINT32             PI_uiMemMum;                                     /*  有效内存区个数              */
    UINT32            *PI_puiIrqs;                                      /*  中断资源表                  */
    UINT32             PI_uiIrqMum;                                     /*  有效中断资源个数            */
} MX_PRTN_INFO;
typedef MX_PRTN_INFO  *PMX_PRTN_INFO;
/*********************************************************************************************************
  分区文件及信息所在的节区
*********************************************************************************************************/
#define MX_PRTNFILE_SEC     __attribute__((__used__)) \
                            __attribute__((__section__(".prtn.file")))
#define MX_PRTNDATA_SEC     __attribute__((__used__)) \
                            __attribute__((__section__(".prtn.data")))
#define MX_PRTNINFO_SEC     __attribute__((__used__)) \
                            __attribute__((__section__(".prtn.info")))
/*********************************************************************************************************
  将 file 路径中的文件内容放入 .prtn.file 节中
*********************************************************************************************************/
#define __INC_BIN_FILE(file, start, end)              \
    asm (                                             \
             ".section .prtn.file, \"awx\"      \n\t" \
             ".p2align 12                       \n\t" \
             ".global " #start "," #end "       \n\t" \
             #start":                           \n\t" \
             "    .incbin \"" file "\"          \n\t" \
             #end":                             \n\t" \
        );
/*********************************************************************************************************
  在 l4matrix.elf 中包含一分区镜像文件
*********************************************************************************************************/
#define MX_LOAD_PRTN(id, file, entry, prio, mems, irqs)         \
    __INC_BIN_FILE(file, _prtn_##id##_start, _prtn_##id##_end)  \
                                                                \
    extern CHAR _prtn_##id##_start[];                           \
    extern CHAR _prtn_##id##_end[];                             \
    static CHAR _prtn_##id##_name[] MX_PRTNDATA_SEC = #id;      \
    static CHAR _prtn_##id##_file[] MX_PRTNDATA_SEC = file;     \
                                                                \
    static MX_PRTN_INFO _prtn_##id##_info MX_PRTNINFO_SEC =     \
                        {                                       \
                            _prtn_##id##_name,                  \
                            _prtn_##id##_file,                  \
                            _prtn_##id##_start,                 \
                            _prtn_##id##_end,                   \
                                                                \
                            entry,                              \
                            prio,                               \
                            mems,                               \
                            sizeof(mems)/sizeof(mems[0]),       \
                            irqs,                               \
                            sizeof(irqs)/sizeof(irqs[0])        \
                        };

#endif                                                                  /*  __K_RESOURCE_H              */
/*********************************************************************************************************
  END
*********************************************************************************************************/

