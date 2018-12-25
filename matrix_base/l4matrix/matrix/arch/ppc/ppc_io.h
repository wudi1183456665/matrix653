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
** 文   件   名: ppcIo.h
**
** 创   建   人: Li.YQ
**
** 文件创建日期: 2018 年 6 月 30 日
**
** 描        述: PowerPC IO.
*********************************************************************************************************/

#ifndef __PPC_IO_H
#define __PPC_IO_H

/*********************************************************************************************************
  ppc 处理器 I/O 屏障
*********************************************************************************************************/

#ifdef __GNUC__
#define PPC_EIEIO()     __asm__ __volatile__ ("eieio")
#else
#define PPC_EIEIO()
#endif                                                                  /*  __GNUC__                    */

#define KN_IO_MB()      { PPC_EIEIO(); KN_SMP_MB();  }
#define KN_IO_RMB()     { PPC_EIEIO(); KN_SMP_RMB(); }
#define KN_IO_WMB()     { PPC_EIEIO(); KN_SMP_WMB(); }
#define in8 read8
/*********************************************************************************************************
  ppc 处理器 I/O 内存读
*********************************************************************************************************/
static MX_INLINE UINT8  read8 (addr_t  ulAddr)
{
	UINT8	ucVal = *(volatile UINT8 *)ulAddr;
	KN_IO_RMB();
	return	(ucVal);
}

static MX_INLINE UINT16  read16 (addr_t  ulAddr)
{
	UINT16	usVal = *(volatile UINT16 *)ulAddr;
	KN_IO_RMB();
	return 	(usVal);
}

static MX_INLINE UINT32  read32 (addr_t  ulAddr)
{
	UINT32	uiVal = *(volatile UINT32 *)ulAddr;
	KN_IO_RMB();
	return 	(uiVal);
}

static MX_INLINE UINT64  read64 (addr_t  ulAddr)
{
	UINT64 u64Val = *(volatile UINT64 *)ulAddr;
	KN_IO_RMB();
	return	(u64Val);
}
/*********************************************************************************************************
  PowerPC 处理器 I/O 内存写
*********************************************************************************************************/
#define out8(d,a)   write8(d,0ul+a)

static MX_INLINE VOID  write8 (UINT8  ucData, addr_t  ulAddr)
{
	KN_IO_WMB();
	*(volatile UINT8 *)ulAddr = ucData;
	KN_IO_WMB();
}

static MX_INLINE VOID  write16 (UINT16  usData, addr_t  ulAddr)
{
	KN_IO_WMB();
	*(volatile UINT16 *)ulAddr = usData;
	KN_IO_WMB();
}

static MX_INLINE VOID  write32 (UINT32  uiData, addr_t  ulAddr)
{
	KN_IO_WMB();
	*(volatile UINT32 *)ulAddr = uiData;
	KN_IO_WMB();
}

static MX_INLINE VOID  write64 (UINT64  u64Data, addr_t  ulAddr)
{
	KN_IO_WMB();
	*(volatile UINT64 *)ulAddr = u64Data;
	KN_IO_WMB();
}

#define readl(addr)                 read32((addr))
#define writel(val, addr)           write32((val), (addr))
#endif 																	/*  __PPC_IO_H					*/
/*********************************************************************************************************
  END
*********************************************************************************************************/
