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
** ��   ��   ��: armGic.c
**
** ��   ��   ��: Li.Jing (�)
**
** �ļ���������: 2018 �� 1 �� 19 ��
**
** ��        ��: ͨ���жϿ�����.
*********************************************************************************************************/
#include <Matrix.h>
#include "config.h"                                                     /*  �������� & ���������       */
#include "driver/gic/gic.h"                                             /*  GIC ����                    */
#include "driver/uart/uart.h"                                           /*  �Թ�����                    */
#include "driver/timer/timer.h"
#include "driver/ccm_pll/ccm_pll.h"                                     /*  i.MX6  PLL ʱ��             */
#include "driver/cpu_utility/cpu_utility.h"                             /*  i.MX6  ��˿���             */
VOID     armDCacheClearAll(VOID);

void uart_put_hex (int v)
{
    static char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    int i;

    for (i = 7; i >= 0; i--) {
        uartPutChar(hex[(v >> (i*4)) & 0xF]);
    }
    uartPutChar(';');
}
/*********************************************************************************************************
** ��������: bspIntHandle
** ��������: �ж����
** ��  ��  : puctxSP   �������ں˶�ջ�е� USR ģʽ������
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��: armExcAsm.S: archIntEntry()
*********************************************************************************************************/
VOID  bspIntHandle (PARCH_USER_CTX  puctxSP)
{
    REGISTER UINT32  uiAck        = armGicIrqReadAck();
    REGISTER UINT32  uiSourceCpu  = (uiAck >> 10) & 0x7;
    REGISTER UINT32  uiVector     = uiAck & 0x1FF;                      /*  ��ȡӲ���жϺ�: 0 -- 255    */

    (VOID)uiSourceCpu;
    //uartPutChar('.');
#if 1
    //bspDebugMsg("[%d]bspIntHandle(): %d\n", archMpCur(), uiVector);

    if (uiVector == GIC_TICK_INT_VEC) {
        armGlobalTimerIntClear();                                       /*  ����ж�                    */
        schedTick();
    }
    archIntHandle((ULONG)uiVector, puctxSP);
#endif

    armGicIrqWriteDone(uiAck);
    //uart_put_hex((int)puctxSP);
}
VOID  bspClearInt (VOID)
{
    armGicIrqWriteDone(GIC_TICK_INT_VEC);
}
/*********************************************************************************************************
** ��������: bspIntVectorEnable
** ��������: ʹ��ָ�����ж�����
** ��  ��  : ulVec     �ж�����
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bspIntVectorEnable (ULONG  ulVec)
{
    armGicIntVecterEnable(ulVec, MX_FALSE, GIC_DEFAULT_INT_PRIO, 1 << 0);
}
/*********************************************************************************************************
** ��������: bspIntVectorDisable
** ��������: ����ָ�����ж�����
** ��  ��  : ulVec     �ж�����
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bspIntVectorDisable (ULONG  ulVector)
{
    armGicIntVecterDisable(ulVector);
}
/*********************************************************************************************************
** ��������: bspIntVectorIsEnable
** ��������: ���ָ�����ж������Ƿ�ʹ��
** ��  ��  : ulVector     �ж�����
** ��  ��  : MX_FALSE �� MX_TRUE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  bspIntVectorIsEnable (ULONG  ulVector)
{
    return  (armGicIrqIsEnable(ulVector) ? MX_TRUE : MX_FALSE);
}
/*********************************************************************************************************
** ��������: bspInfoSfrBase
** ��������: BSP IO �ռ����ַ
** �䡡��  : NONE
** �䡡��  : ���� RAM ����ַ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  bspInfoSfrBase (VOID)
{
    return  (BSP_CFG_SFR_BASE);
}
/*********************************************************************************************************
** ��������: bspInfoSfrSize
** ��������: BSP IO �ռ��С
** �䡡��  : NONE
** �䡡��  : RAM ��С
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  bspInfoSfrSize (VOID)
{
    return  (BSP_CFG_SFR_SIZE);
}
/*********************************************************************************************************
** ��������: bspInfoKernRamBase
** ��������: BSP ���� RAM ����ַ
** �䡡��  : NONE
** �䡡��  : ���� RAM ����ַ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  bspInfoKernRamBase (VOID)
{
    return  (BSP_CFG_KERN_RAM_BASE);
}
/*********************************************************************************************************
** ��������: bspInfoKernRamSize
** ��������: BSP ���� RAM ��С
** �䡡��  : NONE
** �䡡��  : RAM ��С
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  bspInfoKernRamSize (VOID)
{
    return  (BSP_CFG_KERN_RAM_SIZE);
}
/*********************************************************************************************************
** ��������: bspInfoPrtnRamBase
** ��������: BSP ���� RAM ����ַ
** �䡡��  : NONE
** �䡡��  : ���� RAM ����ַ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  bspInfoPrtnRamBase (VOID)
{
    return  (BSP_CFG_PRTN_RAM_BASE);
}
/*********************************************************************************************************
** ��������: bspInfoPrtnRamSize
** ��������: BSP ���� RAM ��С
** �䡡��  : NONE
** �䡡��  : RAM ��С
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  bspInfoPrtnRamSize (VOID)
{
    return  (BSP_CFG_PRTN_RAM_SIZE);
}
/*********************************************************************************************************
** ��������: bspMmuPgdMaxNum
** ��������: ���� pgd ����ʣ�����
** ��  ��  : NONE
** ��  ��  : pgd ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  bspMmuPgdMaxNum (VOID)
{
    return  (0);
}
/*********************************************************************************************************
** ��������: bspMmuPteMaxNum
** ��������: ���� pte ����ʣ�����
** ��  ��  : NONE
** ��  ��  : pte ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
ULONG  bspMmuPteMaxNum (VOID)
{
    return  (0);
}
/*********************************************************************************************************
** ��������: bspMpInt
** ��������: ��ָ���˲����˼��ж�
** ��  ��  : ulCpuId      ��ID ( 0, 1, 2, 3)
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bspMpInt (ULONG  ulCpuId)
{
    armGicSoftwareIntSend(GIC_IPI_INT_VEC(ulCpuId), GIC_SW_INT_OPTION_USE_TARGET_LIST, 1 << ulCpuId);
}
/*********************************************************************************************************
  SMP ͬ������
*********************************************************************************************************/
#define BSP_HOLDING_PEN_START    0xdeadbeef                             /*  �����Ӻ˵�ģ��              */
static volatile ULONG            _G_ulHoldingPen[MX_CFG_MAX_PROCESSORS];
/*********************************************************************************************************
** ��������: bspCpuUp
** ��������: ����ָ����
** ��  ��  : ulCpuId      ��ID ( 0, 1, 2, 3)
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��: intMatrix()
*********************************************************************************************************/
VOID  bspCpuUp (ULONG  ulCpuId)
{
    INTREG  iregInterLevel;

    iregInterLevel = archIntDisable();

    _G_ulHoldingPen[ulCpuId] = 0;
    KN_SMP_MB();

    armDCacheClearAll();                                                /*  ȫ����д����Ч              */

    imx6qCpuStart2nd(ulCpuId);

    KN_SMP_MB();
    while (_G_ulHoldingPen[ulCpuId] != BSP_HOLDING_PEN_START) {         /*  �ȴ������˳�ʼ�����        */
        ;
    }

    archIntEnable(iregInterLevel);
}
/*********************************************************************************************************
** ��������: bspCpuUpDone
** ��������: һ���� CPU �������
** ��  ��  : NONE
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��: schedActiveCpu()
*********************************************************************************************************/
VOID  bspCpuUpDone (VOID)
{
    ULONG  ulCpuId = archMpCur();
    INT    i;

    KN_SMP_MB();
    _G_ulHoldingPen[ulCpuId] = BSP_HOLDING_PEN_START;                   /*  ���˳�ʼ�����              */
    KN_SMP_MB();

    if (ulCpuId == 0) {
        return;
    }

__WAIT_CPUS:
    for (i = 1; i < MX_NCPUS; i++) {
        if (_G_ulHoldingPen[i] != BSP_HOLDING_PEN_START) {              /*  �ȴ����дӺ˳�ʼ�����      */
            goto __WAIT_CPUS;
        }
    }

    armDCacheClearAll();                                                /*  ȫ����д����Ч              */
}
/*********************************************************************************************************
** ��������: bspCpuDown
** ��������: ֹͣһ�� CPU
** ��  ��  : ulCPUID      ��ID ( 0, 1, 2, 3)
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bspCpuDown (ULONG  ulCPUId)
{
    imx6qCpuDisable(ulCPUId);
}
/*********************************************************************************************************
** ��������: bspCpuIpiVectorInstall
** ��������: ��װ IPI ��ʹ��(�˼��жϺ�: CPU0 #0, CPU1 #1, CPU2 #2, CPU3 #3)
** ��  ��  : NONE
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��: NONE
*********************************************************************************************************/
VOID  bspCpuIpiVectorInstall (VOID)
{
    ULONG  ulCpuId = archMpCur();

    armGicIntVecterEnable(GIC_IPI_INT_VEC(ulCpuId), MX_FALSE, GIC_IPI_INT_PRIO, 1 << ulCpuId);
}
/*********************************************************************************************************
** ��������: bspIntcInit
** ��������: �жϿ�������ʼ��
** ��  ��  : NONE
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bspDelayUs (ULONG  ulUs)
{
    REGISTER UINT32  uiCounter;

    if (ulUs) {
        uiCounter = ulUs * ((imx6qMainClkGet(CPU_CLK) / 2) / 1000000);
        if (uiCounter) {
            armPrivateTimerInit(MX_FALSE, uiCounter, 0, MX_FALSE);
            armPrivateTimerStart();
            while (armPrivateTimerCounterGet()) ;
            armPrivateTimerStop();
        }
    }
}
//spinlock_t  G_slDebugMsg;
/*********************************************************************************************************
** ��������: bspDebugMsg
** ��������: ��ʽ��ӡ����
** �䡡��  : pcFormat       �����ַ���
**           ...            �����б�
** �䡡��  :
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  bspDebugMsg (CPCHAR  pcFormat, ... )
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

    //archSpinLock(&G_slDebugMsg);

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

    //archSpinUnlock(&G_slDebugMsg);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
