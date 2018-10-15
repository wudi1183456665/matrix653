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
** ��   ��   ��: dump.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 03 �� 09 ��
**
** ��        ��: ���������Ϣ.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
  �������ڲ�����
*********************************************************************************************************/
typedef __builtin_va_list          va_list;
#define va_start(ap, fmt)          (__builtin_va_start(ap, fmt))
#define va_arg(ap, type)           (__builtin_va_arg(ap, type))
#define va_end(ap)                 (__builtin_va_end(ap))
#define EOF                        (-1)
/*********************************************************************************************************
  ���ڼĴ���
*********************************************************************************************************/
#define UART1_Rx_TST       (*(volatile int *)(0x02020098))              /*  Receiver status reg         */
#define UART1_Rx_VAL       (*(volatile int *)(0x02020000))              /*  Receiver reg                */
/*********************************************************************************************************
  BSP �ṩ�Ĵ��������ӿ�
*********************************************************************************************************/
extern INT  uartPutChar(UCHAR  ucChar);
extern INT  uartPutStr(PUCHAR  pucStr);
/*********************************************************************************************************
** ��������: dump
** ��������: ���������Ϣ
** �䡡��  : pcFormat       �����ַ���
**           ...            �����б�
** ȫ�ֱ���: 
** ����ģ��: 
** ע    ��: �ڳ�ʼ���׶β��ܵ���
*********************************************************************************************************/
VOID  dump (CPCHAR  pcFormat, ... )
{
    va_list                arp;
    UCHAR                  ucCurr;                                      /*  ��ǰ����ĸ�ʽ�ַ�          */
    UINT8                  ucFlags;                                     /*  1:��� 2:%l 4:���� 8:%ll    */
    UINT8                  ucRadix;                                     /*  ���ƣ�֧�� 10, 16           */
    UINT32                 uiVal;                                       /*  �Ӳ����ж�ȡ��������ֵ      */
    UCHAR                  ucVal[16];                                   /*  �������ֵת�����ַ���      */
    INT                    i;
    INT                    iWidth;                                      /*  ��ʽ�ַ����еĿ��ֵ        */
    INT                    iOnce;                                       /*  ÿ�δ�ӡ�ַ�����            */
    INT                    iReturn;                                     /*  ��������ֵ���ܴ�ӡ�ַ�����  */

    kernelEnter();

    va_start(arp, pcFormat);

    for (iOnce = iReturn = 0; iOnce != EOF; iReturn += iOnce) {
        ucCurr = *pcFormat++;
        if (ucCurr == 0) {
            break;                                                      /*  ��ӡ����                    */
        }

        if (ucCurr != '%') {                                            /*  �Ǹ�ʽ���ַ���ֱ�Ӵ�ӡ      */
            iOnce = uartPutChar(ucCurr);
            if (iOnce != EOF) {
                iOnce = 1;
            }
            continue;
        }

        iWidth = ucFlags = 0;
        ucCurr = *pcFormat++;                                           /*  ��ȡ % ��һ���ַ�           */

        /*
         *  ���� %%
         */
        if (ucCurr == '%') {                                            /*  print '%'                   */
            iOnce = uartPutChar(ucCurr);
            continue;
        }

        /*
         *  �����ʽ�ַ����е�����ʽ %08, % 12
         */
        if (ucCurr == '0') {                                            /*  Flag: '0' padding           */
            ucFlags = 1;
            ucCurr = *pcFormat++;
        }
        if (ucCurr == ' ') {                                            /*  Flag: ' ' padding           */
            ucCurr = *pcFormat++;
        }
        while (ucCurr >= '0' && ucCurr <= '9') {                        /*  print width                 */
            iWidth = iWidth * 10 + (ucCurr - '0');
            ucCurr = *pcFormat++;
        }

        /*
         *  �����ʽ�ַ����е�����ʽ %s, %c
         */
        if (ucCurr == 's') {                                            /*  Type is string              */
            iOnce = uartPutStr(va_arg(arp, PUCHAR));
            continue;
        }
        if (ucCurr == 'c') {                                            /*  Type is character           */
            iOnce = uartPutChar(va_arg(arp, INT));
            if (iOnce != EOF) {
                iOnce = 1;
            }
            continue;
        }

        /*
         *  �����ʽ�ַ����е����ͳ��� %l, %ll
         */
        if (ucCurr == 'l') {                                            /*  Prefix: Size is long int    */
            ucFlags |= 2;
            ucCurr   = *pcFormat++;
            if (ucCurr == 'l') {                                        /*  Prefix: Size is long long   */
                ucFlags |= 8;
                ucCurr   = *pcFormat++;
            }
        }

        /*
         *  �����ʽ�ַ����еĽ��Ƹ�ʽ %d, %u, %X, %x, %p
         */
        ucRadix = 0;
        if (ucCurr == 'd') {
            ucRadix = 10;                                               /*  Type is signed decimal      */
        }
        if (ucCurr == 'u') {
            ucRadix = 10;                                               /*  Type is unsigned decimal    */
        }
        if (ucCurr == 'X' || ucCurr == 'x' || ucCurr == 'p') {
            ucRadix = 16;                                               /*  Type is unsigned hexdecimal */
        }
        if (ucRadix == 0) {
            break;                                                      /*  Unknown type                */
        }

        /*
         *  ��ȡ�����е�����ֵ
         */
        if (ucFlags & 8) {                                              /*  Get the long long value     */
            uiVal = (UINT64)va_arg(arp, INT64);

        } else if (ucFlags & 2) {                                       /*  Get the long long value     */
            uiVal = (UINT32)va_arg(arp, INT32);

        } else {
            uiVal = (ucCurr == 'd') ? (UINT32)va_arg(arp, INT) : (UINT32)va_arg(arp, UINT);
        }

        /*
         *  ���ڸ���ֵ������ȡ����ֵ
         */
        if (ucCurr == 'd') {
            if (uiVal & 0x80000000) {                                   /*  ����                        */
                uiVal    = 0 - uiVal;
                ucFlags |= 4;
            }
        }

        /*
         *  ������ת�����ַ���
         */
        i = sizeof(ucVal) - 1;
        ucVal[i] = 0;
        do {
            ucCurr = (UCHAR)(uiVal % ucRadix + '0');
            if (ucCurr > '9') {
                ucCurr += 7;
            }
            ucVal[--i] = ucCurr;
            uiVal /= ucRadix;
        } while (i && uiVal);
        if (i && (ucFlags & 4)) {
            ucVal[--i] = '-';
        }

        /*
         *  ���ַ�������ָ�����
         */
        iWidth = sizeof(ucVal) - 1 - iWidth;
        while (i && i > iWidth) {
            ucVal[--i] = (ucFlags & 1) ? '0' : ' ';
        }

        iOnce = uartPutStr(&ucVal[i]);                                  /*  ��ӡ��ֵ�ַ���              */
    }

    va_end(arp);

    kernelExit();
}
/*********************************************************************************************************
** ��������: dumpUctx
** ��������: ��ӡ�û�������
** �䡡��  : puctx      �û�������ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  dumpUctx (const PARCH_USER_CTX  puctx)
{
    INTREG      iregIntLevel;

    iregIntLevel = kernelEnterAndDisIrq();
    bspDebugMsg("--[%d]--------UCTX INFO----------\n", archMpCur());
    bspDebugMsg("puctx->UCTX_uiR0    = 0x%X\n", puctx->UCTX_uiR0);
    bspDebugMsg("puctx->UCTX_uiR1    = 0x%X\n", puctx->UCTX_uiR1);
    bspDebugMsg("puctx->UCTX_uiR2    = 0x%X\n", puctx->UCTX_uiR2);
    bspDebugMsg("puctx->UCTX_uiR3    = 0x%X\n", puctx->UCTX_uiR3);
    bspDebugMsg("puctx->UCTX_uiR4    = 0x%X\n", puctx->UCTX_uiR4);
    bspDebugMsg("puctx->UCTX_uiR5    = 0x%X\n", puctx->UCTX_uiR5);
    bspDebugMsg("puctx->UCTX_uiR6    = 0x%X\n", puctx->UCTX_uiR6);
    bspDebugMsg("puctx->UCTX_uiR7    = 0x%X\n", puctx->UCTX_uiR7);
    bspDebugMsg("puctx->UCTX_uiR8    = 0x%X\n", puctx->UCTX_uiR8);
    bspDebugMsg("puctx->UCTX_uiR9    = 0x%X\n", puctx->UCTX_uiR9);
    bspDebugMsg("puctx->UCTX_uiR10   = 0x%X\n", puctx->UCTX_uiR10);
    bspDebugMsg("puctx->UCTX_uiR11   = 0x%X\n", puctx->UCTX_uiR11);
    bspDebugMsg("puctx->UCTX_uiR12   = 0x%X\n", puctx->UCTX_uiR12);
    bspDebugMsg("puctx->UCTX_uiR13   = 0x%X\n", puctx->UCTX_uiR13);
    bspDebugMsg("puctx->UCTX_uiR14   = 0x%X\n", puctx->UCTX_uiR14);
    bspDebugMsg("puctx->UCTX_uiR15   = 0x%X\n", puctx->UCTX_uiR15);
    bspDebugMsg("puctx->UCTX_uiR16   = 0x%X\n", puctx->UCTX_uiR16);
    bspDebugMsg("puctx->UCTX_uiR17   = 0x%X\n", puctx->UCTX_uiR17);
    bspDebugMsg("puctx->UCTX_uiR18   = 0x%X\n", puctx->UCTX_uiR18);
    bspDebugMsg("puctx->UCTX_uiR19   = 0x%X\n", puctx->UCTX_uiR19);
    bspDebugMsg("puctx->UCTX_uiR20   = 0x%X\n", puctx->UCTX_uiR20);
    bspDebugMsg("puctx->UCTX_uiR21   = 0x%X\n", puctx->UCTX_uiR21);
    bspDebugMsg("puctx->UCTX_uiR22   = 0x%X\n", puctx->UCTX_uiR22);
    bspDebugMsg("puctx->UCTX_uiR23   = 0x%X\n", puctx->UCTX_uiR23);
    bspDebugMsg("puctx->UCTX_uiR24   = 0x%X\n", puctx->UCTX_uiR24);
    bspDebugMsg("puctx->UCTX_uiR25   = 0x%X\n", puctx->UCTX_uiR25);
    bspDebugMsg("puctx->UCTX_uiR26   = 0x%X\n", puctx->UCTX_uiR26);
    bspDebugMsg("puctx->UCTX_uiR27   = 0x%X\n", puctx->UCTX_uiR27);
    bspDebugMsg("puctx->UCTX_uiR28   = 0x%X\n", puctx->UCTX_uiR28);
    bspDebugMsg("puctx->UCTX_uiR29   = 0x%X\n", puctx->UCTX_uiR29);
    bspDebugMsg("puctx->UCTX_uiR30	= 0x%X\n", puctx->UCTX_uiR30);
    bspDebugMsg("puctx->UCTX_uiR31	= 0x%X\n", puctx->UCTX_uiR31);
    bspDebugMsg("puctx->UCTX_uiCr   	= 0x%X\n", puctx->UCTX_uiCr);
    bspDebugMsg("puctx->UCTX_uiCtr   = 0x%X\n", puctx->UCTX_uiCtr);
    bspDebugMsg("puctx->UCTX_uiLr    = 0x%X\n", puctx->UCTX_uiLr);
    bspDebugMsg("================[%d]=============\n", archMpCur());
    kernelExitAndEnIrq(iregIntLevel);
}
/*********************************************************************************************************
** ��������: dumpUctx1
** ��������: ��������ӡ�û�������
** �䡡��  : puctx      �û�������ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  dumpUctx1 (const PARCH_USER_CTX  puctx)
{
	if (puctx->UCTX_uiR15 == 0x30157FA0) {
		bspDebugMsg("puctx->UCTX_uiPc = 0x%X\n", puctx->UCTX_uiR15);
		bspDebugMsg("puctx->UCTX_uiR0   = 0x%X\n", puctx->UCTX_uiR0);
	}
}
/*********************************************************************************************************
** ��������: dumpReady
** ��������: ��ӡ������
** �䡡��  : pready     ������ָ��
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  dumpReady (const PMX_READY  pready)
{
    INT                     i;
    PMX_VCPU                pvcpu;
    PMX_RDYP                prdyp;
    PMX_LIST_RING           pring;
    INTREG                  iregIntLevel;

    iregIntLevel = kernelEnterAndDisIrq();
    bspDebugMsg("--[%d]-------READY INFO----------\n", archMpCur());
    for (i = MX_PRIO_HIGHEST; i <= MX_PRIO_LOWEST; i++) {
        prdyp = &pready->RDY_rdyp[i];

        if (!_RdyPrioIsEmpty(prdyp)) {
            bspDebugMsg("%d : ", i);

            pring = prdyp->RDYP_pringReadyHeader;
            do {
                pvcpu = _LIST_ENTRY(pring, MX_VCPU, VCPU_ringReady);
                bspDebugMsg(" %p", pvcpu);
                pring = _list_ring_get_next(pring);
            } while (pring != prdyp->RDYP_pringReadyHeader);

            bspDebugMsg("\n");
        }
    }
    bspDebugMsg("================[%d]=============\n", archMpCur());
    kernelExitAndEnIrq(iregIntLevel);
}
/*********************************************************************************************************
** ��������: dumpCpus
** ��������: ��ӡ�������� CPU ���ƿ���Ϣ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  dumpCpus (VOID)
{
    INT                     i;
    PMX_PHYS_CPU            pcpu;
    INTREG                  iregIntLevel;

    iregIntLevel = kernelEnterAndDisIrq();
    bspDebugMsg("--[%d]--------CPU INFO-----------\n", archMpCur());
    for (i = 0; i < MX_NCPUS; i++) {
        pcpu = MX_CPU_GET(i);
        bspDebugMsg(" %d:\n", i);
        bspDebugMsg("pcpu->CPU_pvcpuCur   = 0x%X\n", pcpu->CPU_pvcpuCur);
        bspDebugMsg("pcpu->CPU_pvcpuHigh  = 0x%X\n", pcpu->CPU_pvcpuHigh);
        bspDebugMsg("pcpu->CPU_CAND       = 0x%X\n", pcpu->CPU_cand.CAND_pvcpuCand);
        bspDebugMsg("pcpu->CPU_CAND_ROT   = 0x%X\n", pcpu->CPU_cand.CAND_bNeedRotate);
        bspDebugMsg("pcpu->CPU_ulIpiPend  = 0x%X\n", pcpu->CPU_ulIpiPend);
    } 
    bspDebugMsg("================[%d]=============\n", archMpCur());
    kernelExitAndEnIrq(iregIntLevel);
}
/*********************************************************************************************************
** ��������: dumpHeap
** ��������: ��ӡ΢�ں˶���Ϣ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  dumpHeap (VOID)
{
    UINT   i;

    for (i = 0; i < _K_heapKernel.HEAP_uiValidNum; i++) {
        PMX_SEGMENT p = &_K_heapKernel.HEAP_segments[i];
        bspDebugMsg("[0x% 8x, 0x% 8x] : %X\n", p->SEGMENT_ulLowAddr, p->SEGMENT_ulHigAddr, p->SEGMENT_uiOwner);
    }
}
/*********************************************************************************************************
** ��������: dumpHook
** ��������: ���������ӡ
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  dumpHook (VOID)
{
    if ((UART1_Rx_TST & (1 << 0))) {
        INT  i = UART1_Rx_VAL;
        (VOID)i;

        dumpReady(MX_GLOBAL_RDY());
        dumpCpus();
    }
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

