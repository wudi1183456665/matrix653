/**********************************************************************************************************
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
** ��   ��   ��: bspMap.h
**
** ��   ��   ��: Han.Hui (����)
**
** �ļ���������: 2008 �� 12 �� 23 ��
**
** ��        ��: C ������ڲ���. �����ҳ�ռ���ȫ��ӳ���ϵ����.
*********************************************************************************************************/

#ifndef __BSPMAP_H
#define __BSPMAP_H

/*********************************************************************************************************
  physical memory zone
*********************************************************************************************************/
#ifdef  __BSPINIT_MAIN_FILE

static LW_MMU_PHYSICAL_DESC    _G_physicalDesc[] = {
        {                                                               /*  �ж�������                  */
                BSP_CFG_RAM_BASE,
                0,
                LW_CFG_VMM_PAGE_SIZE,
                LW_PHYSICAL_MEM_VECTOR,
        },

        {                                                               /*  �ں˴����                  */
                BSP_CFG_RAM_BASE,
                BSP_CFG_RAM_BASE,
                BSP_CFG_TEXT_SIZE,
                LW_PHYSICAL_MEM_TEXT,
        },

        {                                                               /*  �ں����ݶ�                  */
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE,
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE,
                BSP_CFG_DATA_SIZE,
                LW_PHYSICAL_MEM_DATA,
        },

        {                                                               /*  DMA ������                  */
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE + BSP_CFG_DATA_SIZE,
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE + BSP_CFG_DATA_SIZE,
                BSP_CFG_DMA_SIZE,
                LW_PHYSICAL_MEM_DMA,
        },

        {                                                               /*  APP ͨ���ڴ�                */
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE + BSP_CFG_DATA_SIZE    \
                                 + BSP_CFG_DMA_SIZE + BSP_CFG_RAM_RSV_SIZE,
                BSP_CFG_RAM_BASE + BSP_CFG_TEXT_SIZE + BSP_CFG_DATA_SIZE    \
                                 + BSP_CFG_DMA_SIZE + BSP_CFG_RAM_RSV_SIZE,
                BSP_CFG_APP_SIZE,
                LW_PHYSICAL_MEM_APP,
        },

#if 0 //l4
        /*
         * sfr
         */
        {
                0x00000000 + LW_CFG_VMM_PAGE_SIZE,                      /*  ROM ��                      */
                0x00000000 + LW_CFG_VMM_PAGE_SIZE,
                0x00900000 - LW_CFG_VMM_PAGE_SIZE,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  ӳ�������                  */
        },

        {                                                               /*  OCRAM                       */
                0x00900000,
                0x00900000,
                256 * LW_CFG_KB_SIZE,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  ӳ�������                  */
        },

        {                                                               /*  ����                        */
                0x00A00000,
                0x00A00000,
                0x0F600000,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  ӳ�������                  */
        },

        {                                                               /*  AIPS-2 �����ַ��           */
                0x02100000,
                0x02100000,
                0x00200000,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  ӳ�������                  */
        },

        {                                                               /*  AIPS-1 �����ַ��           */
                0x02000000,
                0x02000000,
                0x00100000,
                LW_PHYSICAL_MEM_BOOTSFR                                 /*  ӳ�������                  */
        },
#endif

        {                                                               /*  ����                        */
                0,
                0,
                0,
                0
        }
};

/*********************************************************************************************************
  virtual memory
*********************************************************************************************************/

static LW_MMU_VIRTUAL_DESC    _G_virtualDesc[] = {
        {                                                               /*  Ӧ�ó�������ռ�            */
                BSP_CFG_VAPP_START,
                BSP_CFG_VAPP_SIZE,
                LW_VIRTUAL_MEM_APP
        },

#if 0 //l4
        {
                BSP_CFG_VIO_START,                                      /*  ioremap �ռ�                */
                BSP_CFG_VIO_SIZE,
                LW_VIRTUAL_MEM_DEV
        },
#endif

        {                                                               /*  ����                        */
                0,
                0,
                0
        }
};
#endif                                                                  /*  __BSPINIT_MAIN_FILE         */
#endif                                                                  /*  __BSPMAP_H                  */
/*********************************************************************************************************
  END
*********************************************************************************************************/
