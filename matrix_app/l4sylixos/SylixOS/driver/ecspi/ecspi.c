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
** 创   建   人: He.Xin ( 何鑫 )
**
** 文件创建日期: 2016 年 10 月 25 日
**
** 描        述: IMX6 处理器 ECSPI驱动
*********************************************************************************************************/
#define  __SYLIXOS_KERNEL
#include "SylixOS.h"

#include "sdk.h"
#include "irq/irq_numbers.h"
#include "ecspi_regs.h"
#include "ecspi.h"
#include "gpio/imx6q_gpio.h"
#include "board.h"
/*********************************************************************************************************
  宏定义
*********************************************************************************************************/
#define    SPI_DEBUG(fmt, arg...)      printk("[SPI] %s() : " fmt, __func__, ##arg)

#define    __MCSPI_CHANNEL_NR          5
#define    __SPI_RETRY_TIMES           10000
#define    __MAX_SPI_BYTES             (64 * 4)                         /*  FIFO                        */
#define    __MAX_SPI_BITS              (64 * 4 * 8)

#define    IN_TXFIFO(uivalue)          writel(uivalue, ECSPI_TXDATA)
#define    OUT_RXFIFO()                readl(ECSPI_RXDATA)
/*********************************************************************************************************
  ecSpi 通道类型定义
*********************************************************************************************************/
typedef struct {
    addr_t              SPICHAN_addrPhyAddrBase;                        /*  物理地址基地址              */
    UINT                SPICHAN_uiChannel;                              /*  通道号                      */

    addr_t              SPICHAN_pVirtAddrBase;                          /*  虚拟地址基地址              */
    UINT32              SPICHAN_uiInputclock;                           /*  输入时钟频率                */

    BOOL                SPICHAN_bIsInit;                                /*  是否已经初始化              */
} __IMX6_SPI_CHANNEL, *__PIMX6_SPI_CHANNEL;
/*********************************************************************************************************
  全局变量
*********************************************************************************************************/
static __IMX6_SPI_CHANNEL _G_ecSpiChannels[__MCSPI_CHANNEL_NR] = {
    {
         ECSPI1_BASE_ADDR,
         0,
    },
    {
         ECSPI2_BASE_ADDR,
         1,
    },
    {
         ECSPI3_BASE_ADDR,
         2,
    },
    {
         ECSPI4_BASE_ADDR,
         3,
    },
    {
         ECSPI5_BASE_ADDR,
         4,
    },
};
/*********************************************************************************************************
   IMX6 spi控制器初始化
*********************************************************************************************************/
static INT  __imx6SpiInit (__PIMX6_SPI_CHANNEL  pSpiChannel)
{
    UINT  value = 0;

	/*
	 *   Clear the EN bit in ECSPI_CONREG to reset the block
	 */
    value = readl(ECSPI_CONREG);
    value &= ~(ECSPI_CONREG_EN);
    writel(value, ECSPI_CONREG);

    /*
     *  Enable the clocks for ECSPI
     */
	imx6qClkGateSet(pSpiChannel->SPICHAN_addrPhyAddrBase, CLOCK_ON);

	/*
	 *  Set the EN bit in ECSPI_CONREG to put ECSPI out of reset
	 */
	value = readl(ECSPI_CONREG);
    value |= (ECSPI_CONREG_EN);
    writel(value, ECSPI_CONREG);

	/*
	 *  Configure registers of ECSPI properly according to the specifications of the external SPI device
	 */
	value = readl(ECSPI_CONREG);
    value &= ~ECSPI_CONREG_PREDIVIDER;
    value &= ~ECSPI_CONREG_POSTDIVIDER;
    value |= 2 << 12;                                                   /*  设置默认频率为60 Mhz / 3    */
    writel(value, ECSPI_CONREG);

	value = readl(ECSPI_CONREG);
    value &= ~(ECSPI_CONREG_CHMODE);
    value |= 1 << 4;                                                    /*   SS0 MASTER                 */
    writel(value, ECSPI_CONREG);

	value = readl(ECSPI_CONFIGREG);
    value &= ~(1 << 0);
    value &= ~(1 << 4);                                                 /*  SPI MODE0                   */
    value &= ~(1 << 12);
    value &= ~(1 << 8);                                                 /*  single burst                */
    writel(value, ECSPI_CONFIGREG);

	writel(BM_ECSPI_STATREG_TC, ECSPI_STATREG);                         /*  clear tc                    */

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __ecSpiHwInit
** 功能描述: spi 通道初始化，硬件初始化
** 输　入  : pSpiChannel     spi 通道
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ecSpiHwInit (__PIMX6_SPI_CHANNEL  pSpiChannel)
{
    if (!pSpiChannel->SPICHAN_bIsInit) {
	    pSpiChannel->SPICHAN_pVirtAddrBase = pSpiChannel->SPICHAN_addrPhyAddrBase;

		if (__imx6SpiInit(pSpiChannel) != ERROR_NONE) {
            return (PX_ERROR);
        }

		pSpiChannel->SPICHAN_bIsInit = LW_TRUE;
	}

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __ecspiStartRx
** 功能描述: spi start tx
** 输　入  : pSpiChannel     spi 通道
**           uiBurstByte     发送字节数
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ecspiStartTx (__PIMX6_SPI_CHANNEL  pSpiChannel, UINT  uiBurstByte)
{
    UINT  uivalue = 0;

    uivalue = readl(ECSPI_CONREG) ;
    uivalue &= ~ECSPI_CONREG_BURSTLENGTH;
    uivalue |= ((uiBurstByte << 3) - 1) << 20;                          /*  设置发送bit数               */
    writel(uivalue, ECSPI_CONREG);

    uivalue = readl(ECSPI_CONREG) ;
    uivalue |= ECSPI_CONREG_XCH;                                        /*  启动tx                      */
    writel(uivalue, ECSPI_CONREG);

    uivalue = __SPI_RETRY_TIMES;
    while (!(readl(ECSPI_STATREG) & BM_ECSPI_STATREG_TC) && (uivalue--)) {

    }

	writel(BM_ECSPI_STATREG_TC, ECSPI_STATREG);                         /*  清发送完成标志              */
    return (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __ecspiStartRx
** 功能描述: spi 读取接收缓冲区
** 输　入  : pSpiChannel     spi 通道
**           pucRxbuf        数据存放地址，为空时清楚缓冲区
**           UINT            接收字节数
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __ecspiStartRx (__PIMX6_SPI_CHANNEL  pSpiChannel, UINT8  *pucRxbuf, UINT  uiBurstByte)
{
    INT     i;
    UINT32  uivalue;
    INT     iRemainLen = uiBurstByte;

    if (pucRxbuf != NULL) {
        for (i = iRemainLen; i > 0; i -= 4) {
            uivalue = OUT_RXFIFO();

        	if(i < 4) {
            	switch (i) {
            	case 1:
            		*pucRxbuf   = uivalue & 0xff;
            		break;
            	case 2:
            		*pucRxbuf++ = (uivalue >> 8) & 0xff;
            		*pucRxbuf++ = uivalue & 0xff;
            		break;
            	case 3:
            		*pucRxbuf++ = (uivalue >> 16) & 0xff;
            		*pucRxbuf++ = (uivalue >> 8)  & 0xff;
            		*pucRxbuf++ = uivalue & 0xff;
            		break;
            	default:
            		break;
            	}                                                   /*  X X X 1  |  X X 2 1  | X 3 2 1  */

            } else {
            	*pucRxbuf++ = (uivalue >> 24) & 0xff;
        		*pucRxbuf++ = (uivalue >> 16) & 0xff;
        		*pucRxbuf++ = (uivalue >> 8)  & 0xff;
        		*pucRxbuf++ = uivalue & 0xff;
            }
        }
    } else {
        for (i = iRemainLen; i > 0; i -= 4) {
            uivalue = OUT_RXFIFO();                                 /*  clear RXFIFO                    */
        }
    }
	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __espiTransfer
** 功能描述: spi 传输函数
** 输　入  : pSpiChannel     spi 通道
**           pSpiAdapter     spi 适配器
**           pSpiMsg         spi 传输消息组
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT   __espiTransfer (__PIMX6_SPI_CHANNEL    pSpiChannel,
		                     PLW_SPI_ADAPTER        pSpiadapter,
                             PLW_SPI_MESSAGE        pSpimsg)
{
    INT    i;
    UINT8  *pucTxbuf     = pSpimsg->SPIMSG_pucWrBuffer;
    UINT8  *pucRxbuf     = pSpimsg->SPIMSG_pucRdBuffer;
    INT    iBustByte     = pSpimsg->SPIMSG_uiLen;
    INT    iRemainLen    = iBustByte;
    INT    mod           = 0;
    UINT   uivalue       = 0;
    INT    iTxMaxLen     = __MAX_SPI_BYTES;

    /*
     * 发送命令，长度不限, 超过FIFO要拆分
     */
    if (pucRxbuf == NULL && (pucTxbuf != NULL)) {
        for ( ; iRemainLen > 0; iRemainLen -= iTxMaxLen) {
        	if (iRemainLen < iTxMaxLen) {
                mod = iRemainLen % 4;

                if (mod != 0) {
                    switch (mod) {
                    case 1:
                	    uivalue = pucTxbuf[0];
                	    break;
                    case 2:
                	    uivalue = pucTxbuf[0] << 8 |
                	              pucTxbuf[1] << 0;
                	    break;
                    case 3:
                	    uivalue = pucTxbuf[0] << 16 |
                	              pucTxbuf[1] << 8  |
                	              pucTxbuf[2] << 0;
                	    break;
                    default:
                	    break;
                    }                                              /*  X 3 2 1  | X X 2 1 | X X X 1|    */
                    IN_TXFIFO(uivalue);
                    pucTxbuf += mod;
                }

                for (i = 0; i < iRemainLen - mod; i += 4) {
                    uivalue = pucTxbuf[i]   << 24 |
                    		  pucTxbuf[i+1] << 16 |
                    		  pucTxbuf[i+2] << 8  |
                    		  pucTxbuf[i+3] << 0;

                    IN_TXFIFO(uivalue);
                }

                __ecspiStartTx(pSpiChannel, iRemainLen);
                __ecspiStartRx(pSpiChannel, NULL, iRemainLen);

        	} else {
                for (i = 0; i < iTxMaxLen; i += 4) {
                    uivalue = pucTxbuf[i]   << 24 |
                    		  pucTxbuf[i+1] << 16 |
                    		  pucTxbuf[i+2] << 8  |
                    		  pucTxbuf[i+3] << 0;

                    IN_TXFIFO(uivalue);
                }
                __ecspiStartTx(pSpiChannel, iTxMaxLen);
                __ecspiStartRx(pSpiChannel, NULL, iTxMaxLen);

                pucTxbuf += iTxMaxLen;
        	}
        }
    } else if (pucRxbuf != NULL) {
		uivalue = 0x00;

        for ( ; iRemainLen > 0; iRemainLen -= iTxMaxLen) {
            if (iRemainLen < iTxMaxLen) {
                for (i = 0; i < iRemainLen; i += 4) {
                	IN_TXFIFO(uivalue);
                }
                __ecspiStartTx(pSpiChannel, iRemainLen);
                __ecspiStartRx(pSpiChannel, pucRxbuf, iRemainLen);

			} else {
                for (i = 0; i < iTxMaxLen; i += 4) {
                	IN_TXFIFO(uivalue);
                }

                __ecspiStartTx(pSpiChannel, iTxMaxLen);
                __ecspiStartRx(pSpiChannel, pucRxbuf, iTxMaxLen);
                pucRxbuf += iTxMaxLen;
			}
        }
	} else if (pucTxbuf == NULL && pucTxbuf == NULL) {
		SPI_DEBUG("transfer message is invalid\n");
	}

    if (pSpimsg->SPIMSG_pfuncComplete) {                                /*  传输完成上层回调函数        */
    	pSpimsg->SPIMSG_pfuncComplete(pSpimsg->SPIMSG_pvContext);
    }
    return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __imx6SpiTransfer
** 功能描述: spi 传输函数
** 输　入  : pSpiChannel     spi 通道
**           pSpiAdapter     spi 适配器
**           pSpiMsg         spi 传输消息组
**           iNum            消息数量
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT   __imx6SpiTransfer (__PIMX6_SPI_CHANNEL    pSpiChannel,
		                        PLW_SPI_ADAPTER        pSpiadapter,
                                PLW_SPI_MESSAGE        pSpimsg,
                                INT                    iNum)
{
	INT  i = 0;

	for (i=0; i<iNum; i++, pSpimsg++) {
		if (__espiTransfer(pSpiChannel, pSpiadapter, pSpimsg) != ERROR_NONE) {
			SPI_DEBUG("transfer msg_err num %d\n", i);
			break;
		}
	}

	return  (i);
}

/*********************************************************************************************************
** 函数名称: __imx6SpiTransfer
** 功能描述: spi 传输函数
** 输　入  : pSpiChannel     spi 通道
**           pSpiAdapter     spi 适配器
**           iCmd            spi 命令
**           lArg            spi 参数
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6SpiMasterCtl (__PIMX6_SPI_CHANNEL    pSpiChannel,
		                        PLW_SPI_ADAPTER        pSpiadapter,
		                        INT                    iCmd,
		                        LONG                   lArg)
{
	unsigned long  ulBaudRate = BAUDRATE_MAX;
	unsigned long  ulSetRate  = lArg;
	UINT           i          = 1;
	UINT           uivalue    = 0;
	UINT           uiSpiMode  = 0;
	UINT           uiChannel  = 0;
	UINT           uiPol      = 0;
	UINT           uiPha      = 0;

	switch (iCmd) {
	case BAUDRATE_SET:
	{
        if (ulSetRate > BAUDRATE_MAX) {
            SPI_DEBUG("Can't set rate over MAXBAUTRATE %d\n", BAUDRATE_MAX);
            return (PX_ERROR);
        }

        while ((ulBaudRate / i) > ulSetRate) {
            i++;
        }

        uivalue  = readl(ECSPI_CONREG);
        uivalue &= ~ECSPI_CONREG_PREDIVIDER;
        uivalue &= ~ECSPI_CONREG_POSTDIVIDER;
        uivalue |= (i-1) << 12;
        writel(uivalue, ECSPI_CONREG);

        break;
	}
	case SPI_MODE_SET:
	{
        uiSpiMode = lArg & 0xf;
        uiChannel = lArg & 0xf0;

        if (uiSpiMode == 0) {
        	uiPol = 0;
        	uiPha = 0;
        } else if (uiSpiMode == 1) {
        	uiPol = 0;
        	uiPha = 1;
        } else if (uiSpiMode == 2) {
        	uiPol = 1;
        	uiPha = 0;
        } else if (uiSpiMode == 3) {
        	uiPol = 1;
        	uiPha = 1;
        } else {
        	SPI_DEBUG("Spi Set Mode is invalid, range 0 -- 3\n");
        	return (PX_ERROR);
        }

        if (uiChannel > 3) {
        	SPI_DEBUG("Spi Set channel is invalid, range 0 -- 3\n");
        	return (PX_ERROR);
        }

        uivalue  = readl(ECSPI_CONFIGREG);
        uivalue &= ~(0xf << 0);
        uivalue &= ~(0xf << 4);
        uivalue |= uiPol << (4 + uiChannel);
        uivalue |= uiPha << (0 + uiChannel);
        writel(uivalue, ECSPI_CONFIGREG);

		break;
	}
	default:
		break;
	}

	return  (ERROR_NONE);
}
/*********************************************************************************************************
** 函数名称: __imx6Spi0Transfer
** 功能描述: spi 传输函数
** 输　入  : pSpiAdapter     spi 适配器
**           pSpiMsg         spi 传输消息组
**           iNum            消息数量
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6Spi0Transfer (PLW_SPI_ADAPTER     pSpiadapter,
                                PLW_SPI_MESSAGE     pSpimsg,
                                INT                 iNum)
{
	return  (__imx6SpiTransfer(&_G_ecSpiChannels[0], pSpiadapter, pSpimsg, iNum));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi0MasterCtl
** 功能描述: spi 控制函数
** 输　入  : pSpiAdapter     spi 适配器
**           iCmd            控制命令
**           lArg            控制参数
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6Spi0MasterCtl (PLW_SPI_ADAPTER    pSpiadapter,
                                 INT                iCmd,
                                 LONG               lArg)
{
	return  (__imx6SpiMasterCtl(&_G_ecSpiChannels[0], pSpiadapter, iCmd, lArg));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi1Transfer
** 功能描述: spi 传输函数
** 输　入  : pSpiAdapter     spi 适配器
**           pSpiMsg         spi 传输消息组
**           iNum            消息数量
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static  INT  __imx6Spi1Transfer (PLW_SPI_ADAPTER    pSpiadapter,
                                 PLW_SPI_MESSAGE    pSpimsg,
                                 INT                iNum)
{
	return  (__imx6SpiTransfer(&_G_ecSpiChannels[1], pSpiadapter, pSpimsg, iNum));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi1MasterCtl
** 功能描述: spi 传输函数
** 输　入  : pSpiAdapter     spi 适配器
**           iCmd            控制命令
**           lArg            控制参数
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6Spi1MasterCtl (PLW_SPI_ADAPTER    pSpiadapter,
                                 INT                iCmd,
                                 LONG               lArg)
{
	return  (__imx6SpiMasterCtl(&_G_ecSpiChannels[1], pSpiadapter, iCmd, lArg));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi2Transfer
** 功能描述: spi 传输函数
** 输　入  : pSpiAdapter     spi 适配器
**           pSpiMsg         spi 传输消息组
**           iNum            消息数量
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static  INT  __imx6Spi2Transfer (PLW_SPI_ADAPTER    pSpiadapter,
                                 PLW_SPI_MESSAGE    pSpimsg,
                                 INT                iNum)
{
    return  (__imx6SpiTransfer(&_G_ecSpiChannels[2], pSpiadapter, pSpimsg, iNum));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi2MasterCtl
** 功能描述: spi 控制函数
** 输　入  : pSpiAdapter     spi 适配器
**           iCmd            控制命令
**           lArg            控制参数
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6Spi2MasterCtl (PLW_SPI_ADAPTER    pSpiadapter,
                                   INT              iCmd,
                                   LONG             lArg)
{
    return  (__imx6SpiMasterCtl(&_G_ecSpiChannels[2], pSpiadapter, iCmd, lArg));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi3Transfer
** 功能描述: spi 传输函数
** 输　入  : pSpiAdapter     spi 适配器
**           pSpiMsg         spi 传输消息组
**           iNum            消息数量
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6Spi3Transfer (PLW_SPI_ADAPTER     pSpiadapter,
                                PLW_SPI_MESSAGE     pSpimsg,
                                INT                 iNum)
{
    return  (__imx6SpiTransfer(&_G_ecSpiChannels[3], pSpiadapter, pSpimsg, iNum));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi3MasterCtl
** 功能描述: spi 控制函数
** 输　入  : pSpiAdapter     spi 适配器
**           iCmd            控制命令
**           lArg            控制参数
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static  INT   __imx6Spi3MasterCtl (PLW_SPI_ADAPTER  pSpiadapter,
                                   INT              iCmd,
                                   LONG             lArg)
{
    return  (__imx6SpiMasterCtl(&_G_ecSpiChannels[3], pSpiadapter, iCmd, lArg));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi4Transfer
** 功能描述: spi 传输函数
** 输　入  : pSpiAdapter     spi 适配器
**           pSpiMsg         spi 传输消息组
**           iNum            消息数量
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6Spi4Transfer (PLW_SPI_ADAPTER     pSpiadapter,
                                PLW_SPI_MESSAGE     pSpimsg,
                                INT                 iNum)
{
    return  (__imx6SpiTransfer(&_G_ecSpiChannels[4], pSpiadapter, pSpimsg, iNum));
}
/*********************************************************************************************************
** 函数名称: __imx6Spi4MasterCtl
** 功能描述: spi 控制函数
** 输　入  : pSpiAdapter     spi 适配器
**           iCmd            控制命令
**           lArg            控制参数
** 输　出  : 完成传输的消息数量
** 全局变量:
** 调用模块:
*********************************************************************************************************/
static INT  __imx6Spi4MasterCtl (PLW_SPI_ADAPTER    pSpiadapter,
                                 INT                iCmd,
                                 LONG               lArg)
{
    return  (__imx6SpiMasterCtl(&_G_ecSpiChannels[4], pSpiadapter, iCmd, lArg));
}
/*********************************************************************************************************
  IMX6 处理器 spi 驱动程序
*********************************************************************************************************/
static LW_SPI_FUNCS _G_ecSpiFuncs[__MCSPI_CHANNEL_NR] = {
        {
            __imx6Spi0Transfer,
            __imx6Spi0MasterCtl,
        },
        {
            __imx6Spi1Transfer,
            __imx6Spi1MasterCtl,
        },
        {
            __imx6Spi2Transfer,
            __imx6Spi2MasterCtl,
        },
        {
            __imx6Spi3Transfer,
            __imx6Spi3MasterCtl,
        },
        {
            __imx6Spi4Transfer,
            __imx6Spi4MasterCtl,
        },
};
/*********************************************************************************************************
** 函数名称: spiBusDrv
** 功能描述: 初始化 spi 总线并获取驱动程序
** 输　入  : uiChannel         通道号
**           pvHwPinInit       初始化SPI管脚
** 输　出  : spi 总线驱动程序
** 全局变量:
** 调用模块:
*********************************************************************************************************/
PLW_SPI_FUNCS  spiBusDrv (UINT  uiChannel, VOIDFUNCPTR  pvHwPinInit)
{
	if (uiChannel >= __MCSPI_CHANNEL_NR) {
		SPI_DEBUG("i2cBusFuns(): I2C channel invalid!\n");
        return  (LW_NULL);
    }

	if (pvHwPinInit) {
	    pvHwPinInit();
	}

	if (__ecSpiHwInit(&_G_ecSpiChannels[uiChannel]) != ERROR_NONE) {
        return  (LW_NULL);
    }

    return  (&_G_ecSpiFuncs[uiChannel]);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
