/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: k_resource.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 12 �� 28 ��
**
** ��        ��: ��������Դ����.
*********************************************************************************************************/

#ifndef __K_RESOURCE_H
#define __K_RESOURCE_H

/*********************************************************************************************************
  �ڴ�����Դ
*********************************************************************************************************/
typedef struct {
#define MX_PRTN_MEM_AUTO  0xFFFFFFFF                                    /*  ��ָ�������ַ              */
    ULONG              MA_ulPhysAddr;                                   /*  �ڴ�����ʼ�����ַ          */

    ULONG              MA_ulVirtAddr;                                   /*  ӳ����������ʼ��ַ        */
    ULONG              MA_ulSize;                                       /*  �ڴ�����С                  */

#define MX_PRTN_MEM_NORM  0
#define MX_PRTN_MEM_DMA   1
    ULONG              MA_ulType;                                       /*  �ڴ�������                  */
} MX_MEM_AREA;
typedef MX_MEM_AREA   *PMX_MEM_AREA;
/*********************************************************************************************************
  ���������ļ���Ϣ
*********************************************************************************************************/
typedef struct {
    CPCHAR             PI_cpcId;
    CPCHAR             PI_cpcFile;
    CPCHAR             PI_cpcStart;
    CPCHAR             PI_cpcEnd;

    ULONG              PI_ulEntryAddr;                                  /*  ������������������ַ      */
    UINT32             PI_uiPriority;                                   /*  �������߳����ȼ�            */
    PMX_MEM_AREA       PI_pmaMems;                                      /*  �ڴ�����Դ��                */
    UINT32             PI_uiMemMum;                                     /*  ��Ч�ڴ�������              */
    UINT32            *PI_puiIrqs;                                      /*  �ж���Դ��                  */
    UINT32             PI_uiIrqMum;                                     /*  ��Ч�ж���Դ����            */
} MX_PRTN_INFO;
typedef MX_PRTN_INFO  *PMX_PRTN_INFO;
/*********************************************************************************************************
  �����ļ�����Ϣ���ڵĽ���
*********************************************************************************************************/
#define MX_PRTNFILE_SEC     __attribute__((__used__)) \
                            __attribute__((__section__(".prtn.file")))
#define MX_PRTNDATA_SEC     __attribute__((__used__)) \
                            __attribute__((__section__(".prtn.data")))
#define MX_PRTNINFO_SEC     __attribute__((__used__)) \
                            __attribute__((__section__(".prtn.info")))
/*********************************************************************************************************
  �� file ·���е��ļ����ݷ��� .prtn.file ����
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
  �� l4matrix.elf �а���һ���������ļ�
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

