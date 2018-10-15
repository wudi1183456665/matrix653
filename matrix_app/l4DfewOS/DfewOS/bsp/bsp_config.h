/*==============================================================================
** bsp_config.h -- system config.
**
** MODIFY HISTORY:
**
** 2016-10-27 wdf Create.
==============================================================================*/

#ifndef __BSP_CONFIG_H__
#define __BSP_CONFIG_H__

/*======================================================================
  Util CONSTANT
======================================================================*/
#define KB                          (1024)
#define MB                          (1024 * 1024)

/**********************************************************************************************************
  TICK
**********************************************************************************************************/
#define SYS_CLK_RATE				100									/* 100 ticks per second			 */

/**********************************************************************************************************
  CPU -- MEMORY
**********************************************************************************************************/
#define DEFAULT_ALIGN_BYTE          4                                   /* 4 byte alignment              */

extern int  __heap_start;
#define CONFIG_MEM_HEAP_LOW         ((unsigned int)&__heap_start)       /*  malloc low address           */
#define CONFIG_MEM_HEAP_SIZE        (4 * MB)                            /*  malloc heap size             */
#define CONFIG_MEM_HEAP_HIGH        (CONFIG_MEM_HEAP_LOW+CONFIG_MEM_HEAP_SIZE)/*  malloc high address    */

#define CONFIG_MEM_STACK_LOW        (CONFIG_MEM_HEAP_HIGH)              /*  task stack low               */
#define CONFIG_MEM_STACK_SIZE       (4 * MB)                            /*  task stack size              */
#define CONFIG_MEM_STACK_HIGH       (CONFIG_MEM_STACK_LOW+CONFIG_MEM_STACK_SIZE) /*  task stack high     */

#define CONFIG_MEM_STACK_PAGE_SIZE  (4 * KB)                            /*  task stack page size         */
#define CONFIG_MEM_STACK_PAGE_NUM   (CONFIG_MEM_STACK_SIZE / CONFIG_MEM_STACK_PAGE_SIZE)/*  page number  */


/*======================================================================
  bsp extern functions
======================================================================*/
extern int  CPU_LOCK();
extern void CPU_UNLOCK(int cpsr_c);
extern void CPU_REBOOT();
extern void CPU_TIME();
extern int  CPU_STACK_INIT(int entry, int arg, int *stk);

/*======================================================================
  assemble language functions declare
======================================================================*/
#define CONTEXT_SWITCH()            asm volatile ("swi 7\t\n")
#define LOAD_HIGHEST_TASK()         asm volatile ("swi 7\t\n")

extern int _G_trigger;

#endif /* __BSP_CONFIG_H__ */

/*==============================================================================
** FILE END
==============================================================================*/

