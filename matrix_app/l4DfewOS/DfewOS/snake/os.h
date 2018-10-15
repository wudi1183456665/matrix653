

#include <dfewos.h>
#include "../string.h"
#define OS_TASK_ID          OS_TCB*
#define OS_TASK_ID_ERROR    NULL
#define OS_MSG_Q_ID         MSG_QUE*



/*======================================================================
  DfewOS TASK
======================================================================*/
static inline OS_TASK_ID  os_task_create(char         *name,
                           unsigned int  stack_size,
                           unsigned char task_priority,
                           void         *task_entry,
                           int arg1, int arg2)
{
    return task_create(name, stack_size, task_priority, (FUNC_PTR)task_entry, arg1, arg2);
}

static inline void os_task_delay (int ms)
{
    int ticks = SYS_CLK_RATE * ms / 1000;
    delayQ_delay(ticks);
}

static inline void os_task_delete (OS_TASK_ID task_id)
{
	task_delete (task_id);
}

/*======================================================================
  DfewOS MSG_Q
======================================================================*/
static inline OS_MSG_Q_ID os_msgQ_create (int max_num, int max_length)
{
    return msgQ_init(NULL, max_num, max_length);
}

static inline int os_msgQ_send (OS_MSG_Q_ID msgQ_id, char *msg_buf, int msg_len)
{
    if (msgQ_id == NULL) {
        return -2;
    }

    msgQ_send (msgQ_id, msg_buf, msg_len, NO_WAIT);

    return 0;
}

static inline void os_msgQ_receive (OS_MSG_Q_ID msgQ_id, char *buf, int buf_len)
{
    msgQ_receive (msgQ_id, buf, buf_len, WAIT_FOREVER);
}

static inline void os_msgQ_delete(OS_MSG_Q_ID msgQ_id)
{
    msgQ_delete (msgQ_id);
}

static inline void os_msgQ_flush(OS_MSG_Q_ID msgQ_id)
{
    while (msgQ_receive (msgQ_id, NULL, 0, NO_WAIT) == OS_STATUS_OK);
}

/*======================================================================
  DfewOS TICK
======================================================================*/
static inline int os_tick_get()
{
    return tick_get();
}
