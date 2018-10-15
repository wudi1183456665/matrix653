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
** ��   ��   ��: k_partition.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 11 �� 28 ��
**
** ��        ��: ����΢�ں˽������Ͷ����ļ�.
*********************************************************************************************************/

#ifndef __K_PARTITION_H
#define __K_PARTITION_H

/*********************************************************************************************************
  ���̿��ƿ�
*********************************************************************************************************/

typedef struct {
    MX_PAGE_TABLE         PRTN_pgtblMem;                                /*  ҳ��                        */
    ULONG                 PRTN_ulMemUsedSize;                           /*  ������ʹ�õ������ڴ��С    */

    MX_LIST_LINE_HEADER   PRTN_lineVcpu;                                /*  ���ڱ����̵� VCPU ����ͷ    */

    PMX_PRTN_INFO         RTTN_ppiPtrnInfo;                             /*  ����������Դ��Ϣ            */

    BOOL                  PRTN_bIsUsed;                                 /*  ���ṹ���Ƿ�����ʹ��        */

    MX_OBJECT_ID          PRTN_ulPrtnId;                                /*  �������ں˶��� ID           */
} MX_PRTN;
typedef MX_PRTN          *PMX_PRTN;

/*********************************************************************************************************
  �û����ں˹����ڴ��
*********************************************************************************************************/

typedef struct {
    ULONG                 KUMEM_ulUserAddr;                             /*  �û�������ʵ������ַ      */
    ULONG                 KUMEM_ulKernAddr;                             /*  �ں˴�����ʵ������ַ      */
    ULONG                 KUMEM_ulSize;
} MX_KUMEM;
typedef MX_KUMEM         *PMX_KUMEM;

/*********************************************************************************************************
  MMU ҳ���־
*********************************************************************************************************/

#define MX_MMU_FLAG_VALID               0x01                            /*  ӳ����Ч                    */
#define MX_MMU_FLAG_UNVALID             0x00                            /*  ӳ����Ч                    */

#define MX_MMU_FLAG_ACCESS              0x02                            /*  ���Է���                    */
#define MX_MMU_FLAG_UNACCESS            0x00                            /*  ���ܷ���                    */

#define MX_MMU_FLAG_WRITABLE            0x04                            /*  ����д����                  */
#define MX_MMU_FLAG_UNWRITABLE          0x00                            /*  ������д����                */

#define MX_MMU_FLAG_EXECABLE            0x08                            /*  ����ִ�д���                */
#define MX_MMU_FLAG_UNEXECABLE          0x00                            /*  ������ִ�д���              */

#define MX_MMU_FLAG_CACHEABLE           0x10                            /*  ���Ի���                    */
#define MX_MMU_FLAG_UNCACHEABLE         0x00                            /*  �����Ի���                  */

#define MX_MMU_FLAG_BUFFERABLE          0x20                            /*  ����д����                  */
#define MX_MMU_FLAG_UNBUFFERABLE        0x00                            /*  ������д����                */

#define MX_MMU_FLAG_GUARDED             0x40                            /*  �����ϸ��Ȩ�޼��          */
#define MX_MMU_FLAG_UNGUARDED           0x00                            /*  �������ϸ��Ȩ�޼��        */

/*********************************************************************************************************
  Ĭ��ҳ���־
*********************************************************************************************************/

#define MX_MMU_FLAG_KERN                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_EXECABLE |     \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE |   \
                                         MX_MMU_FLAG_GUARDED)           /*  �ں˾�������                */
#define MX_MMU_FLAG_KERN_NOC             (MX_MMU_FLAG_VALID |       \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_EXECABLE |     \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE)        /*  �ں˾�������(�ں˽����п��������û�ģʽ�߳�, ������ʱʹ��)                */
#if 0
#define MX_MMU_FLAG_DEVS                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_UNCACHEABLE |  \
                                         MX_MMU_FLAG_UNBUFFERABLE | \
                                         MX_MMU_FLAG_GUARDED)           /*  ���⹦�ܼĴ�������          */
#else
#define MX_MMU_FLAG_DEVS                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_UNCACHEABLE |  \
                                         MX_MMU_FLAG_UNBUFFERABLE)      /*  ���⹦�ܼĴ�������          */
#endif


#define MX_MMU_FLAG_EXEC                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_EXECABLE |     \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE |   \
                                         MX_MMU_FLAG_GUARDED)           /*  ��ִ������                  */

#define MX_MMU_FLAG_READ                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE |   \
                                         MX_MMU_FLAG_GUARDED)           /*  ֻ������                    */
                                         
#define MX_MMU_FLAG_RDWR                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_CACHEABLE |    \
                                         MX_MMU_FLAG_BUFFERABLE |   \
                                         MX_MMU_FLAG_GUARDED)           /*  ��д����                    */

#define MX_MMU_FLAG_DMA                 (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_ACCESS |       \
                                         MX_MMU_FLAG_WRITABLE |     \
                                         MX_MMU_FLAG_GUARDED)           /*  ����Ӳ��ӳ�� (CACHE һ�µ�) */
                                         
#define MX_MMU_FLAG_FAIL                (MX_MMU_FLAG_VALID |        \
                                         MX_MMU_FLAG_UNACCESS |     \
                                         MX_MMU_FLAG_GUARDED)           /*  �������������              */

#endif                                                                  /*  __K_PARTITION_H             */
/*********************************************************************************************************
  END
*********************************************************************************************************/

