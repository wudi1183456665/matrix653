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
** ��   ��   ��: kip.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 10 �� 31 ��
**
** ��        ��: Kernel-Interface Page �ṹ�嶨��.
*********************************************************************************************************/

#ifndef __KIP_H
#define __KIP_H

#include "./arch/arch_inc.h"                                            /*  ��ϵ�ṹ���                */

/*********************************************************************************************************
  L4 ��׼�涨�� Kernel-Interface Page, �� 32 λ���ϣ�ռ 256(0x100) �ֽ�
  ���ĵ� <L4 eXperimental Kernel(Version X.2).pdf> �� 1.1 С�ڱ���һ��
*********************************************************************************************************/
typedef struct {
    /* 
     *  ƫ�� 0x00
     */
#define L4_KIP_MAGIC            (0x4be6344c)                            /*  KIP �ṹ��ħ�� "L4��K"      */
    UINT32        KIP_uiMagic;                                          /*  KIP �ṹ��ħ��              */
    UINT32        KIP_uiVersion;                                        /*  L4 API �汾                 */
    UINT32        KIP_uiReserved0[2];                                   /*  ���� 8 ���ֽ�               */

    /* 
     *  ƫ�� 0x10
     */
    UINT32        KIP_uiL4sylixosIP;                                    /*  L4SylixOS ������ڵ�ַ      */
    UINT32        KIP_uiL4sylixosSP;                                    /*  L4SylixOS �����ջ          */
    UINT32        KIP_uiReserved1[2];                                   /*  ���� 8 ���ֽ�               */

    /* 
     *  ƫ�� 0x20
     */
    UINT32        KIP_uiReserved2[4];                                   /*  ���� 16 ���ֽ�              */

    /* 
     *  ƫ�� 0x30
     */
    UINT32        KIP_uiReserved3[4];                                   /*  ���� 16 ���ֽ�              */

    /* 
     *  ƫ�� 0x40
     */
    UINT32        KIP_uiReserved4[4];                                   /*  ���� 16 ���ֽ�              */

    /* 
     *  ƫ�� 0x50
     */
    UINT32        KIP_uiReserved5[1];                                   /*  ���� 4 ���ֽ�               */
    UINT32        KIP_uiMemDescNum;                                     /*  �ڴ�����������              */
    UINT32        KIP_uiReserved6[2];                                   /*  ���� 8 ���ֽ�               */

    /* 
     *  ƫ�� 0x60
     */
    UINT32        KIP_uiReserved7[16];                                  /*  ���� 64 ���ֽ�              */

    /* 
     *  ƫ�� 0xa0
     */
    UINT64        KIP_ui64Clock;                                        /*  ϵͳʱ��                    */
    UINT32        KIP_uiReserved8[2];                                   /*  ���� 8 ���ֽ�               */

    /* 
     *  ƫ�� 0xb0
     */
    UINT32        KIP_uiReserved9[16];                                  /*  ���� 16 ���ֽ�              */

    /* 
     *  ƫ�� 0xf0
     */
    CHAR          KIP_cPlatName[16];                                    /*  ƽ̨����                    */
} KIP;
typedef KIP      *PKIP;

/*********************************************************************************************************
  �ڴ�����
*********************************************************************************************************/
typedef struct {
    /*
     *  �� 22 λ: ��ʼ��ַ, �� 10 λ: ����ֵ
     *  +----------+--------+--------+------------+--------+
     *  | start:22 | virt:1 | none:1 | ext_type:4 | type:4 |
     *  +----------+--------+--------+------------+--------+
     *
     *  �ڴ����ͺ��ĵ� <L4 eXperimental Kernel(Version X.2).pdf> �� 7.7 С�ڱ���һ��
     */
#define MEMORY_TYPE_UNDEFINED       0x0
#define MEMORY_TYPE_CONVENTIONAL    0x1                                 /*  �����ڴ�                    */
#define MEMORY_TYPE_RESERVED        0x2                                 /*  ΢�ں˱����ڴ�              */
#define MEMORY_TYPE_DEDICATED       0x3                                 /*  ר���ڴ棨�û������ã�      */
#define MEMORY_TYPE_SHARED          0x4                                 /*  �����û�����ʹ��            */
#define MEMORY_TYPE_LOADER          0xe                                 /*  Loader ����ռ�õ��ڴ�       */
#define MEMORY_TYPE_ARCH            0xf                                 /*  Architecture dependent      */
    addr_t        MD_ulAddrType;                                        /*  ��ʼ��ַ������ֵ            */
    size_t        MD_stSize;                                            /*  �ڴ�����С                  */
} MEM_DESC;
typedef struct MEM_DESC  *PMEM_DESC;

#endif                                                                  /*  __KIP_H                     */
/*********************************************************************************************************
  END
*********************************************************************************************************/

