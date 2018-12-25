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
** ��   ��   ��: loadPartion.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 03 �� 01 ��
**
** ��        ��: ������Դ����. (���������ַ�� 0x50000000 ��ʼ, ��������⹦�ܼĴ����ص�)
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
  ��һ��������Դ����
*********************************************************************************************************/
//#define APP1_PHYS_ADDR  0x12000000
#define APP1_PHYS_ADDR  MX_PRTN_MEM_AUTO
#define APP1_PHYS_SIZE  0x10000000
static  MX_MEM_AREA     __maApp1Mems[] MX_PRTNDATA_SEC = { {APP1_PHYS_ADDR, 0x50000000,
                                                            APP1_PHYS_SIZE, MX_PRTN_MEM_NORM} };
static  UINT32          __uiApp1Irqs[] MX_PRTNDATA_SEC = { 0, 1 };
/*********************************************************************************************************
  �ڶ���������Դ����
*********************************************************************************************************/
//#define APP2_PHYS_ADDR  (APP1_PHYS_ADDR + APP1_PHYS_SIZE)
#define APP2_PHYS_ADDR  MX_PRTN_MEM_AUTO
#define APP2_PHYS_SIZE  0x01000000
static  MX_MEM_AREA     __maApp2Mems[] MX_PRTNDATA_SEC = { {APP2_PHYS_ADDR, 0x50000000, APP2_PHYS_SIZE} };
static  UINT32          __uiApp2Irqs[] MX_PRTNDATA_SEC = { 5, 7, 17 };
/*********************************************************************************************************
  ������������Դ����
*********************************************************************************************************/
//#define APP3_PHYS_ADDR  (APP2_PHYS_ADDR + APP2_PHYS_SIZE)
#define APP3_PHYS_ADDR  MX_PRTN_MEM_AUTO
#define APP3_PHYS_SIZE  0x01000000
static  MX_MEM_AREA     __maApp3Mems[] MX_PRTNDATA_SEC = { {APP3_PHYS_ADDR, 0x50000000, APP3_PHYS_SIZE} };
static  UINT32          __uiApp3Irqs[] MX_PRTNDATA_SEC = { };
/*********************************************************************************************************
  ���ĸ�������Դ����
*********************************************************************************************************/
//#define APP4_PHYS_ADDR  (APP3_PHYS_ADDR + APP3_PHYS_SIZE)
#define APP4_PHYS_ADDR  MX_PRTN_MEM_AUTO
#define APP4_PHYS_SIZE  0x01000000
static  MX_MEM_AREA     __maApp4Mems[] MX_PRTNDATA_SEC = { {APP4_PHYS_ADDR, 0x50000000, APP4_PHYS_SIZE} };
static  UINT32          __uiApp4Irqs[] MX_PRTNDATA_SEC = { };
/*********************************************************************************************************
  �����������Դ����
*********************************************************************************************************/
//#define APP5_PHYS_ADDR  (APP4_PHYS_ADDR + APP4_PHYS_SIZE)
#define APP5_PHYS_ADDR  MX_PRTN_MEM_AUTO
#define APP5_PHYS_SIZE  0x01000000
static  MX_MEM_AREA     __maApp5Mems[] MX_PRTNDATA_SEC = { {APP5_PHYS_ADDR, 0x50000000, APP5_PHYS_SIZE} };
static  UINT32          __uiApp5Irqs[] MX_PRTNDATA_SEC = { };

/*********************************************************************************************************
  APP DfewOS ������Դ����
*********************************************************************************************************/
#define APPD_PHYS_ADDR  0x22000000
#define APPD_PHYS_SIZE  0x01000000
static  MX_MEM_AREA     __maAppDMems[] MX_PRTNDATA_SEC = { {APPD_PHYS_ADDR, 0x50000000, APPD_PHYS_SIZE} };
static  UINT32          __uiAppDIrqs[] MX_PRTNDATA_SEC = { };
/*********************************************************************************************************
  APP SylixOS ������Դ����
*********************************************************************************************************/
#define APPS_PHYS_ADDR1  0x12000000
#define APPS_VIRT_ADDR1  0x12000000
#define APPS_PHYS_SIZE1  0x04600000
#define APPS_PHYS_ADDR2  0x16600000
#define APPS_VIRT_ADDR2  0x16600000
#define APPS_PHYS_SIZE2  0x08000000
#define APPS_PHYS_ADDR3  0x1E600000
#define APPS_VIRT_ADDR3  0x1E600000
#define APPS_PHYS_SIZE3  0x03A00000
static  MX_MEM_AREA     __maAppSMems[] MX_PRTNDATA_SEC = { {APPS_PHYS_ADDR1, APPS_VIRT_ADDR1,
                                                            APPS_PHYS_SIZE1, MX_PRTN_MEM_NORM},
                                                           {APPS_PHYS_ADDR2, APPS_VIRT_ADDR2,
                                                            APPS_PHYS_SIZE2, MX_PRTN_MEM_DMA},
                                                           {APPS_PHYS_ADDR3, APPS_VIRT_ADDR3,
                                                            APPS_PHYS_SIZE3, MX_PRTN_MEM_NORM} };
static  UINT32          __uiAppSIrqs[] MX_PRTNDATA_SEC = { 2 };
/*********************************************************************************************************
  L4SylixOS ������Դ����
*********************************************************************************************************/
#define L4SY_PHYS_ADDR1  0x30000000
#define L4SY_VIRT_ADDR1  0x30000000
#define L4SY_PHYS_SIZE1  0x04600000
#define L4SY_PHYS_ADDR2  0x34600000
#define L4SY_VIRT_ADDR2  0x34600000
#define L4SY_PHYS_SIZE2  0x08000000
#define L4SY_PHYS_ADDR3  0x3C600000
#define L4SY_VIRT_ADDR3  0x3C600000
#define L4SY_PHYS_SIZE3  0x03A00000
static  MX_MEM_AREA     __maL4SyMems[] MX_PRTNDATA_SEC = { {L4SY_PHYS_ADDR1, L4SY_VIRT_ADDR1,
                                                            L4SY_PHYS_SIZE1, MX_PRTN_MEM_NORM},
                                                           {L4SY_PHYS_ADDR2, L4SY_VIRT_ADDR2,
                                                            L4SY_PHYS_SIZE2, MX_PRTN_MEM_DMA},
                                                           {L4SY_PHYS_ADDR3, L4SY_VIRT_ADDR3,
                                                            L4SY_PHYS_SIZE3, MX_PRTN_MEM_NORM} };
static  UINT32          __uiL4SyIrqs[] MX_PRTNDATA_SEC = { 0, 1 };
/*********************************************************************************************************
  �������Խ��̾����ļ� (��ǰ·���� matrix_bsp_imx6/l4matrix/)
*********************************************************************************************************/
//MX_LOAD_PRTN(APP1, "../../matrix_app/l4app1/Debug/app.bin",   0x50000000, 100, __maApp1Mems, __uiApp1Irqs);
//MX_LOAD_PRTN(APP1, "../../matrix_app/l4app1/Release/app.bin",   0x50000000, 100, __maApp1Mems, __uiApp1Irqs);
//MX_LOAD_PRTN(APP2, "../../matrix_app/l4app2/Release/app.bin",   0x50000000, 200, __maApp2Mems, __uiApp2Irqs);
//MX_LOAD_PRTN(APP3, "../../matrix_app/l4app3/Debug/app.bin",   0x50000000, 201, __maApp3Mems, __uiApp3Irqs);
//MX_LOAD_PRTN(APP4, "../../matrix_app/l4app2/Debug/app.bin",   0x50000000, 202, __maApp4Mems, __uiApp4Irqs);
MX_LOAD_PRTN(DFWS, "../../matrix_app/l4DfewOS/Release/app.bin",   0x50000000, 102, __maAppDMems, __uiAppDIrqs);
//MX_LOAD_PRTN(APPS, "../../matrix_app/l4sylixos/Release/app.bin",  0x12000000, 101, __maAppSMems, __uiAppSIrqs);
MX_LOAD_PRTN(L4SY, "../../sylixos_bsp_imx6/Release/l4bspimx6.bin",0x30000000, 100, __maL4SyMems, __uiL4SyIrqs);
/*********************************************************************************************************
  END
*********************************************************************************************************/

