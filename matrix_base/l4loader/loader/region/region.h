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
** ��   ��   ��: region.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2017 �� 10 �� 31 ��
**
** ��        ��: region ����.
*********************************************************************************************************/

#ifndef __REGION_H
#define __REGION_H

/*********************************************************************************************************
  �ڴ��������ṹ��
*********************************************************************************************************/
typedef struct {
    addr_t       RG_ulAddr;                                             /*  �ڴ�����ʼ��ַ              */
    size_t       RG_stSize;                                             /*  �ڴ�����С                  */

#define REGION_TYPE_ROM     0x1                                         /*  BIOS,ROM ռ���ڴ�           */
#define REGION_TYPE_RAM     0x2                                         /*  ���������ڴ�                */
    UINT32       RG_uiType;                                             /*  �ڴ�������                  */
} REGION;
typedef REGION  *PREGION;
/*********************************************************************************************************
  �ӿں���
*********************************************************************************************************/
INT  regionInit(REGION  *prgBspRegions, UINT  uiNum);
INT  regionRequest(addr_t  ulAddr, size_t  stSize);
INT  regionWrite(KIP  *pkipPage);

#endif                                                                  /*  __REGION_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/