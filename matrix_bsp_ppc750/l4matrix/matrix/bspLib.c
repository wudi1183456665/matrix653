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
** 文   件   名: armGic.c
**
** 创   建   人: Li.Jing (李靖)
**
** 文件创建日期: 2018 年 1 月 19 日
**
** 描        述: 通用中断控制器.
*********************************************************************************************************/
#include <Matrix.h>
#include "config.h"                                                     /*  工程配置 & 处理器相关       */
#include "driver/int/i8259a.h"											/*	中断控制器					*/
#include "driver/timer/ppcTimer.h"										/*	定时器						*/
#include "driver/uart/uart_1.h"											/*	串口						*/


//#include "driver/gic/gic.h"                                             /*  GIC 驱动                    */
//#include "driver/uart/uart.h"                                           /*  以供调试                    */
//#include "driver/timer/timer.h"
//#include "driver/ccm_pll/ccm_pll.h"                                     /*  i.MX6  PLL 时钟             */
//#include "driver/cpu_utility/cpu_utility.h"                             /*  i.MX6  多核控制             */
//VOID     armDCacheClearAll(VOID);

static const CHAR   _G_pcCpuInfo[]     = "SM750(266MHz FPU)";
static const CHAR   _G_pcCacheInfo[]   = "64KBytes L1-Cache(D-32K/I-32K)";
static const CHAR   _G_pcPacketInfo[]  = "QEMU PowerPC SylixOS platform";
static const CHAR   _G_pcVersionInfo[] = "BSP version 1.0.0 for LongYuan";

/*********************************************************************************************************
  中断相关
*********************************************************************************************************/
#define PPC750_INTER_INT_NR             8                               /*  内部中断数目                */
/*********************************************************************************************************
  i8259A 平台数据
*********************************************************************************************************/
static I8259A_CTL   _G_i8259aData = {
        .iobase_master  = BSP_CFG_8259A_IO_BASE_MASTER,
        .iobase_slave   = BSP_CFG_8259A_IO_BASE_SLAVE,
        .trigger        = 0,
        .vector_base    = 0,
};

void uart_put_hex (int v)
{
    static char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    int i;

    for (i = 7; i >= 0; i--) {
        uartPutChar(hex[(v >> (i*4)) & 0xF]);
    }
    uartPutChar(';');
}
/*********************************************************************************************************
** 函数名称: bspIntHandle
** 功能描述: 中断入口
** 输  入  : puctxSP   保存在内核堆栈中的 USR 模式上下文
** 输  出  : NONE
** 全局变量:
** 调用模块: armExcAsm.S: archIntEntry()
*********************************************************************************************************/
VOID  bspIntHandle (PARCH_USER_CTX  puctxSP)
{
   REGISTER UINT32	uiVector = BSP_CFG_8259A_VECTOR;
   if (uiVector == BSP_CFG_8259A_VECTOR) {
	   INT iSubVector = i8259aIrq(&_G_i8259aData);
	   if (iSubVector >= 0){
//		   archIntHandle((ULONG)iSubVector + BSP_CFG_8259A_VECTOR_OFFSET, LW_TRUE)
		   archIntHandle((ULONG)iSubVector + BSP_CFG_8259A_VECTOR_OFFSET, MX_TRUE);
	   } else {
		   while (1);
	   }
   } else {
//	   archIntHandle((ULONG)uiVector, LW_TRUE);
	   archIntHandle((ULONG)uiVector, MX_TRUE);
   }
}

//	bspClearInt 函数没有找到调用接口
/*
VOID  bspClearInt (VOID)
{
    armGicIrqWriteDone(GIC_TICK_INT_VEC);
}
*/

/*********************************************************************************************************
** 函数名称: bspIntVectorEnable
** 功能描述: 使能指定的中断向量
** 输  入  : ulVec     中断向量
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bspIntVectorEnable (ULONG  ulVec)
{
//    armGicIntVecterEnable(ulVec, MX_FALSE, GIC_DEFAULT_INT_PRIO, 1 << 0);
	i8259aIrqEnable(&_G_i8259aData, ulVec - BSP_CFG_8259A_VECTOR_OFFSET);
}
/*********************************************************************************************************
** 函数名称: bspIntVectorDisable
** 功能描述: 禁能指定的中断向量
** 输  入  : ulVec     中断向量
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bspIntVectorDisable (ULONG  ulVec)
{
//    armGicIntVecterDisable(ulVector);
	i8259aIrqDisable(&_G_i8259aData, ulVec - BSP_CFG_8259A_VECTOR_OFFSET);
}
/*********************************************************************************************************
** 函数名称: bspIntVectorIsEnable
** 功能描述: 检查指定的中断向量是否使能
** 输  入  : ulVector     中断向量
** 输  出  : MX_FALSE 或 MX_TRUE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
BOOL  bspIntVectorIsEnable (ULONG  ulVector)
{
//    return  (armGicIrqIsEnable(ulVector) ? MX_TRUE : MX_FALSE);
	return  (i8259aIrqIsEnable(&_G_i8259aData, ulVector - BSP_CFG_8259A_VECTOR_OFFSET) ? MX_TRUE : MX_FALSE);
//    return  i8259aIrqIsEnable(&_G_i8259aData, ulVector - BSP_CFG_8259A_VECTOR_OFFSET);

}

/*********************************************************************************************************
  BSP 信息
*********************************************************************************************************/
/*********************************************************************************************************
** 函数名称: bspInfoCpu
** 功能描述: BSP CPU 信息
** 输　入  : NONE
** 输　出  : CPU 信息
** 全局变量:
** 调用模块:
*********************************************************************************************************/
CPCHAR  bspInfoCpu (VOID)
{
    return  (_G_pcCpuInfo);
}
/*********************************************************************************************************
** 函数名称: bspInfoCache
** 功能描述: BSP CACHE 信息
** 输　入  : NONE
** 输　出  : CACHE 信息
** 全局变量:
** 调用模块:
*********************************************************************************************************/
CPCHAR  bspInfoCache (VOID)
{
    return  (_G_pcCacheInfo);
}
/*********************************************************************************************************
** 函数名称: bspInfoPacket
** 功能描述: BSP PACKET 信息
** 输　入  : NONE
** 输　出  : PACKET 信息
** 全局变量:
** 调用模块:
*********************************************************************************************************/
CPCHAR  bspInfoPacket (VOID)
{
    return  (_G_pcPacketInfo);
}
/*********************************************************************************************************
** 函数名称: bspInfoVersion
** 功能描述: BSP VERSION 信息
** 输　入  : NONE
** 输　出  : BSP VERSION 信息
** 全局变量:
** 调用模块:
*********************************************************************************************************/
CPCHAR  bspInfoVersion (VOID)
{
    return  (_G_pcVersionInfo);
}


/*********************************************************************************************************
** 函数名称: bspInfoRomBase
** 功能描述: BSP ROM 基地址
** 输　入  : NONE
** 输　出  : ROM 基地址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
addr_t bspInfoRomBase (VOID)
{
    return  (BSP_CFG_ROM_BASE);
}
/*********************************************************************************************************
** 函数名称: bspInfoRomSize
** 功能描述: BSP ROM 大小
** 输　入  : NONE
** 输　出  : ROM 大小
** 全局变量:
** 调用模块:
*********************************************************************************************************/
size_t bspInfoRomSize (VOID)
{
    return  (BSP_CFG_ROM_SIZE);
}
/*********************************************************************************************************
** 函数名称: bspInfoRamBase
** 功能描述: BSP RAM 基地址
** 输　入  : NONE
** 输　出  : RAM 基地址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
addr_t bspInfoRamBase (VOID)
{
    return  (BSP_CFG_RAM_BASE);
}
/*********************************************************************************************************
** 函数名称: bspInfoRamSize
** 功能描述: BSP RAM 大小
** 输　入  : NONE
** 输　出  : RAM 大小
** 全局变量:
** 调用模块:
*********************************************************************************************************/
size_t bspInfoRamSize (VOID)
{
    return  (BSP_CFG_RAM_SIZE);
}






/*********************************************************************************************************
** 函数名称: bspInfoSfrBase
** 功能描述: BSP IO 空间基地址
** 输　入  : NONE
** 输　出  : 进程 RAM 基地址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  bspInfoSfrBase (VOID)
{
    return  (BSP_CFG_SFR_BASE);
}
/*********************************************************************************************************
** 函数名称: bspInfoSfrSize
** 功能描述: BSP IO 空间大小
** 输　入  : NONE
** 输　出  : RAM 大小
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  bspInfoSfrSize (VOID)
{
    return  (BSP_CFG_SFR_SIZE);
}
/*********************************************************************************************************
** 函数名称: bspInfoKernRamBase
** 功能描述: BSP 进程 RAM 基地址
** 输　入  : NONE
** 输　出  : 进程 RAM 基地址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  bspInfoKernRamBase (VOID)
{
    return  (BSP_CFG_KERN_RAM_BASE);
}
/*********************************************************************************************************
** 函数名称: bspInfoKernRamSize
** 功能描述: BSP 进程 RAM 大小
** 输　入  : NONE
** 输　出  : RAM 大小
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  bspInfoKernRamSize (VOID)
{
    return  (BSP_CFG_KERN_RAM_SIZE);
}
/*********************************************************************************************************
** 函数名称: bspInfoPrtnRamBase
** 功能描述: BSP 进程 RAM 基地址
** 输　入  : NONE
** 输　出  : 进程 RAM 基地址
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  bspInfoPrtnRamBase (VOID)
{
    return  (BSP_CFG_PRTN_RAM_BASE);
}
/*********************************************************************************************************
** 函数名称: bspInfoPrtnRamSize
** 功能描述: BSP 进程 RAM 大小
** 输　入  : NONE
** 输　出  : RAM 大小
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  bspInfoPrtnRamSize (VOID)
{
    return  (BSP_CFG_PRTN_RAM_SIZE);
}
/*********************************************************************************************************
** 函数名称: bspMmuPgdMaxNum
** 功能描述: 返回 pgd 池中剩余个数
** 输  入  : NONE
** 输  出  : pgd 个数
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  bspMmuPgdMaxNum (VOID)
{
    return  (0);
}
/*********************************************************************************************************
** 函数名称: bspMmuPteMaxNum
** 功能描述: 返回 pte 池中剩余个数
** 输  入  : NONE
** 输  出  : pte 个数
** 全局变量:
** 调用模块:
*********************************************************************************************************/
ULONG  bspMmuPteMaxNum (VOID)
{
    return  (0);
}
/*********************************************************************************************************
** 函数名称: bspMpInt
** 功能描述: 对指定核产生核间中断
** 输  入  : ulCpuId      核ID ( 0, 1, 2, 3)
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bspMpInt (ULONG  ulCpuId)
{
//    armGicSoftwareIntSend(GIC_IPI_INT_VEC(ulCpuId), GIC_SW_INT_OPTION_USE_TARGET_LIST, 1 << ulCpuId);
	(VOID)ulCpuId;
}
/*********************************************************************************************************
  SMP 同步变量
*********************************************************************************************************/
#define BSP_HOLDING_PEN_START    0xdeadbeef                             /*  启动从核的模数              */
static volatile ULONG            _G_ulHoldingPen[MX_CFG_MAX_PROCESSORS];
/*********************************************************************************************************
** 函数名称: bspCpuUp
** 功能描述: 启动指定核
** 输  入  : ulCpuId      核ID ( 0, 1, 2, 3)
** 输  出  : NONE
** 全局变量:
** 调用模块: intMatrix()
*********************************************************************************************************/
VOID  bspCpuUp (ULONG  ulCpuId)
{
//    INTREG  iregInterLevel;
//
//    iregInterLevel = archIntDisable();
//
//    _G_ulHoldingPen[ulCpuId] = 0;
//    KN_SMP_MB();
//
//    armDCacheClearAll();                                                /*  全部回写并无效              */
//
//    imx6qCpuStart2nd(ulCpuId);
//
//    KN_SMP_MB();
//    while (_G_ulHoldingPen[ulCpuId] != BSP_HOLDING_PEN_START) {         /*  等待启动核初始化完成        */
//        ;
//    }
//
//    archIntEnable(iregInterLevel);

    bspDebugMsg("bspCpuUp() error: this cpu CAN NOT support this operate!\r\n");
}
/*********************************************************************************************************
** 函数名称: bspCpuUpDone
** 功能描述: 一个从 CPU 启动完成
** 输  入  : NONE
** 输  出  : NONE
** 全局变量:
** 调用模块: schedActiveCpu()
*********************************************************************************************************/
VOID  bspCpuUpDone (VOID)
{
//    ULONG  ulCpuId = archMpCur();
//    INT    i;
//
//    KN_SMP_MB();
//    _G_ulHoldingPen[ulCpuId] = BSP_HOLDING_PEN_START;                   /*  本核初始化完成              */
//    KN_SMP_MB();
//
//    if (ulCpuId == 0) {
//        return;
//    }
//
//__WAIT_CPUS:
//    for (i = 1; i < MX_NCPUS; i++) {
//        if (_G_ulHoldingPen[i] != BSP_HOLDING_PEN_START) {              /*  等待所有从核初始化完成      */
//            goto __WAIT_CPUS;
//        }
//    }
//
//    armDCacheClearAll();                                                /*  全部回写并无效              */

    bspDebugMsg("bspCpuDown() error: this cpu CAN NOT support this operate!\r\n");
}
/*********************************************************************************************************
** 函数名称: bspCpuDown
** 功能描述: 停止一个 CPU
** 输  入  : ulCPUID      核ID ( 0, 1, 2, 3)
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bspCpuDown (ULONG  ulCPUId)
{
//    imx6qCpuDisable(ulCPUId);
    bspDebugMsg("bspCpuDown() error: this cpu CAN NOT support this operate!\r\n");
}
/*********************************************************************************************************
** 函数名称: bspCpuIpiVectorInstall
** 功能描述: 安装 IPI 并使能(核间中断号: CPU0 #0, CPU1 #1, CPU2 #2, CPU3 #3)
** 输  入  : NONE
** 输  出  : NONE
** 全局变量:
** 调用模块: NONE
** ppc750 中是否需要？
*********************************************************************************************************/
//VOID  bspCpuIpiVectorInstall (VOID)
//{
//    ULONG  ulCpuId = archMpCur();
//
//    armGicIntVecterEnable(GIC_IPI_INT_VEC(ulCpuId), MX_FALSE, GIC_IPI_INT_PRIO, 1 << ulCpuId);
//}
/*********************************************************************************************************
** 函数名称: bspDelayUs
** 功能描述: 延迟微秒
** 输  入  : ulUs     微秒数
** 输  出  : NONE
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID bspDelayUs (ULONG ulUs)
{
    /*
     * TODO: 根据你的处理器性能, 修改为你的处理代码
     */
    volatile UINT  i;

    while (ulUs) {
        ulUs--;
        for (i = 0; i < 80; i++) {
        }
    }
}
//spinlock_t  G_slDebugMsg;
/*********************************************************************************************************
** 函数名称: bspDebugMsg
** 功能描述: 调式打印函数
** 输　入  : pcFormat       参数字符串
**           ...            参数列表
** 输　出  :
** 全局变量:
** 调用模块:
*********************************************************************************************************/
VOID  bspDebugMsg(UCHAR  ucCurr, ...)
{
//    UCHAR                  ucCurr;
    uart8250PutStr(BSP_CFG_8250_BASE,ucCurr);


//    va_list                arp;
//    UCHAR                  ucCurr;                                      /*  当前处理的格式字符          */
//    UINT8                  ucFlags;                                     /*  1:填充 2:%l 4:负数 8:%ll    */
//    UINT8                  ucRadix;                                     /*  进制，支持 10, 16           */
//    UINT32                 uiVal;                                       /*  从参数中读取到的整型值      */
//    UCHAR                  ucVal[16];                                   /*  存放整型值转化成字符串      */
//    INT                    i;
//    INT                    iWidth;                                      /*  格式字符串中的宽度值        */
//    INT                    iOnce;                                       /*  每次打印字符个数            */
//    INT                    iReturn;                                     /*  函数返回值，总打印字符个数  */
//
//    //archSpinLock(&G_slDebugMsg);
//
//    va_start(arp, pcFormat);
//
//    for (iOnce = iReturn = 0; iOnce != EOF; iReturn += iOnce) {
//        ucCurr = *pcFormat++;
//        if (ucCurr == 0) {
//            break;                                                      /*  打印结束                    */
//        }
//
//        if (ucCurr != '%') {                                            /*  非格式化字符，直接打印      */
//            iOnce = uart8250PutChar(BSP_CFG_8250_BASE,ucCurr);
//            if (iOnce != EOF) {
//                iOnce = 1;
//            }
//            continue;
//        }
//
//        iWidth = ucFlags = 0;
//        ucCurr = *pcFormat++;                                           /*  获取 % 后一个字符           */
//
//        /*
//         *  处理 %%
//         */
//        if (ucCurr == '%') {                                            /*  print '%'                   */
//            iOnce = uart8250PutChar(BSP_CFG_8250_BASE,ucCurr);
//            continue;
//        }
//
//        /*
//         *  处理格式字符串中的填充格式 %08, % 12
//         */
//        if (ucCurr == '0') {                                            /*  Flag: '0' padding           */
//            ucFlags = 1;
//            ucCurr = *pcFormat++;
//        }
//        if (ucCurr == ' ') {                                            /*  Flag: ' ' padding           */
//            ucCurr = *pcFormat++;
//        }
//        while (ucCurr >= '0' && ucCurr <= '9') {                        /*  print width                 */
//            iWidth = iWidth * 10 + (ucCurr - '0');
//            ucCurr = *pcFormat++;
//        }
//
//        /*
//         *  处理格式字符串中的填充格式 %s, %c
//         */
//        if (ucCurr == 's') {                                            /*  Type is string              */
//            iOnce = uart8250PutStr(BSP_CFG_8250_BASE,va_arg(arp, PUCHAR));
//            continue;
//        }
//        if (ucCurr == 'c') {                                            /*  Type is character           */
//            iOnce = uart8250PutChar(BSP_CFG_8250_BASE,va_arg(arp, INT));
//            if (iOnce != EOF) {
//                iOnce = 1;
//            }
//            continue;
//        }
//
//        /*
//         *  处理格式字符串中的整型长度 %l, %ll
//         */
//        if (ucCurr == 'l') {                                            /*  Prefix: Size is long int    */
//            ucFlags |= 2;
//            ucCurr   = *pcFormat++;
//            if (ucCurr == 'l') {                                        /*  Prefix: Size is long long   */
//                ucFlags |= 8;
//                ucCurr   = *pcFormat++;
//            }
//        }
//
//        /*
//         *  处理格式字符串中的进制格式 %d, %u, %X, %x, %p
//         */
//        ucRadix = 0;
//        if (ucCurr == 'd') {
//            ucRadix = 10;                                               /*  Type is signed decimal      */
//        }
//        if (ucCurr == 'u') {
//            ucRadix = 10;                                               /*  Type is unsigned decimal    */
//        }
//        if (ucCurr == 'X' || ucCurr == 'x' || ucCurr == 'p') {
//            ucRadix = 16;                                               /*  Type is unsigned hexdecimal */
//        }
//        if (ucRadix == 0) {
//            break;                                                      /*  Unknown type                */
//        }
//
//        /*
//         *  获取参数中的整型值
//         */
//        if (ucFlags & 8) {                                              /*  Get the long long value     */
//            uiVal = (UINT64)va_arg(arp, INT64);
//
//        } else if (ucFlags & 2) {                                       /*  Get the long long value     */
//            uiVal = (UINT32)va_arg(arp, INT32);
//
//        } else {
//            uiVal = (ucCurr == 'd') ? (UINT32)va_arg(arp, INT) : (UINT32)va_arg(arp, UINT);
//        }
//
//        /*
//         *  对于负数值，首先取绝对值
//         */
//        if (ucCurr == 'd') {
//            if (uiVal & 0x80000000) {                                   /*  负数                        */
//                uiVal    = 0 - uiVal;
//                ucFlags |= 4;
//            }
//        }
//
//        /*
//         *  将数组转化成字符串
//         */
//        i = sizeof(ucVal) - 1;
//        ucVal[i] = 0;
//        do {
//            ucCurr = (UCHAR)(uiVal % ucRadix + '0');
//            if (ucCurr > '9') {
//                ucCurr += 7;
//            }
//            ucVal[--i] = ucCurr;
//            uiVal /= ucRadix;
//        } while (i && uiVal);
//        if (i && (ucFlags & 4)) {
//            ucVal[--i] = '-';
//        }
//
//        /*
//         *  将字符串填充成指定宽度
//         */
//        iWidth = sizeof(ucVal) - 1 - iWidth;
//        while (i && i > iWidth) {
//            ucVal[--i] = (ucFlags & 1) ? '0' : ' ';
//        }
//
//        iOnce = uartPutStr(&ucVal[i]);                                  /*  打印数值字符串              */
//    }
//
//    va_end(arp);

    //archSpinUnlock(&G_slDebugMsg);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/
