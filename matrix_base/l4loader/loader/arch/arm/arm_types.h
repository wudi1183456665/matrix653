/*********************************************************************************************************
**
**                                    �й�������Դ��֯
**
**                                   Ƕ��ʽʵʱ����ϵͳ
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: arm_types.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2013 �� 05 �� 04 ��
**
** ��        ��: ARM �������.
*********************************************************************************************************/

#ifndef __ARM_ARCH_TYPES_H
#define __ARM_ARCH_TYPES_H

/*********************************************************************************************************
  void ����
*********************************************************************************************************/

typedef          void               VOID;                               /*  void   ����                 */

/*********************************************************************************************************
  ����������������Ͷ���
*********************************************************************************************************/

typedef          int                BOOL;                               /*  ������������                */

typedef          void              *PVOID;                              /*  void * ����                 */
typedef const    void              *CPVOID;                             /*  const void  *               */

typedef          char               CHAR;                               /*  8 λ�ַ�����                */
typedef unsigned char               UCHAR;                              /*  8 λ�޷����ַ�����          */
typedef unsigned char              *PUCHAR;                             /*  8 λ�޷����ַ�����ָ��      */
typedef          char              *PCHAR;                              /*  8 λ�ַ�ָ�����            */
typedef const    char              *CPCHAR;                             /*  const char  *               */

typedef unsigned char               BYTE;                               /*  8 λ�ֽڱ���                */
typedef unsigned char              *PBYTE;                              /*  8 λ�ֽڱ���ָ��            */

typedef          long               LONG;                               /*  32/64 λ������              */
typedef unsigned long               ULONG;                              /*  32/64 λ�޷���������        */

typedef          int                INT;                                /*  ��������� int              */
typedef unsigned int                UINT;                               /*  ��������� unsigned int     */

/*********************************************************************************************************
  ע��: INT8 ������ signed ��. ���� int8_t ���ܱ�֤�� signed ��.
        �ܶ������Ĭ�� char �� unsigned. �����������ǿ�ƽ� INT8 ����Ϊ signed ��.
*********************************************************************************************************/

typedef signed   char               INT8;                               /*  char                        */
typedef unsigned char               UINT8;                              /*  8  λ�޷���������           */

typedef          short              INT16;                              /*  short                       */
typedef unsigned short              UINT16;                             /*  16 λ�޷���������           */

typedef          int                INT32;                              /*  long or int                 */
typedef unsigned int                UINT32;                             /*  32 λ�޷���������           */

typedef          long long          INT64;                              /*  long long                   */
typedef unsigned long long          UINT64;                             /*  64 λ�޷���������           */

/*********************************************************************************************************
  ��ַ�������.
*********************************************************************************************************/

typedef ULONG                       addr_t;
typedef ULONG                       size_t;

/*********************************************************************************************************
  fast variable
*********************************************************************************************************/

#define REGISTER                    register                            /*  �Ĵ�������                  */

#endif                                                                  /*  __ARM_ARCH_TYPES_H          */
/*********************************************************************************************************
  END
*********************************************************************************************************/