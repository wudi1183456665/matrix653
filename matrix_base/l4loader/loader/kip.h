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
** 文   件   名: kip.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 10 月 31 日
**
** 描        述: Kernel-Interface Page 结构体定义.
*********************************************************************************************************/

#ifndef __KIP_H
#define __KIP_H

#include "./arch/arch_inc.h"                                            /*  体系结构相关                */

/*********************************************************************************************************
  L4 标准规定的 Kernel-Interface Page, 在 32 位机上，占 256(0x100) 字节
  和文档 <L4 eXperimental Kernel(Version X.2).pdf> 第 1.1 小节保持一致
*********************************************************************************************************/
typedef struct {
    /* 
     *  偏移 0x00
     */
#define L4_KIP_MAGIC            (0x4be6344c)                            /*  KIP 结构体魔数 "L4μK"      */
    UINT32        KIP_uiMagic;                                          /*  KIP 结构体魔数              */
    UINT32        KIP_uiVersion;                                        /*  L4 API 版本                 */
    UINT32        KIP_uiReserved0[2];                                   /*  保留 8 个字节               */

    /* 
     *  偏移 0x10
     */
    UINT32        KIP_uiL4sylixosIP;                                    /*  L4SylixOS 服务入口地址      */
    UINT32        KIP_uiL4sylixosSP;                                    /*  L4SylixOS 服务堆栈          */
    UINT32        KIP_uiReserved1[2];                                   /*  保留 8 个字节               */

    /* 
     *  偏移 0x20
     */
    UINT32        KIP_uiReserved2[4];                                   /*  保留 16 个字节              */

    /* 
     *  偏移 0x30
     */
    UINT32        KIP_uiReserved3[4];                                   /*  保留 16 个字节              */

    /* 
     *  偏移 0x40
     */
    UINT32        KIP_uiReserved4[4];                                   /*  保留 16 个字节              */

    /* 
     *  偏移 0x50
     */
    UINT32        KIP_uiReserved5[1];                                   /*  保留 4 个字节               */
    UINT32        KIP_uiMemDescNum;                                     /*  内存描述区个数              */
    UINT32        KIP_uiReserved6[2];                                   /*  保留 8 个字节               */

    /* 
     *  偏移 0x60
     */
    UINT32        KIP_uiReserved7[16];                                  /*  保留 64 个字节              */

    /* 
     *  偏移 0xa0
     */
    UINT64        KIP_ui64Clock;                                        /*  系统时间                    */
    UINT32        KIP_uiReserved8[2];                                   /*  保留 8 个字节               */

    /* 
     *  偏移 0xb0
     */
    UINT32        KIP_uiReserved9[16];                                  /*  保留 16 个字节              */

    /* 
     *  偏移 0xf0
     */
    CHAR          KIP_cPlatName[16];                                    /*  平台名称                    */
} KIP;
typedef KIP      *PKIP;

/*********************************************************************************************************
  内存描述
*********************************************************************************************************/
typedef struct {
    /*
     *  高 22 位: 起始地址, 低 10 位: 属性值
     *  +----------+--------+--------+------------+--------+
     *  | start:22 | virt:1 | none:1 | ext_type:4 | type:4 |
     *  +----------+--------+--------+------------+--------+
     *
     *  内存类型和文档 <L4 eXperimental Kernel(Version X.2).pdf> 第 7.7 小节保持一致
     */
#define MEMORY_TYPE_UNDEFINED       0x0
#define MEMORY_TYPE_CONVENTIONAL    0x1                                 /*  常规内存                    */
#define MEMORY_TYPE_RESERVED        0x2                                 /*  微内核保留内存              */
#define MEMORY_TYPE_DEDICATED       0x3                                 /*  专用内存（用户不可用）      */
#define MEMORY_TYPE_SHARED          0x4                                 /*  所有用户都能使用            */
#define MEMORY_TYPE_LOADER          0xe                                 /*  Loader 自身占用的内存       */
#define MEMORY_TYPE_ARCH            0xf                                 /*  Architecture dependent      */
    addr_t        MD_ulAddrType;                                        /*  起始地址及属性值            */
    size_t        MD_stSize;                                            /*  内存区大小                  */
} MEM_DESC;
typedef struct MEM_DESC  *PMEM_DESC;

#endif                                                                  /*  __KIP_H                     */
/*********************************************************************************************************
  END
*********************************************************************************************************/

