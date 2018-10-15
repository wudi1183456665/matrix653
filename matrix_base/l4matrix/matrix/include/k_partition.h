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
** 文   件   名: k_partition.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 11 月 28 日
**
** 描        述: 这是微内核进程类型定义文件.
*********************************************************************************************************/

#ifndef __K_PARTITION_H
#define __K_PARTITION_H

/*********************************************************************************************************
  进程控制块
*********************************************************************************************************/

typedef struct {
    MX_PAGE_TABLE         PRTN_pgtblMem;                                /*  页表                        */
    ULONG                 PRTN_ulMemUsedSize;                           /*  本进程使用的物理内存大小    */

    MX_LIST_LINE_HEADER   PRTN_lineVcpu;                                /*  属于本进程的 VCPU 链表头    */

    PMX_PRTN_INFO         RTTN_ppiPtrnInfo;                             /*  进程配置资源信息            */

    BOOL                  PRTN_bIsUsed;                                 /*  本结构体是否正在使用        */

    MX_OBJECT_ID          PRTN_ulPrtnId;                                /*  本进程内核对象 ID           */
} MX_PRTN;
typedef MX_PRTN          *PMX_PRTN;

/*********************************************************************************************************
  用户和内核共用内存块
*********************************************************************************************************/

typedef struct {
    ULONG                 KUMEM_ulUserAddr;                             /*  用户程序访问的虚拟地址      */
    ULONG                 KUMEM_ulKernAddr;                             /*  内核代码访问的虚拟地址      */
    ULONG                 KUMEM_ulSize;
} MX_KUMEM;
typedef MX_KUMEM         *PMX_KUMEM;

/*********************************************************************************************************
  MMU 页面标志
*********************************************************************************************************/

#define MX_MMU_FLAG_VALID               0x01                            /*  映射有效                    */
#define MX_MMU_FLAG_UNVALID             0x00                            /*  映射无效                    */

#define MX_MMU_FLAG_ACCESS              0x02                            /*  可以访问                    */
#define MX_MMU_FLAG_UNACCESS            0x00                            /*  不能访问                    */

#define MX_MMU_FLAG_WRITABLE            0x04                            /*  可以写操作                  */
#define MX_MMU_FLAG_UNWRITABLE          0x00                            /*  不可以写操作                */

#define MX_MMU_FLAG_EXECABLE            0x08                            /*  可以执行代码                */
#define MX_MMU_FLAG_UNEXECABLE          0x00                            /*  不可以执行代码              */

#define MX_MMU_FLAG_CACHEABLE           0x10                            /*  可以缓冲                    */
#define MX_MMU_FLAG_UNCACHEABLE         0x00                            /*  不可以缓冲                  */

#define MX_MMU_FLAG_BUFFERABLE          0x20                            /*  可以写缓冲                  */
#define MX_MMU_FLAG_UNBUFFERABLE        0x00                            /*  不可以写缓冲                */

#define MX_MMU_FLAG_GUARDED             0x40                            /*  进行严格的权限检查          */
#define MX_MMU_FLAG_UNGUARDED           0x00                            /*  不进行严格的权限检查        */

/*********************************************************************************************************
  默认页面标志
*********************************************************************************************************/

#define MX_MMU_FLAG_KERN                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_EXECABLE |     \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE |   \
                                         MX_MMU_FLAG_GUARDED)           /*  内核镜像区域                */
#define MX_MMU_FLAG_KERN_NOC             (MX_MMU_FLAG_VALID |       \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_EXECABLE |     \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE)        /*  内核镜像区域(内核进程中可以运行用户模式线程, 仅测试时使用)                */
#if 0
#define MX_MMU_FLAG_DEVS                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_UNCACHEABLE |  \
                                         MX_MMU_FLAG_UNBUFFERABLE | \
                                         MX_MMU_FLAG_GUARDED)           /*  特殊功能寄存器区域          */
#else
#define MX_MMU_FLAG_DEVS                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_UNCACHEABLE |  \
                                         MX_MMU_FLAG_UNBUFFERABLE)      /*  特殊功能寄存器区域          */
#endif


#define MX_MMU_FLAG_EXEC                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_EXECABLE |     \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE |   \
                                         MX_MMU_FLAG_GUARDED)           /*  可执行区域                  */

#define MX_MMU_FLAG_READ                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE |   \
                                         MX_MMU_FLAG_GUARDED)           /*  只读区域                    */
                                         
#define MX_MMU_FLAG_RDWR                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE |   \
                                         MX_MMU_FLAG_GUARDED)           /*  读写区域                    */

#define MX_MMU_FLAG_DMA                 (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_GUARDED)           /*  物理硬件映射 (CACHE 一致的) */
                                         
#define MX_MMU_FLAG_FAIL                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_UNACCESS |     \
                                         MX_MMU_FLAG_GUARDED)           /*  不允许访问区域              */

#endif                                                                  /*  __K_PARTITION_H             */
/*********************************************************************************************************
  END
*********************************************************************************************************/

