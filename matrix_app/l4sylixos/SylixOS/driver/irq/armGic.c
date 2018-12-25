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
** ��   ��   ��: Jiao.JinXing (������)
**
** �ļ���������: 2013 �� 12 �� 12 ��
**
** ��        ��: ͨ���жϿ�����.
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"
#include "irq/armGic.h"
#include "arch/arm/common/cp15/armCp15.h"
/*********************************************************************************************************
  GIC �Ĵ���ƫ��
*********************************************************************************************************/
#define GIC_DISTRIBUTOR_REGS_BASE_OFFSET        (0x1000)/*  GIC �жϷ������Ĵ���ƫ��                    */
#define GIC_CPU_INTERFACE_REGS_BASE_OFFSET      (0x100) /*  GIC CPU �ӿڼĴ���ƫ��                      */
/*********************************************************************************************************
  GIC �жϷ������Ĵ���
  ʹ�� GICv2 �Ĵ�������, �������� GICv2 �Ĵ���
*********************************************************************************************************/
typedef struct {
    volatile UINT32     uiCTLR;                         /*  Distributor Control Register.               */
    volatile UINT32     uiTYPER;                        /*  Interrupt Controller Type Register.         */
    volatile UINT32     uiIIDR;                         /*  Distributor Implementer Identification Reg. */
    volatile UINT32     uiReserved0[29];
    volatile UINT32     uiIGROUPRn[8];                  /*  Interrupt Group Registers.                  */
    volatile UINT32     uiReserved1[24];
    volatile UINT32     uiISENABLERn[32];               /*  Interrupt Set-Enable Registers.             */
    volatile UINT32     uiICENABLERn[32];               /*  Interrupt Clear-Enable Registers.           */
    volatile UINT32     uiISPENDRn[32];                 /*  Interrupt Set-Pending Registers.            */
    volatile UINT32     uiICPENDRn[32];                 /*  Interrupt Clear-Pending Registers.          */
    volatile UINT32     uiICDABRn[32];                  /*  Active Bit Registers.                       */
    volatile UINT32     uiReserved2[32];
    volatile UINT8      uiIPRIORITYRn[255 * sizeof(UINT32)];/*  Interrupt Priority Registers.           */
    volatile UINT32     uiReserved3;
    volatile UINT8      uiITARGETSRn[255 * sizeof(UINT32)]; /*  Interrupt Processor Targets Registers.  */
    volatile UINT32     uiReserved4;
    volatile UINT32     uiICFGRn[64];                   /*  Interrupt Configuration Registers.          */
    volatile UINT32     uiReserved5[128];
    volatile UINT32     uiSGIR;                         /*  Software Generated Interrupt Register.      */
} GIC_DISTRIBUTOR_REG;
/*********************************************************************************************************
  GIC_DISTRIBUTOR_REG.uiCTLR �Ĵ���λ����
*********************************************************************************************************/
#define BTI_MASK_CTLR_ENABLEGRP1                (0x1ul << 1)
#define BTI_MASK_CTLR_EABBLEGRP0                (0x1ul << 0)
/*********************************************************************************************************
  GIC_DISTRIBUTOR_REG.uiSGIR �Ĵ���λ����
*********************************************************************************************************/
#define BIT_OFFS_SGIR_TARGETLISTFILTER          (24)
#define BTI_MASK_SGIR_TARGETLISTFILTER          (0x3ul << BIT_OFFS_SGIR_TARGETLISTFILTER)

#define BIT_OFFS_SGIR_CPUTARGETLIST             (16)
#define BTI_MASK_SGIR_CPUTARGETLIST             (0xFFul << BIT_OFFS_SGIR_CPUTARGETLIST)

#define BIT_OFFS_SGIR_NSATT                     (15)
#define BTI_MASK_SGIR_NSATT                     (0x1ul << BIT_OFFS_SGIR_NSATT),

#define BIT_OFFS_SGIR_SGIINTID                  (0)
#define BTI_MASK_SGIR_SGIINTID                  (0xFul << BIT_OFFS_SGIR_SGIINTID)
/*********************************************************************************************************
  GIC CPU �ӿڼĴ���
  ʹ�� GICv2 �Ĵ�������, �������� GICv2 �Ĵ���
*********************************************************************************************************/
typedef struct {
    volatile UINT32     uiCTLR;                         /*  CPU Interface Control Register.             */
    volatile UINT32     uiPMR;                          /*  Interrupt Priority Mask Register.           */
    volatile UINT32     uiBPR;                          /*  Binary Point Register.                      */
    volatile UINT32     uiIAR;                          /*  Interrupt Acknowledge Register.             */
    volatile UINT32     uiEOIR;                         /*  End of Interrupt Register.                  */
    volatile UINT32     uiRPR;                          /*  Running Priority Register.                  */
    volatile UINT32     uiHPPIR;                        /*  Highest Priority Pending Interrupt Register.*/
    volatile UINT32     uiABPR;                         /*  Aliased Binary Point Register.              */
                                                        /*  (only visible with a secure access)         */
    volatile UINT32     uiReserved[56];
    volatile UINT32     uiIIDR;                         /*  CPU Interface Identification Register.      */
} GIC_CPU_INTERFACE_REGS;
/*********************************************************************************************************
  GIC_CPU_INTERFACE_REGS.uiCTLR �Ĵ���λ����
*********************************************************************************************************/
#define BIT_OFFS_CTLR_ENABLES                   (0)
#define BTI_MASK_CTLR_ENABLES                   (0x1ul << BIT_OFFS_CTLR_ENABLES)

#define BIT_OFFS_CTLR_ENABLENS                  (1)
#define BTI_MASK_CTLR_ENABLENS                  (0x1ul << BIT_OFFS_CTLR_ENABLENS)

#define BIT_OFFS_CTLR_ACKCTL                    (2)
#define BTI_MASK_CTLR_ACKCTL                    (0x1ul << BIT_OFFS_CTLR_ACKCTL)

#define BIT_OFFS_CTLR_FIQENABLE                 (3)
#define BTI_MASK_CTLR_FIQENABLE                 (0x1ul << BIT_OFFS_CTLR_FIQENABLE)

#define BIT_OFFS_CTLR_SBPR                      (4)
#define BTI_MASK_CTLR_SBPR                      (0x1ul << BIT_OFFS_CTLR_SBPR)
/*********************************************************************************************************
** ��������: armGicIntDistributorGet
** ��������: ����жϷ�����
** �䡡��  : NONE
** �䡡��  : �жϷ������Ĵ�����ַ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LW_INLINE GIC_DISTRIBUTOR_REG  *armGicIntDistributorGet (VOID)
{
    REGISTER addr_t  ulBase = armPrivatePeriphBaseGet() + GIC_DISTRIBUTOR_REGS_BASE_OFFSET;

    return  ((GIC_DISTRIBUTOR_REG *)ulBase);
}
/*********************************************************************************************************
** ��������: armGicCpuInterfaceGet
** ��������: ��� CPU �ӿ�
** �䡡��  : NONE
** �䡡��  : CPU �ӿڼĴ�����ַ
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LW_INLINE GIC_CPU_INTERFACE_REGS  *armGicCpuInterfaceGet (VOID)
{
    REGISTER addr_t  ulBase = armPrivatePeriphBaseGet() + GIC_CPU_INTERFACE_REGS_BASE_OFFSET;

    return  ((GIC_CPU_INTERFACE_REGS *)ulBase);
}
/*********************************************************************************************************
** ��������: armGicIntRegOffsGet
** ��������: ����жϼĴ���ƫ��
** �䡡��  : uiIrqID           �жϺ�
** �䡡��  : �жϼĴ���ƫ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LW_INLINE UINT32  armGicIntRegOffsGet (UINT32  uiIrqID)
{
    return  (uiIrqID / 32);
}
/*********************************************************************************************************
** ��������: armGicIntBitOffsGet
** ��������: ����ж�λƫ��
** �䡡��  : uiIrqID           �жϺ�
** �䡡��  : �ж�λƫ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LW_INLINE UINT32  armGicIntBitOffsGet (UINT32  uiIrqID)
{
    return  (uiIrqID & 0x1F);
}
/*********************************************************************************************************
** ��������: armGicIntBitMaskGet
** ��������: ����ж�λ����
** �䡡��  : uiIrqID           �жϺ�
** �䡡��  : �ж�λ����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LW_INLINE UINT32  armGicIntBitMaskGet (UINT32  uiIrqID)
{
    return  (1 << armGicIntBitOffsGet(uiIrqID));
}
/*********************************************************************************************************
** ��������: armGicEnable
** ��������: ʹ�� GIC ������(ʹ�ܺ�ȫ�ͷǰ�ȫ�жϻᴫ�ݸ� CPU �ӿ�)
** �䡡��  : bEnable           �Ƿ�ʹ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicEnable (BOOL  bEnable)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();
    REGISTER UINT32  uiValue = read32((addr_t)&pDistributor->uiCTLR);

    if (bEnable) {
        /*
         * Enable both secure and non-secure.
         */
        uiValue |=   BTI_MASK_CTLR_EABBLEGRP0 | BTI_MASK_CTLR_ENABLEGRP1;

    } else {
        /*
         * Clear the enable bits.
         */
        uiValue &= ~(BTI_MASK_CTLR_EABBLEGRP0 | BTI_MASK_CTLR_ENABLEGRP1);
    }
    write32(uiValue, (addr_t)&pDistributor->uiCTLR);
}
/*********************************************************************************************************
** ��������: armGicIrqSecuritySet
** ��������: �жϰ�ȫģʽ����
** �䡡��  : uiIrqID           �жϺ�
**           bSecurity         ��ȫģʽ
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicIrqSecuritySet (UINT32  uiIrqID, BOOL  bSecurity)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();
    REGISTER UINT32  uiReg   = armGicIntRegOffsGet(uiIrqID);
    REGISTER UINT32  uiMask  = armGicIntBitMaskGet(uiIrqID);
    REGISTER UINT32  uiValue = read32((addr_t)&pDistributor->uiIGROUPRn[uiReg]);

    if (bSecurity) {
        uiValue |=  uiMask;
    } else {
        uiValue &= ~uiMask;
    }
    write32(uiValue, (addr_t)&pDistributor->uiIGROUPRn[uiReg]);
}
/*********************************************************************************************************
** ��������: armGicIrqEnable
** ��������: �ж�ʹ��
** �䡡��  : uiIrqID           �жϺ�
**           bEnable           �Ƿ�ʹ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicIrqEnable (UINT32  uiIrqID, BOOL  bEnable)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();
    REGISTER UINT32  uiReg   = armGicIntRegOffsGet(uiIrqID);
    REGISTER UINT32  uiMask  = armGicIntBitMaskGet(uiIrqID);
    REGISTER UINT32  uiValue = 0;

    /*
     * Select set-enable or clear-enable register based on enable flag.
     */
    if (bEnable) {
        uiValue |= uiMask;
        write32(uiValue, (addr_t)&pDistributor->uiISENABLERn[uiReg]);
    } else {
        uiValue |= uiMask;
        write32(uiValue, (addr_t)&pDistributor->uiICENABLERn[uiReg]);
    }
}
/*********************************************************************************************************
** ��������: armGicIrqIsEnable
** ��������: �ж��ж��Ƿ�ʹ��
** �䡡��  : uiIrqID           �жϺ�
** �䡡��  : �ж��Ƿ�ʹ��
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
BOOL  armGicIrqIsEnable (UINT32  uiIrqID)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();
    REGISTER UINT32  uiReg  = armGicIntRegOffsGet(uiIrqID);
    REGISTER UINT32  uiMask = armGicIntBitMaskGet(uiIrqID);

    return  ((read32((addr_t)&pDistributor->uiICENABLERn[uiReg]) & uiMask) ? LW_TRUE : LW_FALSE);
}
/*********************************************************************************************************
** ��������: armGicIrqPrioritySet
** ��������: �����ж����ȼ�
** �䡡��  : uiIrqID           �жϺ�
**           uiPriority        ���ȼ�(0 - 255, 0 Ϊ������ȼ�)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicIrqPrioritySet (UINT32  uiIrqID, UINT32  uiPriority)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();

    /*
     * Update the priority register. The priority registers are byte accessible, and the register
     * struct has the priority registers as a byte array, so we can just index directly by the
     * interrupt ID.
     */
    write8((uiPriority & 0xFF), (addr_t)&pDistributor->uiIPRIORITYRn[uiIrqID]);
}
/*********************************************************************************************************
** ��������: armGicIrqTargetSet
** ��������: �ж�Ŀ������
** �䡡��  : uiIrqID           �жϺ�
**           uiCpuMask         Ŀ�괦����λ��(λ 0 ���� CPU 0, �������ƣ�
**           bEnable           �Ƿ�ʹ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicIrqTargetSet (UINT32  uiIrqID, UINT32  uiCpuMask, BOOL  bEnable)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();
    REGISTER UINT8  ucValue = read8((addr_t)&pDistributor->uiITARGETSRn[uiIrqID]);

    /*
     * Like the priority registers, the target registers are byte accessible, and the register
     * struct has the them as a byte array, so we can just index directly by the
     * interrupt ID.
     */
    if (bEnable) {
        ucValue |=  ((UINT8)uiCpuMask & 0xFF);
    } else {
        ucValue &= ~((UINT8)uiCpuMask & 0xFF);
    }

    write8(ucValue, (addr_t)&pDistributor->uiITARGETSRn[uiIrqID]);
}
/*********************************************************************************************************
** ��������: armGicIrqTargetGet
** ��������: ����ж�Ŀ��
** �䡡��  : uiIrqID           �жϺ�
**           puiCpuId          Ŀ�괦����ID(��0��ʼ)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicIrqTargetGet (UINT32  uiIrqID, UINT32  *puiCpuId)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();
    REGISTER UINT8  ucValue = read8((addr_t)&pDistributor->uiITARGETSRn[uiIrqID]);

    UINT i;

    *puiCpuId = 0;

    for (i = 0; i < 8; i++) {
        if (ucValue & (1 << i)) {
            *puiCpuId = i;
            break;
        }
    }
}
/*********************************************************************************************************
** ��������: armGicSoftwareIntSend
** ��������: ��������ж�
** �䡡��  : uiIrqID           �жϺ�
**           uiOption          ѡ��
**           uiTargetList      Ŀ�괦����λ��(λ 0 ���� CPU 0, ��������,
**                             �� uiOption Ϊ GIC_SW_INT_OPTION_USE_TARGET_LIST ʱ��Ч)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicSoftwareIntSend (UINT32  uiIrqID, UINT32  uiOption, UINT32  uiTargetList)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();
    REGISTER UINT32  uiValue;

    uiValue =  (((UINT32)uiOption) << BIT_OFFS_SGIR_TARGETLISTFILTER)
             | (uiTargetList << BIT_OFFS_SGIR_CPUTARGETLIST)
             | (uiIrqID & 0xF);

    write32(uiValue, (addr_t)&pDistributor->uiSGIR);
}
/*********************************************************************************************************
** ��������: armGicCpuEnable
** ��������: ʹ�ܵ�ǰ CPU �� GIC �Ľӿ�
** �䡡��  : bEnable           �Ƿ�ʹ��
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicCpuEnable (BOOL  bEnable)
{
    REGISTER GIC_CPU_INTERFACE_REGS  *pInterface = armGicCpuInterfaceGet();
    REGISTER UINT32  uiValue = read32((addr_t)&pInterface->uiCTLR);

    if (bEnable) {
        uiValue |=   BTI_MASK_CTLR_ENABLES | BTI_MASK_CTLR_ENABLENS;
    } else {
        uiValue &= ~(BTI_MASK_CTLR_ENABLES | BTI_MASK_CTLR_ENABLENS);
    }

    write32(uiValue, (addr_t)&pInterface->uiCTLR);
}
/*********************************************************************************************************
** ��������: armGicCpuPriorityMaskSet
** ��������: ���õ�ǰ CPU �����ȼ�����
** �䡡��  : uiPriority        �ܹ����ݵ���ǰ CPU ���жϵ�������ȼ�(255 ��ʾ�����ж�)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicCpuPriorityMaskSet (UINT32  uiPriority)
{
    REGISTER GIC_CPU_INTERFACE_REGS  *pInterface = armGicCpuInterfaceGet();

    write32(uiPriority & 0xFF, (addr_t)&pInterface->uiPMR);
}
/*********************************************************************************************************
** ��������: armGicIrqReadAck
** ��������: ֪ͨ GIC ��ʼ�����ж�
** �䡡��  : NONE
** �䡡��  : ��ǰ CPU ���Դ����������ȼ��жϵ��жϺ�(1022 �� 1023 ʱ, ��ʾ������һ���ٵ��ж�)
** ȫ�ֱ���:
** ����ģ��:
**
** Normally, this function is called at the beginning of the IRQ handler. It tells the GIC
** that you are starting to handle an interupt, and returns the number of the interrupt you
** need to handle. After the interrupt is handled, you should call armGicIrqWriteDone()
** to signal that the interrupt is completely handled.
**
** In some cases, a spurious interrupt might happen. One possibility is if another CPU handles
** the interrupt. When a spurious interrupt occurs, the end of the interrupt should be indicated
** but nothing else.
**
** @return The number for the highest priority interrupt available for the calling CPU. If
**     the return value is 1022 or 1023, a spurious interrupt has occurred.
**
*********************************************************************************************************/
UINT32  armGicIrqReadAck (VOID)
{
    REGISTER GIC_CPU_INTERFACE_REGS  *pInterface = armGicCpuInterfaceGet();

    return  (read32((addr_t)&pInterface->uiIAR));
}
/*********************************************************************************************************
** ��������: armGicIrqWriteDone
** ��������: ֪ͨ GIC ��ɴ����ж�
** �䡡��  : uiIrqID           �жϺ�
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicIrqWriteDone (UINT32  uiIrqID)
{
    REGISTER GIC_CPU_INTERFACE_REGS  *pInterface = armGicCpuInterfaceGet();

    write32(uiIrqID, (addr_t)&pInterface->uiEOIR);
}
/*********************************************************************************************************
** ��������: armGicInit
** ��������: ��ʼ�� GIC
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicInit (VOID)
{
    REGISTER GIC_DISTRIBUTOR_REG  *pDistributor = armGicIntDistributorGet();
    REGISTER INT  i;

    armGicEnable(LW_FALSE);                             /*  First disable the distributor.              */

    for (i = 0; i < 32; i++) {
                                                        /*  Clear all pending interrupts.               */
        write32(~0, (addr_t)&pDistributor->uiICPENDRn[i]);
    }

    for (i = 0; i < 32; i++) {
                                                        /*  Disable all interrupts.                     */
        write32(~0, (addr_t)&pDistributor->uiICENABLERn[i]);
    }

    for (i = 0; i < 8; i++) {
                                                        /*  Set all interrupts to secure.               */
        write32(0, (addr_t)&pDistributor->uiIGROUPRn[i]);
    }

    armGicEnable(LW_TRUE);                              /*  Now enable the distributor.                 */
}
/*********************************************************************************************************
** ��������: armGicCpuInit
** ��������: ��ʼ����ǰ CPU ʹ�� GIC �ӿ�
** �䡡��  : bPreemption            �Ƿ�ʹ���ж���ռ
**           uiPriority             �ܹ����ݵ���ǰ CPU ���жϵ�������ȼ�(255 ��ʾ�����ж�)
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicCpuInit (BOOL  bPreemption,  UINT32  uiPriority)
{
    REGISTER GIC_CPU_INTERFACE_REGS  *pInterface = armGicCpuInterfaceGet();

    armGicCpuPriorityMaskSet(uiPriority);               /*  Init the GIC CPU interface.                 */

    if (bPreemption) {
        write32(0x0, (addr_t)&pInterface->uiBPR);       /*  Enable preemption.                          */
    } else {
        write32(0x7, (addr_t)&pInterface->uiBPR);       /*  Disable preemption.                         */
    }

    armGicCpuEnable(LW_TRUE);                           /*  Enable signaling the CPU.                   */
}
/*********************************************************************************************************
** ��������: armGicIntVecterEnable
** ��������: ʹ��ָ�����ж�����
** ��  ��  : ulVector         ����
**           bSecurity        ��ȫģʽ
**           ulPriority       ���ȼ�
**           uiCpuMask        Ŀ�괦����λ��(λ 0 ���� CPU 0, �������ƣ�
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicIntVecterEnable (ULONG  ulVector,  BOOL  bSecurity, ULONG  ulPriority, ULONG  uiCpuMask)
{
    armGicIrqEnable(ulVector, LW_FALSE);

    armGicIrqSecuritySet(ulVector, bSecurity);

    armGicIrqPrioritySet(ulVector, ulPriority);

    armGicIrqTargetSet(ulVector, 0xFF, LW_FALSE);

    armGicIrqTargetSet(ulVector, uiCpuMask, LW_TRUE);

    armGicIrqEnable(ulVector, LW_TRUE);
}
/*********************************************************************************************************
** ��������: armGicIntVecterDisable
** ��������: ����ָ�����ж�����
** ��  ��  : ulVector         ����
** ��  ��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID  armGicIntVecterDisable (ULONG  ulVector)
{
    armGicIrqEnable(ulVector, LW_FALSE);

    armGicIrqTargetSet(ulVector, 0xFF, LW_FALSE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
