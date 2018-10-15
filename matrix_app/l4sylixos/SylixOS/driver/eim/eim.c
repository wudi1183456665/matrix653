/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                   嵌入式实时操作系统
**
**                                       SylixOS(TM)
**
**                               Copyright  All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: eim.c
**
** 创   建   人: Xu.GuiZhou (徐贵洲)
**
** 文件创建日期: 2016 年 09 月 13 日
**
** 描        述: EIM驱动接口实现
*********************************************************************************************************/
#define __SYLIXOS_KERNEL
#include "config.h"
#include "SylixOS.h"
#include "stdlib.h"
#include "string.h"

#include "eim.h"

/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#define READ16(addr)            (*((UINT16 *)(addr)))
#define WRITE16(dat, addr)      (*((UINT16 *)(addr)) = (dat))
/*********************************************************************************************************
  EIM 外扩总线接口
*********************************************************************************************************/
static UINT32   _G_ieimDrvNum   = 0;

static EIM_CTRL  _G_eimCtrl   = {
        .EIM_uiPhyMemBase   = EIM_MEM_BASE_ADDR,
        .EIM_iMemSize       = EIM_SPACE_SIZE,
        .EIM_bInit          = 0,
};

/*********************************************************************************************************
  驱动程序IO接口实现
*********************************************************************************************************/
/*********************************************************************************************************
** 函数名称: __eimOpen
** 功能描述: 打开 EIM 设备
** 输　入  : pDev                  设备
**           pcName                设备名字
**           iFlags                标志
**           iMode                 模式
** 输　出  : 文件节点
** 全局变量:
** 调用模块:
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
** 函数名称: __eimClose
** 功能描述: 关闭 EIM 设备
** 输　入  : pFdEntry              文件结构
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
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
** 函数名称: __eimIoctl
** 功能描述: 控制 EIM 设备
** 输　入  : pFdEntry              文件结构
**           iCmd                  命令
**           lArg                  参数
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __eimIoctl (PLW_FD_ENTRY  pFdEntry, INT  iCmd, LONG lArg)
{
    PEIM_CTRL   pCtrl = (PEIM_CTRL)pFdEntry->FDENTRY_pdevhdrHdr;
    INT         *iSize = (INT *)lArg;

    switch (iCmd) {
    case FIONREAD:                                                      /* 获取内存空间大小             */
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
** 函数名称: __eimMmap
** 功能描述: 映射 EIM 设备
** 输　入  : pFdEntry              文件结构
**           pMmapArea             映射区域
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __eimMmap (PLW_FD_ENTRY            pFdEntry,
                       PLW_DEV_MMAP_AREA       pMmapArea)
{
    INT         iFlags;
    PEIM_CTRL   pEimDev = (PEIM_CTRL)pFdEntry->FDENTRY_pdevhdrHdr;

    /*
     * 参数检查
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
     * 根据读写权限进行映射
     */
    iFlags = pFdEntry->FDENTRY_iFlag & O_ACCMODE;

    switch (iFlags) {
    case O_RDONLY:
    case O_WRONLY:
    case O_RDWR:
    {
        pMmapArea->DMAP_ulFlag &= ~LW_VMM_FLAG_CACHEABLE;               /*  取消 Cacheable              */
        pMmapArea->DMAP_ulFlag &= ~LW_VMM_FLAG_BUFFERABLE;              /*  取消 Bufferable             */

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
** 函数名称: __eimUnMap
** 功能描述: 取消映射 EIM 设备
** 输　入  : pFdEntry              文件结构
**           pMmapArea             映射区域
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __eimUnMap (PLW_FD_ENTRY            pFdEntry,
                        PLW_DEV_MMAP_AREA       pMmapArea)
{
    INT     iFlags;

    /*
     * 参数检查
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
     * 根据读写权限进行取消映射
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
** 函数名称: __eimWrite
** 功能描述: 驱动 write 函数
** 输　入  : pFdentry     文件节点入口结构
**           pvBuf        写缓冲
**           stLen        写数据长度
** 输　出  : 成功读取数据字节数量
** 全局变量:
** 调用模块:
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
** 函数名称: __eimRead
** 功能描述: 驱动 read 函数
** 输　入  : pFdentry     文件节点入口结构
**           pvBuf        读缓冲
**           stLen        读数据长度
** 输　出  : 成功数码管值的个数
** 全局变量:
** 调用模块:
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
** 函数名称: eimDrv
** 功能描述: 安装 EIM 驱动
** 输　入  : NONE
** 输　出  : ERROR_CODE or Driver Number
** 全局变量:
** 调用模块:
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
** 函数名称: eimDevCreate
** 功能描述: 生成 EIM 设备
** 输　入  : pcName 设备名称
**           iSize  数码管的数量
** 输　出  : ERROR_CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  eimDevCreate(PCHAR pcName, VOIDFUNCPTR hwInitHook)
{
    PEIM_CTRL pCtrl = &_G_eimCtrl;                                      /* 可扩展成支持多设备           */

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


