/*==============================================================================
** cpuc. -- some CPU related routines
**
** MODIFY HISTORY:
**
** 2016-10-27 wdf Create.
==============================================================================*/
#include "../config.h"
#include "../types.h"
#include "../dlist.h"
#include "../task.h"
#include "../tick.h"

/*=======================================================
 * OS defined variables
**=====================================================*/
extern OS_TCB  *G_p_current_tcb;
extern OS_TCB **G_pp_highest_tcb;

/*=======================================================
 * 堆空间和栈空间 8MB
**=====================================================*/
//int __attribute__((aligned(4096))) _heap_stack[2*1024*1024];

/*==============================================================================
 * - CPU_LOCK()
 *
 * - Disable Irqs
 */
int CPU_LOCK()
{
    int   ireg;

    ireg = MXI_vcpuDisableInt();

    return ireg;
}

/*==============================================================================
 * - CPU_UNLOCK()
 *
 * - Enable Irqs
 */
void CPU_UNLOCK(int ireg)
{
    if (ireg == 0) {
        MXI_vcpuEnableInt(ireg);
    }
}

/*==============================================================================
 * - CPU_REBOOT()
 *
 * - Reboot this partition
 */
void CPU_REBOOT()
{
    //MX_reset_partition(G_imgHdr.id);
    MXI_prtnRestart(0);
}

void CPU_TIME()
{
    //mxTime_t tHypv;
    //mxTime_t tPart;
    //MX_get_time(TIME_HYPV, &tHypv);
    //MX_get_time(TIME_PART, &tPart);
}

/*==============================================================================
 * - CPU_STACK_INIT()
 *
 * - Init XAL stack, [stk] just like %esp
 */
int CPU_STACK_INIT(int entry, int arg, int *stk)
{
    int sp = (int)stk;

    *--stk = 1;   /* _G_trigger */

    *--stk = entry;      /* R15 */
    *--stk = entry;      /* R14 */
    *--stk = sp;         /* Sp */
    *--stk = 0x12121212; /* R12 */
    *--stk = 0x11111111; /* R11 */
    *--stk = 0x10101010; /* R10 */
    *--stk = 0x09090909; /* R9 */
    *--stk = 0x08080808; /* R8 */
    *--stk = 0x07070707; /* R7 */
    *--stk = 0x06060606; /* R6 */
    *--stk = 0x05050505; /* R5 */
    *--stk = 0x04040404; /* R4 */
    *--stk = 0x03030303; /* R3 */
    *--stk = 0x02020202; /* R2 */
    *--stk = 0x01010101; /* R1 */
    *--stk = arg;        /* R0 */
    *--stk = 0x50;       /* Cpsr (USR 模式 使能 IRQ) */

    return (int)stk;
}

/*==============================================================================
 * - __uctxCopy()
 *
 * - copy user context
 */
static void  __uctxCopy (int *puctxDst, const int *puctxSrc)
{
    *puctxDst++ = *puctxSrc++; /* Cpsr */
    *puctxDst++ = *puctxSrc++; /* R0 */
    *puctxDst++ = *puctxSrc++; /* R1 */
    *puctxDst++ = *puctxSrc++; /* R2 */
    *puctxDst++ = *puctxSrc++; /* R3 */
    *puctxDst++ = *puctxSrc++; /* R4 */
    *puctxDst++ = *puctxSrc++; /* R5 */
    *puctxDst++ = *puctxSrc++; /* R6 */
    *puctxDst++ = *puctxSrc++; /* R7 */
    *puctxDst++ = *puctxSrc++; /* R8 */
    *puctxDst++ = *puctxSrc++; /* R9 */
    *puctxDst++ = *puctxSrc++; /* R10 */
    *puctxDst++ = *puctxSrc++; /* R11 */
    *puctxDst++ = *puctxSrc++; /* R12 */
    *puctxDst++ = *puctxSrc++; /* Sp */
    *puctxDst++ = *puctxSrc++; /* R14 */
    *puctxDst++ = *puctxSrc++; /* R15 */
}
/*==============================================================================
 * - L4_OSTaskSwitch()
 *
 * - L4 or OS called at switch task
 */
void L4_OSTaskSwitch (int *puctx)
{
#define  UCTX_REGS_SIZE  68
#define  UCTX_OFFSET_SP  14

    if (G_p_current_tcb != NULL) {
        G_p_current_tcb->sp  = *(puctx + UCTX_OFFSET_SP); /* save sp */
        G_p_current_tcb->sp -= UCTX_REGS_SIZE; /* space for regs */
        __uctxCopy((int *)G_p_current_tcb->sp, puctx); /* save regs */
    }

    if (*G_pp_highest_tcb != NULL) {
        G_p_current_tcb = *G_pp_highest_tcb; /* get new task */
    }

    __uctxCopy(puctx, (int *)G_p_current_tcb->sp); /* restore regs */
    G_p_current_tcb->sp += UCTX_REGS_SIZE; /* pop regs */
}

/*==============================================================================
 * - L4_OSIntEntry()
 *
 * - Tick timer ISR
 */
int L4_OSIntEntry (int  iVec, int  *puctx)
{
    tick_announce();
    L4_OSTaskSwitch(puctx);
    return 0;
}

/*==============================================================================
** FILE END
==============================================================================*/

