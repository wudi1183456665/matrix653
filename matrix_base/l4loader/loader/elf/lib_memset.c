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
** ��   ��   ��: lib_memset.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2006 �� 12 �� 13 ��
**
** ��        ��: ��

** BUG:
2011.06.22  �� iCount С�� 0 ʱ, ������.
2013.03.29  memset iC ��ת��Ϊ uchar ����.
2016.07.15  �Ż��ٶ�.
*********************************************************************************************************/
#include "lib_memory.h"
/*********************************************************************************************************
  ���ֶ��뷽ʽ����
*********************************************************************************************************/
#define __LONGSIZE              sizeof(ULONG)
#define __LONGMASK              (__LONGSIZE - 1)

#define __BIGBLOCKSTEP          (16)
#define __BIGBLOCKSIZE          (__LONGSIZE << 4)

#define __LITLOCKSTEP           (1)
#define __LITLOCKSIZE           (__LONGSIZE)

#define __TLOOP(s)              if (ulTemp) {       \
                                    __TLOOP1(s);    \
                                }
#define __TLOOP1(s)             do {                \
                                    s;              \
                                } while (--ulTemp)
/*********************************************************************************************************
** ��������: lib_memset
** ��������: �ڴ�����
** �䡡��  : pvDest      �������ڴ�
**           iC          ��������
**           stCount     �ֽ���
** �䡡��  : Ŀ���ڴ�
*********************************************************************************************************/
PVOID  lib_memset (PVOID  pvDest, INT  iC, size_t  stCount)
{
    REGISTER INT       i;
    REGISTER ULONG     ulTemp;
    REGISTER PUCHAR    pucDest = (PUCHAR)pvDest;
    REGISTER ULONG    *pulDest;

             UCHAR     ucC     = (UCHAR)iC;
             ULONG     ulFill  = (ULONG)ucC;

    if (stCount == 0) {
        return  (pvDest);
    }

    for (i = 1; i < (__LONGSIZE / sizeof(UCHAR)); i++) {                /*  ���� ulong ����ĸ�ֵ����   */
        ulFill = (ulFill << 8) + ucC;
    }

    if ((ULONG)pucDest & __LONGMASK) {                                  /*  ����ǰ�˷Ƕ��벿��          */
        if (stCount < __LONGSIZE) {
            ulTemp = (ULONG)stCount;
        } else {
            ulTemp = (ULONG)(__LONGSIZE - ((ULONG)pucDest & __LONGMASK));
        }

        stCount -= (size_t)ulTemp;
        __TLOOP1(*pucDest++ = ucC);
    }

    /*
     *  ���ֶ��봦��
     */
    ulTemp = (INT)(stCount / __LONGSIZE);

    pulDest = (ULONG *)pucDest;

    while (ulTemp >= __BIGBLOCKSTEP) {
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;

        *pulDest++ = ulFill;
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;

        *pulDest++ = ulFill;
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;

        *pulDest++ = ulFill;
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;
        *pulDest++ = ulFill;

        ulTemp -= __BIGBLOCKSTEP;
    }

    while (ulTemp >= __LITLOCKSTEP) {
        *pulDest++ = ulFill;
        ulTemp -= __LITLOCKSTEP;
    }

    pucDest = (PUCHAR)pulDest;

    /*
     *  ʣ�ಿ��
     */
    ulTemp = (ULONG)(stCount & __LONGMASK);
    __TLOOP(*pucDest++ = ucC);

    return  (pvDest);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
