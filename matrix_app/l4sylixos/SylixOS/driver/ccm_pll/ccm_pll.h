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
** ��   ��   ��: ccm_pll.h
**
** ��   ��   ��: Zhang.Xu (����)
**
** �ļ���������: 2015 �� 12 �� 1 ��
**
** ��        ��: I.MX6Q ������ PLL ʱ�����ú�״̬��ȡ�������� ͷ�ļ�
*********************************************************************************************************/
#ifndef _CCM_PLL_H_
#define _CCM_PLL_H_

/*********************************************************************************************************
** ϵͳʱ����غ궨��
*********************************************************************************************************/
#define  CLK_SRC_32K          32768
#define  REGS_SATA_BASE       (0x02200000)
/*
 * Create a clock gate bit mask value. x 0..15, for CG0 to CG15
 */
#define CG(x)                 (3 << ((x) * 2))
/*
 * Constants for CCM CCGR register fields.
 */
enum _clock_gate_constants
{
    CLOCK_ON     = 0x3,                                                 /*  Clk  on in both run & stop  */
    CLOCK_ON_RUN = 0x1,                                                 /*  Clock on only in run mode   */
    CLOCK_OFF    = 0x0                                                  /*  Clocked gated off           */
};
/*
 * Low power mdoes.
 */
typedef enum _lp_modes {
    RUN_MODE,
    WAIT_MODE,
    STOP_MODE,
} lp_modes_t;
/*
 * brief Main clock sources,ԭ�����ʹ�� enum ʵ�֣�����ʹ�ú궨��ʵ��
 */
#define CPU_CLK              0
#define AXI_CLK              1
#define MMDC_CH0_AXI_CLK     2
#define AHB_CLK              3
#define IPG_CLK              4
#define IPG_PER_CLK          5
#define MMDC_CH1_AXI_CLK     6
/*
 * Peripheral clocks.
 */
typedef enum _peri_clocks {
    UART1_MODULE_CLK,
    UART2_MODULE_CLK,
    UART3_MODULE_CLK,
    UART4_MODULE_CLK,
    SSI1_BAUD,
    SSI2_BAUD,
    CSI_BAUD,
    MSTICK1_CLK,
    MSTICK2_CLK,
    RAWNAND_CLK,
    USB_CLK,
    VPU_CLK,
    SPI_CLK,
    CAN_CLK,
    EIM_CLK,
} peri_clocks_t;

/*
 * Available PLLs.
 */
typedef enum plls {
    PLL1,
    PLL2,
    PLL3,
    PLL4,
    PLL5,
} plls_t;
/*********************************************************************************************************
** ϵͳʱ��ȫ�ֱ���
*********************************************************************************************************/
extern const UINT32 PLL1_OUTPUT;
extern const UINT32 PLL2_OUTPUT[];
extern const UINT32 PLL3_OUTPUT[];
extern const UINT32 PLL4_OUTPUT;
extern const UINT32 PLL5_OUTPUT;

/*********************************************************************************************************
  imx6q ccm_pll driver functions
*********************************************************************************************************/
VOID    imx6qClkGateSet(addr_t atBaseAddr, UINT32 uiGateMode);
UINT32  imx6qMainClkGet(INT iClk);
UINT32  imx6qPeriClkGet(peri_clocks_t clk);
VOID    imx6qCcmPLLInit(VOID);
VOID    imx6qCcmLowPower(lp_modes_t lp_mode);
VOID    imx6qLpmWakeUpSrc(UINT32 uiIrqNum, BOOL bEnable);
VOID    imx6qClkGateSetOnAll (VOID);

#endif
/*********************************************************************************************************
  END
*********************************************************************************************************/
