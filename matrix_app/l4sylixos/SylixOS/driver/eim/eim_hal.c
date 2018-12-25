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
** ��   ��   ��: eimHal.c
**
** ��   ��   ��: Xu.GuiZhou (�����)
**
** �ļ���������: 2016 �� 09 �� 13 ��
**
** ��        ��: EIM����Ӳ����ز�ӿ�ʵ��
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include "stdlib.h"
#include "string.h"

#include "sdk.h"
#include "eim.h"

/*********************************************************************************************************
** ��������: axi_clock_show
** ��������: ��ʾ EIM �豸AXI��ʱ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
void axi_clock_show()
{
    printk(KERN_INFO "AXI_CLK_ROOT --> ");
    switch (HW_CCM_CBCDR.B.AXI_SEL) {
    case 0:                                                             /* AXI_CLK_ROOT                 */
        printk(KERN_INFO "PERIPH_CLK\n");
        break;
    case 1:
    {
        if(HW_CCM_CBCDR.B.AXI_ALT_SEL == 0) {
            printk(KERN_INFO "PLL2_PFD2\n");
        }
        else {
            printk(KERN_INFO "PLL3_PFD1\n");
        }
        break;
    }
    default :
        printk(KERN_INFO "UNKOWN SOURCE\n");
        break;
    }

    printk(KERN_INFO "AXI_SEL:               %8x \n", HW_CCM_CBCDR.B.AXI_SEL);
    printk(KERN_INFO "AXI_PODF:              %8x \n", HW_CCM_CBCDR.B.AXI_PODF);
    printk(KERN_INFO "AXI_ALT_SEL:           %8x \n", HW_CCM_CBCDR.B.AXI_ALT_SEL);
}
/*********************************************************************************************************
** ��������: eimShowClock
** ��������: ��ʾ EIM �豸ʱ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
void eimShowClock()
{
    printk(KERN_INFO "ACLK_EIM_SLOW_CLK_ROOT --> ");
    switch (HW_CCM_CSCMR1.B.ACLK_EIM_SLOW_SEL) {
    case 0:                                                             /* IPU1_DI0_PRE_CLK_SEL         */
        axi_clock_show();
        break;
    case 1:
        printk(KERN_INFO "pll3_sw_clk\n");
        break;
    case 2:
        printk(KERN_INFO "PLL2 PFD2\n");
        break;
    case 3:
        printk(KERN_INFO "PLL2 PFD0\n");
        break;
    default :
        printk(KERN_INFO "UNKOWN SOURCE\n");
        break;
    }
}
/*********************************************************************************************************
** ��������: eimClockInit
** ��������: ��ʼ�� EIM �豸ʱ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
void eimClockInit()
{
    imx6qClkGateSet(REGS_EIM_BASE, CLOCK_ON);
    printk(KERN_INFO "EIM clock = %d MHz\n", imx6qPeriClkGet(EIM_CLK) / 1000000);
}
/*********************************************************************************************************
** ��������: eimClockInit
** ��������: ��ʼ�� EIM �豸ʱ��
** �䡡��  : pCtrl    EIM�����豸
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
void eimTimeingConfig(PEIM_CTRL pCtrl)
{
    /*
     * config CS0 timing
     */
    writel(0x0011300F, pCtrl->EIM_uiRegBase + 0x00);
    writel(0x00001001, pCtrl->EIM_uiRegBase + 0x04);
    writel(0x07010000, pCtrl->EIM_uiRegBase + 0x08);
    writel(0x00000000, pCtrl->EIM_uiRegBase + 0x0C);
    writel(0x06040000, pCtrl->EIM_uiRegBase + 0x10);
    writel(0x00000000, pCtrl->EIM_uiRegBase + 0x14);

    /*
     * config CS1 timing
     */
    writel(0x0011300F, pCtrl->EIM_uiRegBase + 0x18);
    writel(0x00001001, pCtrl->EIM_uiRegBase + 0x1C);
    writel(0x07010000, pCtrl->EIM_uiRegBase + 0x20);
    writel(0x00000000, pCtrl->EIM_uiRegBase + 0x24);
    writel(0x06040000, pCtrl->EIM_uiRegBase + 0x28);
    writel(0x00000000, pCtrl->EIM_uiRegBase + 0x2C);

    writel(0x00000021, pCtrl->EIM_uiRegBase + 0x90);
    writel(0x00000010, pCtrl->EIM_uiRegBase + 0x94);
    writel(0x00000000, pCtrl->EIM_uiRegBase + 0x98);
}

/*********************************************************************************************************
** ��������: eimHwInit
** ��������: ��ʼ�� EIM �豸Ӳ����ع���
** �䡡��  : pCtrl    EIM�����豸
** �䡡��  : NONE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
VOID eimHwInit(PEIM_CTRL pCtrl)
{
    pCtrl->EIM_uiRegBase    = REGS_EIM_BASE;

    if (pCtrl->EIM_vfHwInitHook) {                                      /* HW config routine            */
        pCtrl->EIM_vfHwInitHook();
    }

    eimClockInit();
    eimTimeingConfig(pCtrl);
}

/*********************************************************************************************************
** ��������: eimInit
** ��������: ��ʼ�� EIM �豸
** �䡡��  : pCtrl    EIM�����豸
** �䡡��  : ERROR_CODE or Driver Number
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT eimInit(PEIM_CTRL pCtrl)
{
    pCtrl->EIM_uiMemBase   = (addr_t)API_VmmIoRemap((PVOID)EIM_MEM_BASE_ADDR, pCtrl->EIM_iMemSize);
    if (pCtrl->EIM_uiMemBase == 0) {
        printk(KERN_ERR "eimInit() API_VmmIoRemap error!!\n");
        return PX_ERROR;
    }

    eimHwInit(pCtrl);                                                   /* DO after ioremap             */

    printk(KERN_INFO "EIM Space %08x [0 ~ %x]\n", pCtrl->EIM_uiMemBase, pCtrl->EIM_iMemSize);
    return ERROR_NONE;
}
