/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: ppcMmu.h
**
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2016 �� 01 �� 14 ��
**
** ��        ��: PowerPC ��ϵ���� MMU ����.
*********************************************************************************************************/

#ifndef __ARCH_PPCMMU_H
#define __ARCH_PPCMMU_H


/*********************************************************************************************************
  FUNCPTR
*********************************************************************************************************/

#ifdef __cplusplus
typedef INT         (*FUNCPTR)(...);                                    /*  function returning int      */
typedef off_t       (*OFFTFUNCPTR)(...);                                /*  function returning off_t    */
typedef size_t      (*SIZETFUNCPTR)(...);                               /*  function returning size_t   */
typedef ssize_t     (*SSIZETFUNCPTR)(...);                              /*  function returning ssize_t  */
typedef LONG        (*LONGFUNCPTR)(...);                                /*  function returning long     */
typedef ULONG       (*ULONGFUNCPTR)(...);                               /*  function returning ulong    */
typedef VOID        (*VOIDFUNCPTR)(...);                                /*  function returning void     */
typedef PVOID       (*PVOIDFUNCPTR)(...);                               /*  function returning void *   */
typedef BOOL        (*BOOLFUNCPTR)(...);                                /*  function returning bool     */

#else
typedef INT         (*FUNCPTR)();                                       /*  function returning int      */
typedef INT64       (*OFFTFUNCPTR)();                                   /*  function returning off_t    */
typedef size_t      (*SIZETFUNCPTR)();                                  /*  function returning size_t   */
typedef LONG     (*SSIZETFUNCPTR)();                                 /*  function returning ssize_t  */
typedef LONG        (*LONGFUNCPTR)();                                   /*  function returning long     */
typedef ULONG       (*ULONGFUNCPTR)();                                  /*  function returning ulong    */
typedef VOID        (*VOIDFUNCPTR)();                                   /*  function returning void     */
typedef PVOID       (*PVOIDFUNCPTR)();                                  /*  function returning void *   */
typedef BOOL        (*BOOLFUNCPTR)();                                   /*  function returning bool     */
#endif                                                                  /*  _cplusplus                  */

/*********************************************************************************************************
  MMU ת����Ŀ����
*********************************************************************************************************/
#if !defined(__ASSEMBLY__) && !defined(ASSEMBLY)

typedef UINT32  MX_PGD_TRANSENTRY;                                      /*  ҳĿ¼����                  */
typedef UINT32  MX_PMD_TRANSENTRY;                                      /*  �м�ҳĿ¼����              */

typedef union {
    struct {
        UINT        PTE_uiRPN       : 20;                               /*  ����ҳ��                    */
        UINT        PTE_ucReserved1 :  3;                               /*  ����                        */
        UINT        PTE_bRef        :  1;                               /*  ����λ                      */
        UINT        PTE_bChange     :  1;                               /*  �޸�λ                      */
        UINT        PTE_ucWIMG      :  4;                               /*  �ڴ�� CACHE ����λ         */
        UINT        PTE_bReserved2  :  1;                               /*  ����                        */
        UINT        PTE_ucPP        :  2;                               /*  ҳ����Ȩ��λ                */
    };                                                                  /*  ͨ�õ� PPC32 PTE            */
    UINT32          PTE_uiValue;                                        /*  ֵ                          */

    struct {
        /*
         * ����ֵ���� TLB MISS ʱ��װ�� MAS2 MAS3 �Ĵ���
         */
        UINT        MAS3_uiRPN      : 20;                               /*  ����ҳ��                    */

        UINT        MAS3_ucReserved0:  2;                               /*  ����                        */

        /*
         * �����û��������� TLB MISS ʱ��װ�� MAS2 �Ĵ���
         *
         * MAS2 �Ĵ������� X0 X1 G E λ, G �� E λ�̶�Ϊ 0
         * X0 X1 λ�� MAS4 �Ĵ����� X0D X1D �Զ���װ
         */
#define MAS3_bValid      MAS3_bUserAttr0                                /*  �Ƿ���Ч                    */
#define MAS3_bWT         MAS3_bUserAttr1                                /*  �Ƿ�д��͸                  */
#define MAS3_bUnCache    MAS3_bUserAttr2                                /*  �Ƿ񲻿� Cache              */
#define MAS3_bMemCoh     MAS3_bUserAttr3                                /*  �Ƿ����ڴ�һ����          */

        UINT        MAS3_bUserAttr0 :  1;                               /*  �û����� 0                  */
        UINT        MAS3_bUserAttr1 :  1;                               /*  �û����� 1                  */
        UINT        MAS3_bUserAttr2 :  1;                               /*  �û����� 2                  */
        UINT        MAS3_bUserAttr3 :  1;                               /*  �û����� 3                  */

        UINT        MAS3_bUserExec  :  1;                               /*  �û���ִ��Ȩ��              */
        UINT        MAS3_bSuperExec :  1;                               /*  ����Ա��ִ��Ȩ��            */

        UINT        MAS3_bUserWrite :  1;                               /*  �û���дȨ��                */
        UINT        MAS3_bSuperWrite:  1;                               /*  ����Ա��дȨ��              */

        UINT        MAS3_bUserRead  :  1;                               /*  �û��ɶ�Ȩ��                */
        UINT        MAS3_bSuperRead :  1;                               /*  ����Ա�ɶ�Ȩ��              */
    };                                                                  /*  E500 PTE                    */
    UINT32          MAS3_uiValue;                                       /*  ֵ                          */
} MX_PTE_TRANSENTRY;                                                    /*  ҳ����Ŀ����                */
#endif
/*********************************************************************************************************
  mmu ִ�й���
*********************************************************************************************************/

typedef MX_PGD_TRANSENTRY  *(*PGDFUNCPTR)();
typedef MX_PMD_TRANSENTRY  *(*PMDFUNCPTR)();
typedef MX_PTE_TRANSENTRY  *(*PTEFUNCPTR)();

typedef struct {
    ULONG                    MMUOP_ulOption;                            /*  MMU ѡ��                    */
#define MX_VMM_MMU_FLUSH_TLB_MP     0x01                                /*  ÿһ�����Ƿ�Ҫ����      */

    FUNCPTR                  MMUOP_pfuncMemInit;                        /*  ��ʼ���ڴ�, (ҳ���Ŀ¼��)  */
    FUNCPTR                  MMUOP_pfuncGlobalInit;                     /*  ��ʼ��ȫ��ӳ���ϵ          */

    PGDFUNCPTR               MMUOP_pfuncPGDAlloc;                       /*  ���� PGD �ռ�               */
    VOIDFUNCPTR              MMUOP_pfuncPGDFree;                        /*  �ͷ� PGD �ռ�               */
    PMDFUNCPTR               MMUOP_pfuncPMDAlloc;                       /*  ���� PMD �ռ�               */
    VOIDFUNCPTR              MMUOP_pfuncPMDFree;                        /*  �ͷ� PMD �ռ�               */

    PTEFUNCPTR               MMUOP_pfuncPTEAlloc;                       /*  ���� PTE �ռ�               */
    VOIDFUNCPTR              MMUOP_pfuncPTEFree;                        /*  �ͷ� PTE �ռ�               */

    BOOLFUNCPTR              MMUOP_pfuncPGDIsOk;                        /*  PGD ������Ƿ���ȷ          */
    BOOLFUNCPTR              MMUOP_pfuncPMDIsOk;                        /*  PMD ������Ƿ���ȷ          */

    BOOLFUNCPTR              MMUOP_pfuncPTEIsOk;                        /*  PTE ������Ƿ���ȷ          */

    PGDFUNCPTR               MMUOP_pfuncPGDOffset;                      /*  ͨ����ַ���ָ�� PGD ����   */
    PMDFUNCPTR               MMUOP_pfuncPMDOffset;                      /*  ͨ����ַ���ָ�� PMD ����   */
    PTEFUNCPTR               MMUOP_pfuncPTEOffset;                      /*  ͨ����ַ���ָ�� PTE ����   */

    FUNCPTR                  MMUOP_pfuncPTEPhysGet;                     /*  ͨ�� PTE ��Ŀ��ȡ�����ַ   */

    ULONGFUNCPTR             MMUOP_pfuncFlagGet;                        /*  ���ҳ���־                */
    FUNCPTR                  MMUOP_pfuncFlagSet;                        /*  ����ҳ���־ (��ǰδʹ��)   */

    VOIDFUNCPTR              MMUOP_pfuncMakeTrans;                      /*  ����ҳ��ת����ϵ������      */
    VOIDFUNCPTR              MMUOP_pfuncMakeCurCtx;                     /*  ���ǰ��ҳ��ת����ϵ      */
    VOIDFUNCPTR              MMUOP_pfuncInvalidateTLB;                  /*  ��Ч TLB ��                 */

    VOIDFUNCPTR              MMUOP_pfuncSetEnable;                      /*  ���� MMU                    */
    VOIDFUNCPTR              MMUOP_pfuncSetDisable;                     /*  �ر� MMU                    */
} MX_MMU_OP;

typedef MX_MMU_OP           *PMX_MMU_OP;
extern  MX_MMU_OP            _G_mmuOpLib;                               /*  MMU ����������              */


/*********************************************************************************************************
  mmu ��Ϣ
*********************************************************************************************************/

typedef struct __mx_mmu_context {
//    MX_VMM_AREA              MMUCTX_vmareaVirSpace;                     /*  �����ַ�ռ䷴���          */
    MX_PGD_TRANSENTRY       *MMUCTX_pgdEntry;                           /*  PGD ����ڵ�ַ              */                                                                 /* !MX_CFG_VMM_L4_HYPERVISOR_EN */
} MX_MMU_CONTEXT;
typedef MX_MMU_CONTEXT      *PMX_MMU_CONTEXT;
/*
 * *****************************************************************************
 */
VOID   ppcMmuInit(MX_MMU_OP *pmmuop, CPCHAR  pcMachineName);

ULONG  ppcMmuPteMissHandle(addr_t  ulAddr);
INT    ppcMmuPtePreLoad(addr_t  ulAddr);

UINT32 ppcMmuGetSRR1(VOID);
UINT32 ppcMmuGetDSISR(VOID);


#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/
