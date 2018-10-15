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
** 文   件   名: ecspi.c
**
** 创   建   人: He.Xin (何 鑫)
**
** 文件创建日期: 2016 年 10 月 27 日
**
** 描        述: SPI_NORFLASH 驱动(25VF016B = 2MB)
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#define  __SYLIXOS_YAFFS_DRV
#include "SylixOS.h"
#include "ecspi/ecspi.h"
/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#define    READ                      0x03                               /*  tx:1 byte cmd + 3 byte      */
#define    READ_HS                   0x0B                               /*  tx:1 byte cmd + 3 byte      */
#define    RDSR                      0x05                               /*  read stat reg 1 byte        */
#define    RDSR_BUSY                 (1 << 0)                           /*  1=write-in-progress         */
#define    RDSR_WEL                  (1 << 1)                           /*  1=write enable              */
#define    RDSR_BP0                  (1 << 2)                           /*  block write prot level      */
#define    RDSR_BP1                  (1 << 3)                           /*  block write prot level      */
#define    RDSR_BP2                  (1 << 4)                           /*  block write prot level      */
#define    RDSR_BP3                  (1 << 5)                           /*  block write prot level      */
#define    RDSR_AAI                  (1 << 6)                           /*  1=AAI prog mode             */
#define    RDSR_BPL                  (1 << 7)                           /*  1=BP3,BP2,BP1,BP0 RO;       */
#define    WREN                      0x06                               /*  write enable                */
#define    WRDI                      0x04                               /*  write disable               */
#define    EWSR                      0x50                               /*  Enable write status         */
#define    WRSR                      0x01                               /*  Write stat reg              */
#define    ERASE_4K                  0x20                               /*  sector erase                */
#define    ERASE_32K                 0x52                               /*  32K block erase             */
#define    ERASE_64K                 0xD8                               /*  64K block erase             */
#define    ERASE_CHIP                0x60                               /*  whole chip erase            */
#define    BYTE_PROG                 0x02                               /*  write one byte              */
#define    AAI_PROG                  0xAD                               /*  auto increase write         */
#define    JEDEC_ID                  0x9F                               /*  read JEDEC ID               */

#define    QSPI_USE_LOCK             0
#define    SPI_LOCK()
#define    SPI_UNLOCK()

#define    ENABLE_WRITE_STATUS()     __flashNorCmd1byte(EWSR)
#define    WRITE_ENABLE()			 __flashNorCmd1byte(WREN)
#define    WRITE_DISABLE()		     __flashNorCmd1byte(WRDI)

#define    SPI_ENABLE_SS()           API_GpioSetValue(_G_spiNorObj.uiSSPin, 0)
#define    SPI_DISABLE_SS()          API_GpioSetValue(_G_spiNorObj.uiSSPin, 1)

#define    NOR_BASE_ADDR             0
#define    NOR_ERASE_SIZE            (4 * 1024)
#define    NOR_BLOCK_SIZE            NOR_ERASE_SIZE
#define    NOR_SIZE_RESERVED         (256 * 1024)                       /*  Reserve 256KB at the end    */

#define    NOR_TOTAL_SIZE            (2 * 1024 * 1024 - NOR_SIZE_RESERVED)
#define    NOR_BLOCK_PER_CHIP        (NOR_TOTAL_SIZE / NOR_BLOCK_SIZE)

#define    SPI_ADAPTER_NAME          "/bus/spi/0"
#define    SPI_NOR_DEVNAME           "/dev/spiflash"

#define    NOR_DEBUG(fmt, arg...)    printk("[SPI] %s() : " fmt, __func__, ##arg)
/*********************************************************************************************************
  SPI_NOR 参数
     name       |       jedec   |    ext_jedec  | sector_size | nr_sectors |      flags       |
  s"SST25VF016B",	   0xbf2541,       0x0,       64 * 1024,        32,	      SECT_4K | SST_WP
*********************************************************************************************************/
/*********************************************************************************************************
 SPI_NOR 内部对象数据结构
*********************************************************************************************************/
typedef struct {
    struct spi_flash    *pspiflash;
    struct yaffs_devnor  devnor;
    PLW_SPI_DEVICE       Spi_NorDev;
    UINT                 uiOffsetBytes;
    UINT                 uiEraseSize;
    UINT                 uiTotalSize;
    UINT                 uiPageSize;

    UINT                 uiSSPin;
#if QSPI_USE_LOCK > 0
    LW_OBJECT_HANDLE     hLock;
#endif

    CPCHAR               pcName;
} SPI_NOR_OBJ, *PSPI_NOR_OBJ;
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
static  SPI_NOR_OBJ  _G_spiNorObj  = {
        .uiEraseSize = NOR_ERASE_SIZE,
        .uiTotalSize = NOR_TOTAL_SIZE,
};
/*********************************************************************************************************
** 函数名称: __ssGpioInit
** 功能描述: ss GPIO init
** 输　入  : uiSSPin         GPIO 序号
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ssGpioInit (UINT  uiSSPin)
{
	API_GpioRequestOne(uiSSPin, LW_GPIOF_OUT_INIT_HIGH, "SS0");

	_G_spiNorObj.uiSSPin = uiSSPin;

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: NorDevCreate
** 功能描述: 向适配器挂载SPI接口设备
** 输　入  : pSpiBusName     SPI适配器名称
**           uiSSPin         GPIO 序号
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT  norDevCreate (PCHAR  pSpiBusName, UINT  uiSSPin)
{
    INT           iError;
    PSPI_NOR_OBJ  pSpi_Nor = &_G_spiNorObj;

    if (pSpi_Nor->Spi_NorDev == NULL) {
        pSpi_Nor->Spi_NorDev = API_SpiDeviceCreate(SPI_ADAPTER_NAME, SPI_NOR_DEVNAME);
    }
    __ssGpioInit(uiSSPin);

    iError = API_SpiDeviceBusRequest(pSpi_Nor->Spi_NorDev);
    if (iError == PX_ERROR) {
    	NOR_DEBUG("Spi Request Bus error\n");
    	return (PX_ERROR);
    }

    API_SpiDeviceCtl(pSpi_Nor->Spi_NorDev, SPI_MODE_SET, 0x00);

    API_SpiDeviceBusRelease(pSpi_Nor->Spi_NorDev);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __flashNorStatus
** 功能描述: 读取Spi_Flash状态寄存器
** 输　入  :
** 输　出  : nor_flash状态寄存器值
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static UINT8  __flashNorStatus ()
{
	UINT8  rxValue[1];
    UINT8  txCmd[1]  = {RDSR};

	LW_SPI_MESSAGE   spiCmdMessage  = {
			.SPIMSG_uiLen           = 1,
			.SPIMSG_pucWrBuffer     = txCmd,
			.SPIMSG_pucRdBuffer     = NULL,
	};
	LW_SPI_MESSAGE   spiRdMessage   = {
			.SPIMSG_uiLen           = 1,
			.SPIMSG_pucWrBuffer     = NULL,
			.SPIMSG_pucRdBuffer     = rxValue,
	};

	SPI_ENABLE_SS();
	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiRdMessage,  1);
	SPI_DISABLE_SS();

	return  (*rxValue);
}

static INT  __flashWaitForIdle()
{
    PSPI_NOR_OBJ  pSpi_Nor  = &_G_spiNorObj;
    UINT          uiRetry   = 10000;

    while (__flashNorStatus() & RDSR_BUSY) {
        if (uiRetry-- == 0) {
            printk(KERN_WARNING "nor flash erase error!\n");
            API_SpiDeviceBusRelease(pSpi_Nor->Spi_NorDev);
            return  (PX_ERROR);
        }
    }
	
	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __flashNorWriteStatus
** 功能描述: 写Spi_Flash状态寄存器
** 输　入  : value  写入值
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __flashNorWriteStatus (UINT8  value)
{
	UINT8             txCmd[2]      = {WRSR, value};
	LW_SPI_MESSAGE    spiCmdMessage = {
			    .SPIMSG_uiLen       = 2,
			    .SPIMSG_pucWrBuffer = txCmd,
			    .SPIMSG_pucRdBuffer = NULL,
	};

	SPI_ENABLE_SS();
	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
	SPI_DISABLE_SS();

    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __flashNorCmd1byte
** 功能描述: 写Spi_Flash 1Byte cmd
** 输　入  : cmd    控制命令
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __flashNorCmd1byte (UINT8 cmd)
{
	UINT8            txCmd[1]       = {cmd};
	LW_SPI_MESSAGE   spiCmdMessage  = {
			.SPIMSG_uiLen           = 1,
			.SPIMSG_pucWrBuffer     = txCmd,
			.SPIMSG_pucRdBuffer     = NULL,
	};

	SPI_ENABLE_SS();
	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
	SPI_DISABLE_SS();

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __flashErase
** 功能描述: 以4KB大小擦除flash
** 输　入  : addr         擦除地址
**           block_size   4K大小
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __flashErase (addr_t  addr, UINT  block_size)
{
	INT              iError;
	UINT8            txCmd[4];

	PSPI_NOR_OBJ     pSpi_Nor       = &_G_spiNorObj;
	LW_SPI_MESSAGE   spiCmdMessage  = {
			  .SPIMSG_uiLen         = 4,
			  .SPIMSG_pucWrBuffer   = txCmd,
			  .SPIMSG_pucRdBuffer   = NULL,
	};

	txCmd[0] = ERASE_4K;
	txCmd[1] = addr >> 16;
	txCmd[2] = addr >> 8;
	txCmd[3] = addr >> 0;

    iError = API_SpiDeviceBusRequest(pSpi_Nor->Spi_NorDev);
    if (iError == PX_ERROR) {
    	NOR_DEBUG("Spi Request Bus error\n");
    	return  (PX_ERROR);
    }

	ENABLE_WRITE_STATUS();
	__flashNorWriteStatus(0);
	WRITE_ENABLE();

	SPI_ENABLE_SS();
	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
	SPI_DISABLE_SS();

    __flashWaitForIdle();
	API_SpiDeviceBusRelease(pSpi_Nor->Spi_NorDev);

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __flashRead
** 功能描述: 读取flash
** 输　入  : addr         读取地址
**           rd_len       读取长度
**           data         数据存放地址
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT __flashRead (addr_t  addr, UINT  rd_len, void  *data)
{
	INT              iError;
	UINT8            txCmd[4];

    PSPI_NOR_OBJ     pSpi_Nor       = &_G_spiNorObj;
	LW_SPI_MESSAGE   spiCmdMessage  = {
			.SPIMSG_uiLen           = 4,
			.SPIMSG_pucWrBuffer     = txCmd,
			.SPIMSG_pucRdBuffer     = NULL,
	};

	txCmd[0] = READ;
	txCmd[1] = addr >> 16;
	txCmd[2] = addr >> 8;
	txCmd[3] = addr >> 0;

	LW_SPI_MESSAGE  spiRdMessage    = {
			.SPIMSG_uiLen           = rd_len,
			.SPIMSG_pucWrBuffer     = NULL,
			.SPIMSG_pucRdBuffer     = data,
	};

    iError = API_SpiDeviceBusRequest(pSpi_Nor->Spi_NorDev);
    if (iError == PX_ERROR) {
    	NOR_DEBUG("Spi Request Bus error\n");
    	return  (PX_ERROR);
    }

	SPI_ENABLE_SS();
	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiRdMessage, 1);
	SPI_DISABLE_SS();

	API_SpiDeviceBusRelease(pSpi_Nor->Spi_NorDev);

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __flashWrite1Byte
** 功能描述: 写1字节flash
** 输　入  : addr         读取地址
**           value        写入数值
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __flashWrite1Byte(addr_t  addr, UINT8  value)
{
	UINT8            txCmd[5];
	LW_SPI_MESSAGE   spiCmdMessage  = {
			.SPIMSG_uiLen           = 5,
			.SPIMSG_pucWrBuffer     = txCmd,
			.SPIMSG_pucRdBuffer     = NULL,
	};

	txCmd[0] = BYTE_PROG;
	txCmd[1] = addr >> 16;
	txCmd[2] = addr >> 8;
	txCmd[3] = addr >> 0;
	txCmd[4] = value;

	WRITE_ENABLE();

	SPI_ENABLE_SS();
	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
	SPI_DISABLE_SS();

	__flashWaitForIdle();

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __flashWrite
** 功能描述: 写flash
** 输　入  : addr         写入地址
**           len          写入长度
**           data         数据地址
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __flashWrite (addr_t addr, UINT len, const void *data)
{
    PSPI_NOR_OBJ     pSpi_Nor       = &_G_spiNorObj;
    u32              uiaddr         = addr;
    const  char      *wbuf          = data;
    INT32            iRemainLen     = len;
    UINT8            ucFirstWflag   = 1;
    UINT8            txCmd[6];
	INT              iError;
    LW_SPI_MESSAGE   spiCmdMessage  = {
    			.SPIMSG_uiLen       = 6,
    			.SPIMSG_pucWrBuffer = txCmd,
    			.SPIMSG_pucRdBuffer = NULL,
    };

    iError = API_SpiDeviceBusRequest(pSpi_Nor->Spi_NorDev);
    if (iError == PX_ERROR) {
    	NOR_DEBUG("Spi Request Bus error\n");
    	return (PX_ERROR);
    }

    ENABLE_WRITE_STATUS();
    __flashNorWriteStatus(0);

    if ((uiaddr & 1) != 0) {
	    __flashWrite1Byte(uiaddr, *wbuf);

	    if (--iRemainLen == 0) {
	    	return (ERROR_NONE);
	    }
	    uiaddr++;
	    wbuf++;
    }

    WRITE_ENABLE();
    for ( ; iRemainLen > 1; iRemainLen -= 2, uiaddr += 2) {
        if (ucFirstWflag == 1) {
        	ucFirstWflag = 0;

        	txCmd[0] = AAI_PROG;
        	txCmd[1] = addr >> 16;
        	txCmd[2] = addr >> 8;
        	txCmd[3] = addr >> 0;
        	txCmd[4] = *wbuf++;
        	txCmd[5] = *wbuf++;

        	SPI_ENABLE_SS();
        	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
        	SPI_DISABLE_SS();

        	__flashWaitForIdle();
        } else if (ucFirstWflag == 0) {
        	spiCmdMessage.SPIMSG_uiLen = 3;

        	txCmd[0] = AAI_PROG;
        	txCmd[1] = *wbuf++;
        	txCmd[2] = *wbuf++;

        	SPI_ENABLE_SS();
        	API_SpiDeviceTransfer(_G_spiNorObj.Spi_NorDev, &spiCmdMessage, 1);
        	SPI_DISABLE_SS();

        	__flashWaitForIdle();
        }
    }
    WRITE_DISABLE();

    __flashWaitForIdle();

    if (iRemainLen == 1) {
	    __flashWrite1Byte(uiaddr, *wbuf);
    }

    API_SpiDeviceBusRelease(pSpi_Nor->Spi_NorDev);

	return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: spiNorErase
** 功能描述: 擦除一个块
** 输  入  : nordev    nor设备
**           block     块地址
** 输  出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static int spiNorErase (struct yaffs_devnor *nordev, int  block)
{
    addr_t  addr = norBlock2Addr(nordev, block);
    int     ret  = -1;

    SPI_LOCK();
    ret = __flashErase(addr, NOR_BLOCK_SIZE);
    SPI_UNLOCK();

    return  (ret == 0 ? YAFFS_OK : YAFFS_FAIL);
}
/*********************************************************************************************************
** 函数名称: spiNorRead
** 功能描述: 数据读取
** 输  入  : nordev    nor设备
**           addr      字节地址
**           data      保存读取的数据
**           len       需要读取数据的长度
** 输  出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static int spiNorRead (struct yaffs_devnor *nordev, addr_t addr, void *data, size_t len)
{
    int     ret = -1;

    SPI_LOCK();
    ret = __flashRead(addr, len, data);
    SPI_UNLOCK();

    return  (ret == 0 ? YAFFS_OK : YAFFS_FAIL);
}
/*********************************************************************************************************
** 函数名称: spiNorWrite
** 功能描述: 数据写入
** 输  入  : nordev    nor设备
**           addr      字节地址
**           data      写入数据缓冲地址
**           len       需要读取数据的长度
** 输  出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static int spiNorWrite (struct yaffs_devnor *nordev, addr_t addr, const void *data, size_t len)
{
    int     ret = -1;

    SPI_LOCK();
    ret = __flashWrite(addr, len, data);
    SPI_UNLOCK();

    return  (ret == 0 ? YAFFS_OK : YAFFS_FAIL);
}
/*********************************************************************************************************
** 函数名称: spiNorInitialise
** 功能描述: 芯片初始化
** 输  入  : nordev    nor设备
** 输  出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static int spiNorInitialise (struct yaffs_devnor *nordev)
{
    return  (YAFFS_OK);
}
/*********************************************************************************************************
** 函数名称: spiNorDeinitialise
** 功能描述: 芯片反初始化
** 输  入  : nordev    nor设备
** 输  出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static int spiNorDeinitialise (struct yaffs_devnor *nordev)
{
    return  (YAFFS_OK);
}
/*********************************************************************************************************
** 函数名称: SpiNorDrvInstall
** 功能描述: SPI_NOR 驱动安装
** 输  入  : uiOffsetByts   实际使用时, 偏移(保留)的字节数
** 输  出  : ERROR CODE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
INT spiNorDrvInstall (CPCHAR  pcName, UINT  ulOffsetBytes)
{
    struct yaffs_devnor  *nordev;
    struct yaffs_nor     *nor;
    struct yaffs_dev     *dev;
    struct yaffs_param   *param;
    struct yaffs_driver  *drv;

    if (ulOffsetBytes >= _G_spiNorObj.uiTotalSize) {
    	NOR_DEBUG("invalid bytes offset.\n");
        return  (PX_ERROR);
    }

    if (!pcName) {
        pcName = "/flash";
    }

    _G_spiNorObj.uiOffsetBytes = ulOffsetBytes;
    _G_spiNorObj.pcName        = pcName;

    nordev = &_G_spiNorObj.devnor;

    nor    = &nordev->nor;
    dev    = &nordev->dev;

    param  = &dev->param;
    drv    = &dev->drv;

    nor->nor_base            = NOR_BASE_ADDR;
    nor->block_size          = NOR_BLOCK_SIZE;
    nor->chunk_size          = 512 + 16;
    nor->bytes_per_chunk     = 512;
    nor->spare_per_chunk     = 16;
    nor->chunk_per_block     = nor->block_size / nor->chunk_size;
    nor->block_per_chip      = NOR_BLOCK_PER_CHIP;

    nor->nor_erase_fn        = spiNorErase;
    nor->nor_read_fn         = spiNorRead;
    nor->nor_write_fn        = spiNorWrite;
    nor->nor_initialise_fn   = spiNorInitialise;
    nor->nor_deinitialise_fn = spiNorDeinitialise;

    param->name                  = pcName;
    param->total_bytes_per_chunk = nor->bytes_per_chunk;
    param->chunks_per_block      = nor->chunk_per_block;
    param->n_reserved_blocks     = 4;
    param->start_block           = (ulOffsetBytes / NOR_BLOCK_SIZE) + 1;
    param->end_block             = NOR_BLOCK_PER_CHIP - 1;
    param->use_nand_ecc          = 0;
    param->disable_soft_del      = 1;
    param->n_caches              = 10;

    drv->drv_write_chunk_fn  = ynorWriteChunk;
    drv->drv_read_chunk_fn   = ynorReadChunk;
    drv->drv_erase_fn        = ynorEraseBlock;
    drv->drv_initialise_fn   = ynorInitialise;
    drv->drv_deinitialise_fn = ynorDeinitialise;

    dev->driver_context = &nordev;

    yaffs_add_device(dev);
    yaffs_mount(pcName);

    return  (ERROR_NONE);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
