#define __SYLIXOS_KERNEL
#include "config.h"
#include <linux/compat.h>
#include "SylixOS.h"

#include "pinmux/iomux_config.h"
#include "regsgpio.h"
/*
 * First subscript is bank, second is pin within the bank. There are always 32 pin
 * entries per bank. If a pin does not have an assigned GPIO, its address is 0.
 */
const UINT32 GpioPinmuxReg[HW_GPIO_INSTANCE_COUNT][GPIO_PIN_COUNT] =
    {
        /* Bank 1 */
        {
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO00_ADDR,                       /*  Pin 0                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO01_ADDR,                       /*  Pin 1                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO02_ADDR,                       /*  Pin 2                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO03_ADDR,                       /*  Pin 3                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO04_ADDR,                       /*  Pin 4                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO05_ADDR,                       /*  Pin 5                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO06_ADDR,                       /*  Pin 6                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO07_ADDR,                       /*  Pin 7                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO08_ADDR,                       /*  Pin 8                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO09_ADDR,                       /*  Pin 9                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD2_CLK_ADDR,                      /*  Pin 10                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD2_CMD_ADDR,                      /*  Pin 11                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA3_ADDR,                    /*  Pin 12                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA2_ADDR,                    /*  Pin 13                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA1_ADDR,                    /*  Pin 14                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD2_DATA0_ADDR,                    /*  Pin 15                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA0_ADDR,                    /*  Pin 16                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA1_ADDR,                    /*  Pin 17                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD1_CMD_ADDR,                      /*  Pin 18                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA2_ADDR,                    /*  Pin 19                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD1_CLK_ADDR,                      /*  Pin 20                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD1_DATA3_ADDR,                    /*  Pin 21                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_MDIO_ADDR,                    /*  Pin 22                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_REF_CLK_ADDR,                 /*  Pin 23                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_RX_ER_ADDR,                   /*  Pin 24                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_CRS_DV_ADDR,                  /*  Pin 25                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_RX_DATA1_ADDR,                /*  Pin 26                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_RX_DATA0_ADDR,                /*  Pin 27                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_TX_EN_ADDR,                   /*  Pin 28                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_TX_DATA1_ADDR,                /*  Pin 29                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_TX_DATA0_ADDR,                /*  Pin 30                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_ENET_MDC_ADDR,                     /*  Pin 31                      */
        }, {        /* Bank 2 */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA00_ADDR,                  /*  Pin 0                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA01_ADDR,                  /*  Pin 1                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA02_ADDR,                  /*  Pin 2                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA03_ADDR,                  /*  Pin 3                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA04_ADDR,                  /*  Pin 4                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA05_ADDR,                  /*  Pin 5                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA06_ADDR,                  /*  Pin 6                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_DATA07_ADDR,                  /*  Pin 7                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA0_ADDR,                    /*  Pin 8                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA1_ADDR,                    /*  Pin 9                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA2_ADDR,                    /*  Pin 10                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA3_ADDR,                    /*  Pin 11                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA4_ADDR,                    /*  Pin 12                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA5_ADDR,                    /*  Pin 13                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA6_ADDR,                    /*  Pin 14                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_DATA7_ADDR,                    /*  Pin 15                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR22_ADDR,                   /*  Pin 16                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR21_ADDR,                   /*  Pin 17                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR20_ADDR,                   /*  Pin 18                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR19_ADDR,                   /*  Pin 19                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR18_ADDR,                   /*  Pin 20                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR17_ADDR,                   /*  Pin 21                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR16_ADDR,                   /*  Pin 22                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_CS0_ADDR,                      /*  Pin 23                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_CS1_ADDR,                      /*  Pin 24                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_OE_ADDR,                       /*  Pin 25                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_RW_ADDR,                       /*  Pin 26                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_LBA_ADDR,                      /*  Pin 27                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB0_ADDR,                      /*  Pin 28                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB1_ADDR,                      /*  Pin 29                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB2_ADDR,                      /*  Pin 30                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_EB3_ADDR,                      /*  Pin 31                      */
        }, {   /* Bank 3 */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD00_ADDR,                     /*  Pin 0                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD01_ADDR,                     /*  Pin 1                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD02_ADDR,                     /*  Pin 2                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD03_ADDR,                     /*  Pin 3                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD04_ADDR,                     /*  Pin 4                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD05_ADDR,                     /*  Pin 5                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD06_ADDR,                     /*  Pin 6                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD07_ADDR,                     /*  Pin 7                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD08_ADDR,                     /*  Pin 8                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD09_ADDR,                     /*  Pin 9                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD10_ADDR,                     /*  Pin 10                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD11_ADDR,                     /*  Pin 11                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD12_ADDR,                     /*  Pin 12                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD13_ADDR,                     /*  Pin 13                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD14_ADDR,                     /*  Pin 14                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_AD15_ADDR,                     /*  Pin 15                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA16_ADDR,                   /*  Pin 16                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA17_ADDR,                   /*  Pin 17                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA18_ADDR,                   /*  Pin 18                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA19_ADDR,                   /*  Pin 19                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA20_ADDR,                   /*  Pin 20                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA21_ADDR,                   /*  Pin 21                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA22_ADDR,                   /*  Pin 22                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA23_ADDR,                   /*  Pin 23                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA24_ADDR,                   /*  Pin 24                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA25_ADDR,                   /*  Pin 25                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA26_ADDR,                   /*  Pin 26                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA27_ADDR,                   /*  Pin 27                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA28_ADDR,                   /*  Pin 28                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA29_ADDR,                   /*  Pin 29                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA30_ADDR,                   /*  Pin 30                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_DATA31_ADDR,                   /*  Pin 31                      */
        }, {    /* Bank 4 */
            0,
            0,
            0,
            0,
            0,
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO19_ADDR,                       /*  Pin 5                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL0_ADDR,                     /*  Pin 6                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW0_ADDR,                     /*  Pin 7                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL1_ADDR,                     /*  Pin 8                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW1_ADDR,                     /*  Pin 9                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL2_ADDR,                     /*  Pin 10                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW2_ADDR,                     /*  Pin 11                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL3_ADDR,                     /*  Pin 12                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW3_ADDR,                     /*  Pin 13                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_COL4_ADDR,                     /*  Pin 14                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_KEY_ROW4_ADDR,                     /*  Pin 15                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DI0_DISP_CLK_ADDR,                 /*  Pin 16                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN15_ADDR,                    /*  Pin 17                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN02_ADDR,                    /*  Pin 18                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN03_ADDR,                    /*  Pin 19                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DI0_PIN04_ADDR,                    /*  Pin 20                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA00_ADDR,                 /*  Pin 21                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA01_ADDR,                 /*  Pin 22                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA02_ADDR,                 /*  Pin 23                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA03_ADDR,                 /*  Pin 24                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA04_ADDR,                 /*  Pin 25                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA05_ADDR,                 /*  Pin 26                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA06_ADDR,                 /*  Pin 27                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA07_ADDR,                 /*  Pin 28                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA08_ADDR,                 /*  Pin 29                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA09_ADDR,                 /*  Pin 30                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA10_ADDR,                 /*  Pin 31                      */
        }, {     /* Bank 5 */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_WAIT_ADDR,                     /*  Pin 0                       */
            0,                                                          /*  Unassigned GPIO pin 1       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR25_ADDR,                   /*  Pin 2                       */
            0,                                                          /*  Unassigned GPIO pin 3       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR24_ADDR,                   /*  Pin 4                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA11_ADDR,                 /*  Pin 5                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA12_ADDR,                 /*  Pin 6                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA13_ADDR,                 /*  Pin 7                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA14_ADDR,                 /*  Pin 8                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA15_ADDR,                 /*  Pin 9                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA16_ADDR,                 /*  Pin 10                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA17_ADDR,                 /*  Pin 11                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA18_ADDR,                 /*  Pin 12                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA19_ADDR,                 /*  Pin 13                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA20_ADDR,                 /*  Pin 14                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA21_ADDR,                 /*  Pin 15                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA22_ADDR,                 /*  Pin 16                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_DISP0_DATA23_ADDR,                 /*  Pin 17                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_PIXCLK_ADDR,                  /*  Pin 18                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_HSYNC_ADDR,                   /*  Pin 19                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA_EN_ADDR,                 /*  Pin 20                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_VSYNC_ADDR,                   /*  Pin 21                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA04_ADDR,                  /*  Pin 22                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA05_ADDR,                  /*  Pin 23                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA06_ADDR,                  /*  Pin 24                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA07_ADDR,                  /*  Pin 25                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA08_ADDR,                  /*  Pin 26                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA09_ADDR,                  /*  Pin 27                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA10_ADDR,                  /*  Pin 28                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA11_ADDR,                  /*  Pin 29                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA12_ADDR,                  /*  Pin 30                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA13_ADDR,                  /*  Pin 31                      */
        }, {    /* Bank 6 */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA14_ADDR,                  /*  Pin 0                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA15_ADDR,                  /*  Pin 1                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA16_ADDR,                  /*  Pin 2                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA17_ADDR,                  /*  Pin 3                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA18_ADDR,                  /*  Pin 4                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_CSI0_DATA19_ADDR,                  /*  Pin 5                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_ADDR23_ADDR,                   /*  Pin 6                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CLE_ADDR,                     /*  Pin 7                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_ALE_ADDR,                     /*  Pin 8                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_WP_B_ADDR,                    /*  Pin 9                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_READY_ADDR,                   /*  Pin 10                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CS0_B_ADDR,                   /*  Pin 11                      */
            0,                                                          /*  Unassigned GPIO pin 12      */
            0,                                                          /*  Unassigned GPIO pin 13      */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CS1_B_ADDR,                   /*  Pin 14                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CS2_B_ADDR,                   /*  Pin 15                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_NAND_CS3_B_ADDR,                   /*  Pin 16                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA7_ADDR,                    /*  Pin 17                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA6_ADDR,                    /*  Pin 18                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TXC_ADDR,                    /*  Pin 19                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD0_ADDR,                    /*  Pin 20                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD1_ADDR,                    /*  Pin 21                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD2_ADDR,                    /*  Pin 22                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TD3_ADDR,                    /*  Pin 23                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RX_CTL_ADDR,                 /*  Pin 24                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD0_ADDR,                    /*  Pin 25                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_TX_CTL_ADDR,                 /*  Pin 26                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD1_ADDR,                    /*  Pin 27                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD2_ADDR,                    /*  Pin 28                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RD3_ADDR,                    /*  Pin 29                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_RGMII_RXC_ADDR,                    /*  Pin 30                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_EIM_BCLK_ADDR,                     /*  Pin 31                      */
        }, {   /* Bank 7 */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA5_ADDR,                    /*  Pin 0                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA4_ADDR,                    /*  Pin 1                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CMD_ADDR,                      /*  Pin 2                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_CLK_ADDR,                      /*  Pin 3                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA0_ADDR,                    /*  Pin 4                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA1_ADDR,                    /*  Pin 5                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA2_ADDR,                    /*  Pin 6                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_DATA3_ADDR,                    /*  Pin 7                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD3_RESET_ADDR,                    /*  Pin 8                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CMD_ADDR,                      /*  Pin 9                       */
            HW_IOMUXC_SW_MUX_CTL_PAD_SD4_CLK_ADDR,                      /*  Pin 10                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO16_ADDR,                       /*  Pin 11                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO17_ADDR,                       /*  Pin 12                      */
            HW_IOMUXC_SW_MUX_CTL_PAD_GPIO18_ADDR,                       /*  Pin 13                      */
            0,                                                          /*  Unassigned GPIO pin 14      */
            0,                                                          /*  Unassigned GPIO pin 15      */
            0,                                                          /*  Unassigned GPIO pin 16      */
            0,                                                          /*  Unassigned GPIO pin 17      */
            0,                                                          /*  Unassigned GPIO pin 18      */
            0,                                                          /*  Unassigned GPIO pin 19      */
            0,                                                          /*  Unassigned GPIO pin 20      */
            0,                                                          /*  Unassigned GPIO pin 21      */
            0,                                                          /*  Unassigned GPIO pin 22      */
            0,                                                          /*  Unassigned GPIO pin 23      */
            0,                                                          /*  Unassigned GPIO pin 24      */
            0,                                                          /*  Unassigned GPIO pin 25      */
            0,                                                          /*  Unassigned GPIO pin 26      */
            0,                                                          /*  Unassigned GPIO pin 27      */
            0,                                                          /*  Unassigned GPIO pin 28      */
            0,                                                          /*  Unassigned GPIO pin 29      */
            0,                                                          /*  Unassigned GPIO pin 30      */
            0,                                                          /*  Unassigned GPIO pin 31      */
        },
    };
/*********************************************************************************************************
** ��������: GpioPinmuxSet
** ��������: ���� GPIO �ܽŵĹ���ѡ��
** ��  ��  : uiPinGroup  GPIO �ķ���
**           uiPinNum    GPIO �ڱ����ڵı�� 0��31
**           uiCfg       �ܽŹ�������
** ��  ��  : 0: ��ȷ -1:����
** ȫ�ֱ���:
** ����ģ��:
**           12.15 �� �޸���GpioPinmuxReg[uiPinGroup - 1][uiPinNum]ΪGpioPinmuxReg[uiPinGroup][uiPinNum]
**           Ϊ�˷�������gpio �ļĴ����鶨��淶����������ʼ�������������ֲ�����Ϊ����ʼ�������ӵ�gpio��
**           �����Щ��������ܽŵĳɹ����ɺϡ�
*********************************************************************************************************/
UINT32  GpioPinmuxSet (UINT32  uiPinGroup, UINT32 uiPinNum, UINT32 uiCfg)
{
    addr_t atAddr = GpioPinmuxReg[uiPinGroup][uiPinNum];

    if(0 == atAddr) {
    	return PX_ERROR;
    }
    writel(uiCfg, atAddr);                                              /*  GPIO ���ܾ�Ϊ ALT5          */

    return  ERROR_NONE;
}
/*********************************************************************************************************
  END
*********************************************************************************************************/