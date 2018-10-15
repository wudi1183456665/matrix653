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
** ��   ��   ��: lib_memcpy.c
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2006 �� 12 �� 13 ��
**
** ��        ��: ��
**
** BUG:
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
** ��������: lib_memcpy
** ��������: �ڴ濽��
** �䡡��  : pvDest      Ŀ���ڴ�
**           pvSrc       Դ�ڴ�
**           stCount     �ֽ���
** �䡡��  : Ŀ���ڴ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
PVOID  lib_memcpy (PVOID  pvDest, CPVOID   pvSrc, size_t  stCount)
{
    REGISTER PUCHAR    pucDest;
    REGISTER PUCHAR    pucSrc;

    REGISTER ULONG    *pulDest;
    REGISTER ULONG    *pulSrc;

             ULONG     ulTemp;

    pucDest = (PUCHAR)pvDest;
    pucSrc  = (PUCHAR)pvSrc;

    if (stCount == 0 || pucDest == pucSrc) {
        return  (pvDest);
    }

    if (pucDest < pucSrc) {
        /*
         *  ����ѭ�򿽱�
         */
        ulTemp = (ULONG)pucSrc;
        if ((ulTemp | (ULONG)pucDest) & __LONGMASK) {
            /*
             *  �����Ƕ��벿��
             */
            if (((ulTemp ^ (ULONG)pucDest) & __LONGMASK) || (stCount < __LONGSIZE)) {
                ulTemp = (ULONG)stCount;
            } else {
                ulTemp = (ULONG)(__LONGSIZE - (ulTemp & __LONGMASK));
            }

            stCount -= (size_t)ulTemp;
            __TLOOP1(*pucDest++ = *pucSrc++);
        }

        /*
         *  ���ֶ��뿽��
         */
        ulTemp = (ULONG)(stCount / __LONGSIZE);

        pulDest = (ULONG *)pucDest;
        pulSrc  = (ULONG *)pucSrc;

        while (ulTemp >= __BIGBLOCKSTEP) {
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;

            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;

            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;

            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;
            *pulDest++ = *pulSrc++;

            ulTemp -= __BIGBLOCKSTEP;
        }

        while (ulTemp >= __LITLOCKSTEP) {
            *pulDest++ = *pulSrc++;
            ulTemp -= __LITLOCKSTEP;
        }

        pucDest = (PUCHAR)pulDest;
        pucSrc  = (PUCHAR)pulSrc;

        /*
         *  ʣ�ಿ��
         */
        ulTemp = (ULONG)(stCount & __LONGMASK);
        __TLOOP(*pucDest++ = *pucSrc++);

    } else {
        /*
         *  ����ѭ�򿽱�
         */
        pucSrc  += stCount;
        pucDest += stCount;

        ulTemp = (ULONG)pucSrc;
        if ((ulTemp | (ULONG)pucDest) & __LONGMASK) {
            /*
             *  �����Ƕ��벿��
             */
            if (((ulTemp ^ (ULONG)pucDest) & __LONGMASK) || (stCount <= __LONGSIZE)) {
                ulTemp = (ULONG)stCount;
            } else {
                ulTemp &= __LONGMASK;
            }
            stCount -= (size_t)ulTemp;
            __TLOOP1(*--pucDest = *--pucSrc);
        }

        /*
         *  ���ֶ��뿽��
         */
        ulTemp = (ULONG)(stCount / __LONGSIZE);

        pulDest = (ULONG *)pucDest;
        pulSrc  = (ULONG *)pucSrc;

        while (ulTemp >= __BIGBLOCKSTEP) {
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;

            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;

            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;

            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;
            *--pulDest = *--pulSrc;

            ulTemp -= __BIGBLOCKSTEP;
        }

        while (ulTemp >= __LITLOCKSTEP) {
            *--pulDest = *--pulSrc;
            ulTemp -= __LITLOCKSTEP;
        }

        pucDest = (PUCHAR)pulDest;
        pucSrc  = (PUCHAR)pulSrc;

        /*
         *  ʣ�ಿ��
         */
        ulTemp = (ULONG)(stCount & __LONGMASK);
        __TLOOP(*--pucDest = *--pucSrc);
    }

    return  (pvDest);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
