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
** ��   ��   ��: partition.c
**
** ��   ��   ��: Li.Jing (�)
**
** �ļ���������: 2017 �� 12 �� 19 ��
**
** ��        ��: ���̹���.
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** ��������: __prtnKernCreate
** ��������: �����ں˽���
** �䡡��  : ulVirtAddr     �������е�ַ
**           ulPhysAddr     ���������ַ
**           ulSize         �����С
** �䡡��  : ���̿��ƿ�ָ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static PMX_PRTN  __prtnKernCreate (addr_t  ulVirtAddr, addr_t  ulPhysAddr, ULONG  ulSize)
{
    PMX_PRTN        pprtnNew;
    MX_OBJECT_ID    ulPrtnId;
    MX_PAGE_TABLE   pgtblNew;                                           /*  ҳ��                        */

    ulPrtnId = kobjAlloc(_KOBJ_PRTN);
    pprtnNew = (PMX_PRTN)kobjGet(ulPrtnId);

    pgtblNew = archMmuAllocPageTable();
    archMmuMap(pgtblNew,                                                /*  ӳ���ں�����ռ�            */
               ulVirtAddr,                                              /*  �ں����е�ַ                */
               ulPhysAddr,                                              /*  �ں����������ַ            */
               ulSize,                                                  /*  �ں���ռ��С(16M)           */
               MX_MMU_FLAG_KERN_NOC);                                   /*  �ں�����                    */
    //bspDebugMsg("---------------\n");
    archMmuMap(pgtblNew,                                                /*  ӳ�� IO �ռ�                */
               bspInfoSfrBase(),                                        /*  ���� IO ��ַ                */
               bspInfoSfrBase(),                                        /*  ���� IO ��ַ                */
               bspInfoSfrSize(),                                        /*  IO ��ַ�ռ��С(247M)       */
               MX_MMU_FLAG_DEVS);                                       /*  �豸����                    */
    //bspDebugMsg("---------------\n");

    pprtnNew->PRTN_ulPrtnId    = ulPrtnId;
    pprtnNew->PRTN_pgtblMem    = pgtblNew;
    pprtnNew->RTTN_ppiPtrnInfo = MX_NULL;                               /*  �ں˽�����������Դ��Ϣ      */

    return  (pprtnNew);
}

/*********************************************************************************************************
** ��������: prtnMakeCur
** ��������: ��ָ����������Ϊ��ǰ����
** �䡡��  : pprtn         ����������Ϊ��ǰ���̵Ľ���
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  prtnMakeCur (PMX_PRTN  pprtn)
{
    //bspDebugMsg("\033[31m------switch to page table %X, %X-------\033[37m\n",
    //             pprtn->PRTN_pgtblMem, MX_VIRT_2_PHYS(pprtn->PRTN_pgtblMem));

    archMmuLoad((MX_PAGE_TABLE)MX_VIRT_2_PHYS(pprtn->PRTN_pgtblMem));   /*  ����ҳ��                    */

    MX_CPU_GET_CUR()->CPU_pprtnCur = pprtn;
}
/*********************************************************************************************************
** ��������: prtnAddKumem
** ��������: �ڽ��̿ռ�������һ���û����ں˹�ͬ���ʵ��ڴ�
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  prtnAddKumem (PMX_PRTN  pprtn,  PMX_KUMEM  pkumem)
{
    archMmuMap(pprtn->PRTN_pgtblMem,
               pkumem->KUMEM_ulUserAddr,                                /*  �û��ռ������ַ            */
               MX_VIRT_2_PHYS(pkumem->KUMEM_ulKernAddr),                /*  ����������ַ              */
               pkumem->KUMEM_ulSize,                                    /*  ��������С                  */
               MX_MMU_FLAG_RDWR);                                       /*  ��д                        */
}
/*********************************************************************************************************
** ��������: __prtnUserAlloc
** ��������: ����һ�����̿��ƿ�
** �䡡��  : NONE
** �䡡��  : ���̿��ƿ�ָ��
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static PMX_PRTN  __prtnUserAlloc (VOID)
{
    MX_OBJECT_ID          ulPrtnId;
    PMX_PRTN              pprtnUser;

    ulPrtnId  = kobjAlloc(_KOBJ_PRTN);
    pprtnUser = (PMX_PRTN)kobjGet(ulPrtnId);

    pprtnUser->PRTN_ulPrtnId = ulPrtnId;

    return  (pprtnUser);
}
/*********************************************************************************************************
** ��������: __prtnUserMemMap
** ��������: ӳ���û���ַ�ռ�
** �䡡��  : ppiPrtn            ����������Ϣ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static VOID  __prtnUserMemMap (PMX_PRTN_INFO  ppiPrtn, PMX_PRTN  pprtnUser)
{
    MX_PAGE_TABLE   pgtblNew;                                           /*  ҳ��                        */
    UINT32          i;
    ULONG           ulFlag;
    PMX_MEM_AREA    pmaCur;

    pgtblNew = archMmuCopy(_K_pprtnKernel->PRTN_pgtblMem);

    for (i = 0; i < ppiPrtn->PI_uiMemMum; i++) {
        pmaCur = &ppiPrtn->PI_pmaMems[i];

        if (pmaCur->MA_ulPhysAddr == MX_PRTN_MEM_AUTO) {
            pmaCur->MA_ulPhysAddr = (ULONG)heapAlloc(pmaCur->MA_ulSize, (UINT)ppiPrtn->PI_cpcId);
        } else {
            heapAllocSegment((PVOID)pmaCur->MA_ulPhysAddr, pmaCur->MA_ulSize,
                             (UINT)ppiPrtn->PI_cpcId);
        }

        switch (pmaCur->MA_ulType) {

        case MX_PRTN_MEM_NORM:
            ulFlag = MX_MMU_FLAG_KERN_NOC;
            break;

        case MX_PRTN_MEM_DMA:
            ulFlag = MX_MMU_FLAG_DEVS;
            break;

        default:
            ulFlag = MX_MMU_FLAG_KERN_NOC;
            break;
        }

        archMmuMap(pgtblNew,                                            /*  ӳ����̶�������ռ�        */
                   pmaCur->MA_ulVirtAddr,                               /*  �����ַ                    */
                   pmaCur->MA_ulPhysAddr,                               /*  �����ַ                    */
                   pmaCur->MA_ulSize,                                   /*  ��С                        */
                   ulFlag);                                             /*  ����                        */
    }

    pprtnUser->PRTN_pgtblMem = pgtblNew;
}
/*********************************************************************************************************
** ��������: __prtnUserCodeCopy
** ��������: ���ƽ��̾��������õ������ڴ���
** �䡡��  : ppiPrtn            ����������Ϣ
**           bIsRestart         �Ƿ�������
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static  VOID  __prtnUserCodeCopy (PMX_PRTN_INFO  ppiPrtn, BOOL  bIsRestart)
{
    ULONG                *pulFileData;
    ULONG                *pulLoadAddr;

    pulLoadAddr = (ULONG *)(bIsRestart ? ppiPrtn->PI_pmaMems[0].MA_ulVirtAddr :
                                         ppiPrtn->PI_pmaMems[0].MA_ulPhysAddr);
    pulFileData = (ULONG *)ppiPrtn->PI_cpcStart;
    while (pulFileData < (ULONG *)ppiPrtn->PI_cpcEnd) {
        *pulLoadAddr = *pulFileData;                                    /*  ���ƴ��뵽���̵�ַ�ռ�      */
        pulLoadAddr++;
        pulFileData++;
        //bspDebugMsg(".");
    }
}
/*********************************************************************************************************
** ��������: __prtnUserMainVcpu
** ��������: �����û������е��� VCPU
** �䡡��  : ppiPrtn            ����������Ϣ
**           pprtnUser          ���̿��ƿ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
static  VOID __prtnUserMainVcpu (PMX_PRTN_INFO  ppiPrtn, PMX_PRTN  pprtnUser)
{
    PMX_VCPU              pvcpu;
    MX_VCPU_ATTR          vcpuattr;

    vcpuattr.VCPUATTR_uiPriority = ppiPrtn->PI_uiPriority;
    vcpuattr.VCPUATTR_pvArg      = (PVOID)5;
    vcpuattr.VCPUATTR_bIsUser    = MX_TRUE;
    pvcpu = vcpuCreate((PVCPU_START_ROUTINE)ppiPrtn->PI_ulEntryAddr, &vcpuattr);
                                                                        /*  �����������߳�              */

    vcpuBindPartition(pvcpu, pprtnUser);
    //vcpuSetAffinity(pvcpu, 0);
    vcpuStart(pvcpu);
}
/*********************************************************************************************************
** ��������: prtnRestart
** ��������: ��������һ������
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  prtnRestart (PMX_PRTN  pprtn)
{
    PMX_PRTN_INFO         ppiPrtnInfo;
    PMX_VCPU              pvcpuCur;

    pprtn       = MX_CPU_GET_CUR()->CPU_pprtnCur;                       /*  ��֧�ֵ�ǰ��������          */
    pvcpuCur    = MX_VCPU_GET_CUR();
    ppiPrtnInfo = pprtn->RTTN_ppiPtrnInfo;

    if (ppiPrtnInfo == NULL) {                                          /*  �ں˽���                    */

    } else {                                                            /*  ��������                    */
        bspDebugMsg("--------------------------------------------------\n");
        bspDebugMsg("   Part Id : %s\n", ppiPrtnInfo->PI_cpcId);
        bspDebugMsg(" Part File : %s\n", ppiPrtnInfo->PI_cpcFile);
        bspDebugMsg("Part Start : %X\n", ppiPrtnInfo->PI_cpcStart);
        bspDebugMsg("  Part End : %X\n", ppiPrtnInfo->PI_cpcEnd);

        bspDebugMsg("Part Entry : %X\n", ppiPrtnInfo->PI_ulEntryAddr);
        bspDebugMsg(" Part iMem : %X\n", ppiPrtnInfo->PI_uiMemMum);
        bspDebugMsg(" Memory[0] : %X, ", ppiPrtnInfo->PI_pmaMems[0].MA_ulPhysAddr);
        bspDebugMsg(             "%X\n", ppiPrtnInfo->PI_pmaMems[0].MA_ulSize);
        bspDebugMsg(" Part iIrq : %X\n", ppiPrtnInfo->PI_uiIrqMum);
        bspDebugMsg("--------------------------------------------------\n");

        __prtnUserCodeCopy(ppiPrtnInfo, MX_TRUE);                       /*  �����û�����                */

        irqDisAttach(pvcpuCur);
//        pvcpuCur->VCPU_puctxSP->UCTX_uiCpsr = 0x50;                     /*  ���ж�, USER ģʽ           */
//        pvcpuCur->VCPU_puctxSP->UCTX_uiR0   = 5;
//        pvcpuCur->VCPU_puctxSP->UCTX_uiR15  = ppiPrtnInfo->PI_ulEntryAddr;
    }
}
/*********************************************************************************************************
** ��������: prtnInit
** ��������: ��ʼ������ģ��
** �䡡��  : NONE
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  prtnInit (VOID)
{
    extern  MX_PRTN_INFO  _prtn_info[];
    extern  MX_PRTN_INFO  _eprtn_info[];

    PMX_PRTN_INFO         ppiCur;
    PMX_PRTN              pprtnUser;

    /*
     *  �����ں˽���
     */
    _K_pprtnKernel = __prtnKernCreate(MX_CFG_KERN_VIRT_ADDR,
                                      bspInfoKernRamBase(),
                                      bspInfoKernRamSize());

    for (ppiCur = _prtn_info; ppiCur < _eprtn_info; ppiCur++) {
        bspDebugMsg("--------------------------------------------------\n");
        bspDebugMsg("   Part Id : %s\n", ppiCur->PI_cpcId);
        bspDebugMsg(" Part File : %s\n", ppiCur->PI_cpcFile);
        bspDebugMsg("Part Start : %X\n", ppiCur->PI_cpcStart);
        bspDebugMsg("  Part End : %X\n", ppiCur->PI_cpcEnd);

        bspDebugMsg("Part Entry : %X\n", ppiCur->PI_ulEntryAddr);
        bspDebugMsg(" Part iMem : %X\n", ppiCur->PI_uiMemMum);
        bspDebugMsg(" Memory[0] : %X, ", ppiCur->PI_pmaMems[0].MA_ulPhysAddr);
        bspDebugMsg(             "%X\n", ppiCur->PI_pmaMems[0].MA_ulSize);
        bspDebugMsg(" Part iIrq : %X\n", ppiCur->PI_uiIrqMum);

        pprtnUser = __prtnUserAlloc();                                  /*  ������̿��ƿ�              */
        pprtnUser->RTTN_ppiPtrnInfo = ppiCur;                           /*  ����������Դ��Ϣ            */
        __prtnUserMemMap(ppiCur, pprtnUser);                            /*  ӳ����������ڴ�            */
        __prtnUserCodeCopy(ppiCur, FALSE);                              /*  �����û�����                */
        __prtnUserMainVcpu(ppiCur, pprtnUser);                          /*  �����û����߳�              */

        bspDebugMsg("--------------------------------------------------\n");
    }

    dumpHeap();
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

