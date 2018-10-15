/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                SylixOS(TM)  LW : long wing
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: irq_numbers.h
**
** 创   建   人: Jiao.JinXing(焦进星)
**
** 文件创建日期: 2013 年 12 月 12 日
**
** 描        述: 通用中断控制器的中断编号
*********************************************************************************************************/
#ifndef __IRQ_NUMBERS_H__
#define __IRQ_NUMBERS_H__

#define SW_INTERRUPT_0                0                                 /* Software interrupt 0.        */
#define SW_INTERRUPT_1                1                                 /* Software interrupt 1.        */
#define SW_INTERRUPT_2                2                                 /* Software interrupt 2.        */
#define SW_INTERRUPT_3                3                                 /* Software interrupt 3.        */
#define SW_INTERRUPT_4                4                                 /* Software interrupt 4.        */
#define SW_INTERRUPT_5                5                                 /* Software interrupt 5.        */
#define SW_INTERRUPT_6                6                                 /* Software interrupt 6.        */
#define SW_INTERRUPT_7                7                                 /* Software interrupt 7.        */
#define SW_INTERRUPT_8                8                                 /* Software interrupt 8.        */
#define SW_INTERRUPT_9                9                                 /* Software interrupt 9.        */
#define SW_INTERRUPT_10               10                                /* Software interrupt 10.       */
#define SW_INTERRUPT_11               11                                /* Software interrupt 11.       */
#define SW_INTERRUPT_12               12                                /* Software interrupt 12.       */
#define SW_INTERRUPT_13               13                                /* Software interrupt 13.       */
#define SW_INTERRUPT_14               14                                /* Software interrupt 14.       */
#define SW_INTERRUPT_15               15                                /* Software interrupt 15.       */
#define RSVD_INTERRUPT_16             16                                /* Reserved.                    */
#define RSVD_INTERRUPT_17             17                                /* Reserved.                    */
#define RSVD_INTERRUPT_18             18                                /* Reserved.                    */
#define RSVD_INTERRUPT_19             19                                /* Reserved.                    */
#define RSVD_INTERRUPT_20             20                                /* Reserved.                    */
#define RSVD_INTERRUPT_21             21                                /* Reserved.                    */
#define RSVD_INTERRUPT_22             22                                /* Reserved.                    */
#define RSVD_INTERRUPT_23             23                                /* Reserved.                    */
#define RSVD_INTERRUPT_24             24                                /* Reserved.                    */
#define RSVD_INTERRUPT_25             25                                /* Reserved.                    */
#define RSVD_INTERRUPT_26             26                                /* Reserved.                    */
#define RSVD_INTERRUPT_27             27                                /* Reserved.                    */
#define RSVD_INTERRUPT_28             28                                /* Reserved.                    */
#define RSVD_INTERRUPT_29             29                                /* Reserved.                    */
#define RSVD_INTERRUPT_30             30                                /* Reserved.                    */
#define RSVD_INTERRUPT_31             31                                /* Reserved.                    */
#define IMX_INT_IOMUXC_GPR            32                                /* General Purpose Register 1 fr*/
#define IMX_INT_CHEETAH_CSYSPWRUPREQ  33                                /* Listed as DAP in RM          */
#define IMX_INT_SDMA                  34                                /* Logical OR of all 48 SDMA int*/
#define IMX_INT_VPU_JPG               35                                /* JPEG codec interrupt request.*/
#define IMX_INT_SNVS_LP_SET_PWR_OFF   36                                /* PMIC power off request.      */
#define IMX_INT_IPU1_ERR              37                                /* IPU1 error interrupt request.*/
#define IMX_INT_IPU1_FUNC             38                                /* IPU1 sync interrupt request. */
#define IMX_INT_IPU2_ERR              39                                /* IPU2 error interrupt request.*/
#define IMX_INT_IPU2_FUNC             40                                /* IPU2 sync interrupt request. */
#define IMX_INT_GPU3D                 41                                /* GPU3D interrupt request.     */
#define IMX_INT_GPU2D                 42                                /* Idle interrupt from GPU2D (fo*/
#define IMX_INT_OPENVG_XAQ2           43                                /* GPU2D general interrupt reque*/
#define IMX_INT_VPU_IPI               44                                /* VPU interrupt request.       */
#define IMX_INT_APBHDMA               45                                /* Logical OR of 4 signals: dma_*/
#define IMX_INT_EIM                   46                                /* EIM interrupt request.       */
#define IMX_INT_BCH                   47                                /* BCH operation complete interr*/
#define IMX_INT_GPMI                  48                                /* GPMI operation timeout error */
#define IMX_INT_DTCP                  49                                /* DTCP interrupt request.      */
#define IMX_INT_VDOA                  50                                /* Logical OR of VDOA interrupt */
#define IMX_INT_SNVS                  51                                /* SNVS consolidated interrupt. */
#define IMX_INT_SNVS_SEC              52                                /* SNVS security interrupt.     */
#define IMX_INT_CSU                   53                                /* CSU interrupt request 1. Indi*/
#define IMX_INT_USDHC1                54                                /* uSDHC1 (Enhanced SDHC) interr*/
#define IMX_INT_USDHC2                55                                /* uSDHC2 (Enhanced SDHC) interr*/
#define IMX_INT_USDHC3                56                                /* uSDHC3 (Enhanced SDHC) interr*/
#define IMX_INT_USDHC4                57                                /* uSDHC4 (Enhanced SDHC) interr*/
#define IMX_INT_UART1                 58                                /* Logical OR of UART1 interrupt*/
#define IMX_INT_UART2                 59                                /* Logical OR of UART2 interrupt*/
#define IMX_INT_UART3                 60                                /* Logical OR of UART3 interrupt*/
#define IMX_INT_UART4                 61                                /* Logical OR of UART4 interrupt*/
#define IMX_INT_UART5                 62                                /* Logical OR of UART5 interrupt*/
#define IMX_INT_ECSPI1                63                                /* eCSPI1 interrupt request.    */
#define IMX_INT_ECSPI2                64                                /* eCSPI2 interrupt request.    */
#define IMX_INT_ECSPI3                65                                /* eCSPI3 interrupt request.    */
#define IMX_INT_ECSPI4                66                                /* eCSPI4 interrupt request.    */
#define IMX_INT_ECSPI5                67                                /* eCSPI5 interrupt request.    */
#define IMX_INT_I2C1                  68                                /* I2C1 interrupt request.      */
#define IMX_INT_I2C2                  69                                /* I2C2 interrupt request.      */
#define IMX_INT_I2C3                  70                                /* I2C3 interrupt request.      */
#define IMX_INT_SATA                  71                                /* SATA interrupt request.      */
#define IMX_INT_USBOH3_UH1            72                                /* USB Host 1 interrupt request.*/
#define IMX_INT_USBOH3_UH2            73                                /* USB Host 2 interrupt request.*/
#define IMX_INT_USBOH3_UH3            74                                /* USB Host 3 interrupt request.*/
#define IMX_INT_USBOH3_UOTG           75                                /* USB OTG interrupt request.   */
#define IMX_INT_USB_UTMI0             76                                /* UTMI0 interrupt request.     */
#define IMX_INT_USB_UTMI1             77                                /* UTMI1 interrupt request.     */
#define IMX_INT_SSI1                  78                                /* SSI1 interrupt request.      */
#define IMX_INT_SSI2                  79                                /* SSI2 interrupt request.      */
#define IMX_INT_SSI3                  80                                /* SSI3 interrupt request.      */
#define IMX_INT_TEMPERATURE           81                                /* Temperature Sensor (temp. gre*/
#define IMX_INT_ASRC                  82                                /* ASRC interrupt request.      */
#define IMX_INT_ESAI                  83                                /* ESAI interrupt request.      */
#define IMX_INT_SPDIF                 84                                /* Logical OR of SPDIF TX and SP*/
#define IMX_INT_MLB                   85                                /* MLB error interrupt request. */
#define IMX_INT_PMU_ANA_BO            86                                /* PMU analog regulator brown-ou*/
#define IMX_INT_GPT                   87                                /* Logical OR of GPT rollover in*/
#define IMX_INT_EPIT1                 88                                /* EPIT1 output compare interrup*/
#define IMX_INT_EPIT2                 89                                /* EPIT2 output compare interrup*/
#define IMX_INT_GPIO1_INT7            90                                /* INT7 interrupt request.      */
#define IMX_INT_GPIO1_INT6            91                                /* INT6 interrupt request.      */
#define IMX_INT_GPIO1_INT5            92                                /* INT5 interrupt request.      */
#define IMX_INT_GPIO1_INT4            93                                /* INT4 interrupt request.      */
#define IMX_INT_GPIO1_INT3            94                                /* INT3 interrupt request.      */
#define IMX_INT_GPIO1_INT2            95                                /* INT2 interrupt request.      */
#define IMX_INT_GPIO1_INT1            96                                /* INT1 interrupt request.      */
#define IMX_INT_GPIO1_INT0            97                                /* INT0 interrupt request.      */
#define IMX_INT_GPIO1_INT15_0         98                                /* interrupt for GPIO1  0 - 15. */
#define IMX_INT_GPIO1_INT31_16        99                                /* interrupt for GPIO1  16 - 31.*/
#define IMX_INT_GPIO2_INT15_0         100                               /* interrupt for GPIO2  0 - 15. */
#define IMX_INT_GPIO2_INT31_16        101                               /* interrupt for GPIO2  16 - 31.*/
#define IMX_INT_GPIO3_INT15_0         102                               /* interrupt for GPIO3  0 - 15. */
#define IMX_INT_GPIO3_INT31_16        103                               /* interrupt for GPIO3  16 - 31.*/
#define IMX_INT_GPIO4_INT15_0         104                               /* interrupt for GPIO4  0 - 15. */
#define IMX_INT_GPIO4_INT31_16        105                               /* interrupt for GPIO4  16 - 31.*/
#define IMX_INT_GPIO5_INT15_0         106                               /* interrupt for GPIO5  0 - 15. */
#define IMX_INT_GPIO5_INT31_16        107                               /* interrupt for GPIO5  16 - 31.*/
#define IMX_INT_GPIO6_INT15_0         108                               /* interrupt for GPIO6  0 - 15. */
#define IMX_INT_GPIO6_INT31_16        109                               /* interrupt for GPIO6  16 - 31.*/
#define IMX_INT_GPIO7_INT15_0         110                               /* interrupt for GPIO7  0 - 15. */
#define IMX_INT_GPIO7_INT31_16        111                               /* interrupt for GPIO7  16 - 31.*/
#define IMX_INT_WDOG1                 112                               /* WDOG1 timer reset interrupt  */
#define IMX_INT_WDOG2                 113                               /* WDOG2 timer reset interrupt  */
#define IMX_INT_KPP                   114                               /* Key Pad interrupt request.   */
#define IMX_INT_PWM1                  115                               /* Cumulative interrupt for PWM1*/
#define IMX_INT_PWM2                  116                               /* Cumulative interrupt for PWM2*/
#define IMX_INT_PWM3                  117                               /* Cumulative interrupt for PWM3*/
#define IMX_INT_PWM4                  118                               /* Cumulative interrupt for PWM4*/
#define IMX_INT_CCM_INT1              119                               /* CCM interrupt request 1.     */
#define IMX_INT_CCM_INT2              120                               /* CCM interrupt request 2.     */
#define IMX_INT_GPC_INT1              121                               /* GPC interrupt request 1.     */
#define IMX_INT_GPC_INT2              122                               /* GPC interrupt request 2.     */
#define IMX_INT_SRC                   123                               /* SRC interrupt request.       */
#define IMX_INT_CHEETAH_L2            124                               /* Logical OR of all L2 interrup*/
#define IMX_INT_CHEETAH_PARITY        125                               /* Parity Check error interrupt */
#define IMX_INT_CHEETAH_PERFORM       126                               /* Logical OR of Performance Uni*/
#define IMX_INT_CHEETAH_TRIGGER       127                               /* Logical OR of CTI trigger out*/
#define IMX_INT_SRC_CPU_WDOG          128                               /* Combined CPU wdog interrupts */
#define IMX_INT_INTERRUPT_129         129                               /* Reserved.                    */
#define IMX_INT_INTERRUPT_130         130                               /* Reserved.                    */
#define IMX_INT_INTERRUPT_131         131                               /* Reserved.                    */
#define IMX_INT_CSI_INTR1             132                               /* MIPI CSI interrupt request 1.*/
#define IMX_INT_CSI_INTR2             133                               /* MIPI CSI interrupt request 2.*/
#define IMX_INT_DSI                   134                               /* MIPI DSI interrupt request.  */
#define IMX_INT_HSI                   135                               /* MIPI HSI interrupt request.  */
#define IMX_INT_SJC                   136                               /* SJC interrupt GP reg         */
#define IMX_INT_CAAM_INT0             137                               /* CAAM job ring 0 interrupt.   */
#define IMX_INT_CAAM_INT1             138                               /* CAAM job ring 1 interrupt.   */
#define IMX_INT_INTERRUPT_139         139                               /* Reserved.                    */
#define IMX_INT_TZASC1                140                               /* ASC1 interrupt request.      */
#define IMX_INT_TZASC2                141                               /* ASC2 interrupt request.      */
#define IMX_INT_FLEXCAN1              142                               /* FLEXCAN1 combined interrupt. */
#define IMX_INT_FLEXCAN2              143                               /* FLEXCAN2 combined interrupt. */
#define IMX_INT_PERFMON1              144                               /* Reserved.                    */
#define IMX_INT_PERFMON2              145                               /* Reserved.                    */
#define IMX_INT_PERFMON3              146                               /* Reserved.                    */
#define IMX_INT_HDMI_TX               147                               /* HDMI master interrupt request*/
#define IMX_INT_HDMI_TX_WAKEUP        148                               /* HDMI CEC engine dedicated int*/
#define IMX_INT_MLB_AHB0              149                               /* Channels [31:0] interrupt req*/
#define IMX_INT_ENET                  150                               /* MAC 0 IRQ, Logical OR of:    */
#define IMX_INT_ENET_1588             151                               /* 锟糓AC 0 1588 Timer interrupt*/
#define IMX_INT_PCIE_1                152                               /* PCIe interrupt request 1.    */
#define IMX_INT_PCIE_2                153                               /* PCIe interrupt request 2.    */
#define IMX_INT_PCIE_3                154                               /* PCIe interrupt request 3.    */
#define IMX_INT_PCIE_4                155                               /* PCIe interrupt request 4.    */
#define IMX_INT_DCIC1                 156                               /* Logical OR of DCIC1 interrupt*/
#define IMX_INT_DCIC2                 157                               /* Logical OR of DCIC2 interrupt*/
#define IMX_INT_MLB_AHB1              158                               /* Logical OR of channel[63:32] */
#define IMX_INT_PMU_DIG_BO            159                               /* PMU digital regulator brown-o*/
#define IMX_INTERRUPT_COUNT           160                               /* Total number of interrupts.  */

#endif                                                                  /* __IRQ_NUMBERS_H__            */
/*********************************************************************************************************
  END
*********************************************************************************************************/
