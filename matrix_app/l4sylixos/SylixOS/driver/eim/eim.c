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
** ��   ��   ��: eim.c
**
** ��   ��   ��: Xu.GuiZhou (�����)
**
** �ļ���������: 2016 �� 09 �� 13 ��
**
** ��        ��: EIM�����ӿ�ʵ��
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include "stdlib.h"
#include "string.h"

#include "eim.h"

/*********************************************************************************************************
  �궨��
*********************************************************************************************************/
#define READ16(addr)            (*((UINT16 *)(addr)))
#define WRITE16(dat, addr)      (*((UINT16 *)(addr)) = (dat))
/*********************************************************************************************************
  EIM �������߽ӿ�
*********************************************************************************************************/
static UINT32   _G_ieimDrvNum   = 0;

static EIM_CTRL  _G_eimCtrl   = {
        .EIM_uiPhyMemBase   = EIM_MEM_BASE_ADDR,
        .EIM_iMemSize       = EIM_SPACE_SIZE,
        .EIM_bInit          = 0,
};

/*********************************************************************************************************
  ��������IO�ӿ�ʵ��
*********************************************************************************************************/
/*********************************************************************************************************
** ��������: __eimOpen
** ��������: �� EIM �豸
** �䡡��  : pDev                  �豸
**           pcName                �豸����
**           iFlags                ��־
**           iMode                 ģʽ
** �䡡��  : �ļ��ڵ�
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static LONG  __eimOpen (PEIM_CTRL pCtrl, PCHAR pcName, INT iFlags, INT iMode)
{
    PLW_FD_NODE   pFdNode;
    BOOL          bIsNew;

    if (pcName == LW_NULL) {
        _ErrorHandle(ERROR_IO_NO_DEVICE_NAME_IN_PATH);
        return  (PX_ERROR);
    }

    if(LW_DEV_GET_USE_COUNT(&pCtrl->EIM_devHdr) > 0) {
        _ErrorHandle(ERROR_IO_FILE_BUSY);
        return  (PX_ERROR);
    }

    pFdNode = API_IosFdNodeAdd(&pCtrl->EIM_devHdr_fdNodeHeader, (dev_t)pCtrl, 0,
                               iFlags, iMode, 0, 0, 0, LW_NULL, &bIsNew);
    if (pFdNode == LW_NULL) {
        printk(KERN_ERR "eimOpen(): failed to add fd node!\n");
        return  (PX_ERROR);
    }

    pCtrl->EIM_szOffset = 0;
    LW_DEV_INC_USE_COUNT(&pCtrl->EIM_devHdr);
    return ERROR_NONE;
}

/*********************************************************************************************************
** ��������: __eimClose
** ��������: �ر� EIM �豸
** �䡡��  : pFdEntry              �ļ��ṹ
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT __eimClose (PLW_FD_ENTRY   pFdEntry)
{
    PEIM_CTRL  pCtrl = (PEIM_CTRL)pFdEntry->FDENTRY_pdevhdrHdr;

    if(LW_DEV_GET_USE_COUNT(&pCtrl->EIM_devHdr) == 0) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }
    LW_DEV_DEC_USE_COUNT(&pCtrl->EIM_devHdr);

    return (ERROR_NONE);
}

/*********************************************************************************************************
** ��������: __eimIoctl
** ��������: ���� EIM �豸
** �䡡��  : pFdEntry              �ļ��ṹ
**           iCmd                  ����
**           lArg                  ����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __eimIoctl (PLW_FD_ENTRY  pFdEntry, INT  iCmd, LONG lArg)
{
    PEIM_CTRL   pCtrl = (PEIM_CTRL)pFdEntry->FDENTRY_pdevhdrHdr;
    INT         *iSize = (INT *)lArg;

    switch (iCmd) {
    case FIONREAD:                                                      /* ��ȡ�ڴ�ռ��С             */
        *iSize = pCtrl->EIM_iMemSize;
        break;
    case FIOSEEK:
    {
        if (*iSize < 0 || (*iSize >= pCtrl->EIM_iMemSize)) {
            _ErrorHandle(EINVAL);
            return  (PX_ERROR);
        }
        pCtrl->EIM_szOffset = *iSize;
        break;
    }
    default:
        _ErrorHandle(ENOSYS);
        return  (PX_ERROR);
    }

    return *iSize;
}

/*********************************************************************************************************
** ��������: __eimMmap
** ��������: ӳ�� EIM �豸
** �䡡��  : pFdEntry              �ļ��ṹ
**           pMmapArea             ӳ������
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __eimMmap (PLW_FD_ENTRY            pFdEntry,
                       PLW_DEV_MMAP_AREA       pMmapArea)
{
    INT         iFlags;
    PEIM_CTRL   pEimDev = (PEIM_CTRL)pFdEntry->FDENTRY_pdevhdrHdr;

    /*
     * �������
     */
    if (!pMmapArea) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if (pMmapArea->DMAP_stLen != pEimDev->EIM_iMemSize) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if (pMmapArea->DMAP_offPages != 0) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    /*
     * ���ݶ�дȨ�޽���ӳ��
     */
    iFlags = pFdEntry->FDENTRY_iFlag & O_ACCMODE;

    switch (iFlags) {
    case O_RDONLY:
    case O_WRONLY:
    case O_RDWR:
    {
        pMmapArea->DMAP_ulFlag &= ~LW_VMM_FLAG_CACHEABLE;               /*  ȡ�� Cacheable              */
        pMmapArea->DMAP_ulFlag &= ~LW_VMM_FLAG_BUFFERABLE;              /*  ȡ�� Bufferable             */

        if (API_VmmRemapArea(pMmapArea->DMAP_pvAddr,
                             (PVOID)pEimDev->EIM_uiPhyMemBase,
                             pMmapArea->DMAP_stLen,
                             pMmapArea->DMAP_ulFlag,
                             LW_NULL, LW_NULL)) {
            return  (PX_ERROR);
        }
        return ERROR_NONE;
    }
    default:
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }
}
/*********************************************************************************************************
** ��������: __eimUnMap
** ��������: ȡ��ӳ�� EIM �豸
** �䡡��  : pFdEntry              �ļ��ṹ
**           pMmapArea             ӳ������
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static INT  __eimUnMap (PLW_FD_ENTRY            pFdEntry,
                        PLW_DEV_MMAP_AREA       pMmapArea)
{
    INT     iFlags;

    /*
     * �������
     */
    if (!pMmapArea) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if (pMmapArea->DMAP_stLen == 0) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if (pMmapArea->DMAP_offPages != 0) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    /*
     * ���ݶ�дȨ�޽���ȡ��ӳ��
     */
    iFlags = pFdEntry->FDENTRY_iFlag & O_ACCMODE;

    switch (iFlags) {
    case O_RDONLY:
    case O_WRONLY:
    case O_RDWR:
        /* TODO: DO something?? */
        return  (ERROR_NONE);

    default:
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    return  (PX_ERROR);
}
/*********************************************************************************************************
** ��������: __eimWrite
** ��������: ���� write ����
** �䡡��  : pFdentry     �ļ��ڵ���ڽṹ
**           pvBuf        д����
**           stLen        д���ݳ���
** �䡡��  : �ɹ���ȡ�����ֽ�����
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ssize_t  __eimWrite (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
    PEIM_CTRL   pCtrl   = (PEIM_CTRL)pFdentry->FDENTRY_pdevhdrHdr;
    UINT16     *pusBuf  = (UINT16 *) pvBuf;

    size_t      i = 0;

    if (stLen < 0) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if ((addr_t)pusBuf & 0x1) {                                         /* 16bit aligned                */
        _ErrorHandle(EFAULT);
        return  (PX_ERROR);
    }

    for(i=0; i<(stLen/2); i++) {
        WRITE16(*pusBuf, pCtrl->EIM_uiMemBase + pCtrl->EIM_szOffset);
        pusBuf++;
    }

    return (i*2);
}

/*********************************************************************************************************
** ��������: __eimRead
** ��������: ���� read ����
** �䡡��  : pFdentry     �ļ��ڵ���ڽṹ
**           pvBuf        ������
**           stLen        �����ݳ���
** �䡡��  : �ɹ������ֵ�ĸ���
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
static ssize_t  __eimRead (PLW_FD_ENTRY   pFdentry, PVOID  pvBuf, size_t stLen)
{
    PEIM_CTRL   pCtrl   = (PEIM_CTRL)pFdentry->FDENTRY_pdevhdrHdr;
    UINT16     *pusBuf  = (UINT16 *) pvBuf;

    size_t      i = 0;

    if (stLen < 0) {
        _ErrorHandle(EINVAL);
        return  (PX_ERROR);
    }

    if ((addr_t)pusBuf & 0x1) {                                         /* 16bit aligned                */
        _ErrorHandle(EFAULT);
        return  (PX_ERROR);
    }

    for(i=0; i<(stLen/2); i++) {
        *pusBuf = READ16(pCtrl->EIM_uiMemBase + pCtrl->EIM_szOffset);
        pusBuf++;
    }
    return (i*2);
}

/*********************************************************************************************************
** ��������: eimDrv
** ��������: ��װ EIM ����
** �䡡��  : NONE
** �䡡��  : ERROR_CODE or Driver Number
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  eimDrv (VOID)
{
    struct file_operations  fileop;

    if (_G_ieimDrvNum) {
        return  (ERROR_NONE);
    }

    lib_memset(&fileop, 0, sizeof(struct file_operations));

    fileop.owner     = THIS_MODULE;
    fileop.fo_create = __eimOpen;
    fileop.fo_open   = __eimOpen;
    fileop.fo_close  = __eimClose;
    fileop.fo_ioctl  = __eimIoctl;
    fileop.fo_write  = __eimWrite;
    fileop.fo_read   = __eimRead;
    fileop.fo_mmap   = __eimMmap;
    fileop.fo_unmap  = __eimUnMap;

    _G_ieimDrvNum = iosDrvInstallEx2(&fileop, LW_DRV_TYPE_NEW_1);

    DRIVER_LICENSE(_G_ieimDrvNum,     "Dual BSD/GPL->Ver 1.0");
    DRIVER_AUTHOR(_G_ieimDrvNum,      "Xu.GuiZhou");
    DRIVER_DESCRIPTION(_G_ieimDrvNum, "EIM driver.");

    return  (_G_ieimDrvNum > 0) ? (ERROR_NONE) : (PX_ERROR);
}

/*********************************************************************************************************
** ��������: eimDevCreate
** ��������: ���� EIM �豸
** �䡡��  : pcName �豸����
**           iSize  ����ܵ�����
** �䡡��  : ERROR_CODE
** ȫ�ֱ���:
** ����ģ��:
*********************************************************************************************************/
INT  eimDevCreate(PCHAR pcName, VOIDFUNCPTR hwInitHook)
{
    PEIM_CTRL pCtrl = &_G_eimCtrl;                                      /* ����չ��֧�ֶ��豸           */

    if (pcName == NULL) {
        printk(KERN_ERR "eimDevCreate(): device name invalid!\n");
        return  (PX_ERROR);
    }

    if (API_IosDevAddEx(&pCtrl->EIM_devHdr, pcName, _G_ieimDrvNum, DT_CHR) != ERROR_NONE) {
        printk(KERN_ERR "eimDevCreate(): can not add device : %s.\n", strerror(errno));
        return  (PX_ERROR);
    }

    if (hwInitHook) {
        pCtrl->EIM_vfHwInitHook = hwInitHook;
    }

    eimInit(pCtrl);
    return  (ERROR_NONE);
}


