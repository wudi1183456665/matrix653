/*==============================================================================
** tty_snake.c -- a snake game in tty.
**
** MODIFY HISTORY:
**
** 2012-12-11 wdf Create.
==============================================================================*/

#include <dfewos.h>
#include "os.h"

/*======================================================================
  GUI Configs
======================================================================*/
#define NODE_ELEMENT    "||" /* block's node. it consist of two char */
#define COL_START       3
#define ROW_START       2
#define COL_NUM         38   /* left -- right width */   
#define ROW_NUM         23   /* top -- bottom width */

#define BLOCK_HEIGHT    1 
#define BLOCK_WIDTH     2

/*======================================================================
  KeyBoard Strings
======================================================================*/
#define KEY_ESC     "\033"
#define KEY_UP      "\033[A"
#define KEY_DOWN    "\033[B"
#define KEY_RIGHT   "\033[C"
#define KEY_LEFT    "\033[D"
#define KEY_BLANK   " "         /* quickly a step */
#define KEY_HOLD    "h"         /* freeze snake */
#define KEY_AUTO    "`"         /* the key below [ESC] and left [1] */


/*======================================================================
  Speed Configs
======================================================================*/
#define SNAKE_SPEED       2
#define ERASE_COLOR       0
#define HEAD_COLOR        3 /* »Æ */
#define BODY_COLOR        1 /* ºì */
#define FOOD_COLOR        2 /* ÂÌ */

/*======================================================================
  msgQ & task Configs
======================================================================*/
#define JOB_QUEUE_MAX     60
#define JOB_TASK_PRI      100
#define MOV_TASK_PRI      100

/*======================================================================
  job task can process these messages
======================================================================*/
typedef enum snake_msg {
    SNAKE_MSG_QUIT,
    SNAKE_MSG_UP,
    SNAKE_MSG_DOWN,
    SNAKE_MSG_RIGHT,
    SNAKE_MSG_LEFT,
    SNAKE_MSG_AUTO,
    SNAKE_MSG_MOVE,
    SNAKE_MSG_HOLD
} SNAKE_MSG;

/*======================================================================
  send message return values
======================================================================*/
typedef enum send_msg_status {
    SEND_OK,
    SEND_TIMEOUT = -1,
    SEND_GAME_OVER = -2
} SEND_MSG_STATUS;

/*======================================================================
  when the snake go ahead, may return below vaules
======================================================================*/
typedef enum motion_status {
    MOTION_STATUS_OK = 0,
    MOTION_STATUS_ERROR = -1,
    MOTION_STATUS_GAME_OVER = -2
} MOTION_STATUS;

/*======================================================================
  the NODE of snake
======================================================================*/
typedef struct snake_node {
    DL_NODE dlist_node;
    short x;
    short y;
} SNAKE_NODE;


static void _the_game ();



/*==============================================================================
 * - _show_node()
 *
 * - Show a snake node [__]
 */
static int _show_node (int x, int y, int show)
{
    if (show & 0x80000000) {  /* show score */
        int start_col;

        start_col = COL_START + ((COL_NUM * BLOCK_WIDTH) - 10) / 2;

        /* move cursor */
        serial_printf ("\033[%d;%dH", ROW_START - 1, start_col);

        /* set background & foreground color */
        serial_printf (COLOR_BG_BLACK);
        serial_printf (COLOR_FG_YELLOW);
        
        serial_printf ("Score: %d", show & 0x7FFFFFFF);

        return show;
    }

    /* move cursor */
    serial_printf ("\033[%d;%dH", y + ROW_START, x*2 + COL_START);

    /* set foreground color as BLACK */
    serial_printf (COLOR_FG_BLACK);

    /* set background color */
    serial_printf ("\033[%dm", 40 + show);

    /* show or erase node */
    serial_printf (NODE_ELEMENT);

    return show;
}

/*==============================================================================
 * - _draw_frame()
 *
 * - The Frame looks like this:
 *     +------------------+
 *     |                  |
 *     |                  |
 *     |                  |
 *     |                  |
 *     |                  |
 *     +------------------+
 */
static void _draw_frame ()
{
    int i ;

    /* move cursor to left top */
    /* +--------------+ */
    serial_printf ("\033[%d;%dH", ROW_START - 1, COL_START - 1);
    serial_putc ('+');
    for (i = 0; i < COL_NUM * BLOCK_WIDTH; i++) serial_putc ('-');
    serial_putc ('+');

    /* move cursor to left bottom */
    /* +--------------+ */
    serial_printf ("\033[%d;%dH", ROW_START + ROW_NUM * BLOCK_HEIGHT, COL_START - 1);
    serial_putc ('+');
    for (i = 0; i < COL_NUM * BLOCK_WIDTH; i++) serial_putc ('-');
    serial_putc ('+');

    /* draw left right line */
    /* |              | */
    for (i = 0; i < ROW_NUM * BLOCK_HEIGHT; i++) {
        serial_printf ("\033[%d;%dH", ROW_START+i, COL_START - 1);
        serial_putc ('|');
        serial_printf ("\033[%d;%dH", ROW_START+i, COL_START + COL_NUM * BLOCK_WIDTH);
        serial_putc ('|');
    }
}

/*==============================================================================
 * - snake_main()
 *
 * - Main
 */
void snake_main (int argc, char **argv)
{
    /* clear screen and hide cursor */
    serial_printf (CLEAR_SCREEN);
    serial_printf (HIDE_CURSOR);

    /* draw frame */
    _draw_frame ();

    /* start snake */
    _the_game ();

    /* clear screen, show cursor and set cmd mode */
    serial_printf (COLOR_FG_WHITE);
    serial_printf (COLOR_BG_BLACK);
    serial_printf (CLEAR_SCREEN);
    serial_printf (SHOW_CURSOR);
}
    
/*==============================================================================
** tty function END
==============================================================================*/


/*======================================================================
  the global variables
======================================================================*/
static DL_LIST         _G_snake;
static SNAKE_NODE      _G_food;
static int             _G_score;
static OS_MSG_Q_ID     _G_msgQ;

/*==============================================================================
 * - _send_msg()
 *
 * - send a message to job queue
 *
 * -        SEND_OK: send to the message queue success
 *     SEND_TIMEOUT: the message queue is full no sapce for new message
 *   SEND_GAME_OVER: the message queue is delete by _job_task
 */
int _send_msg (SNAKE_MSG msg)
{
    SEND_MSG_STATUS  status = SEND_OK;

    int os_send_ret = os_msgQ_send (_G_msgQ, (char *)&msg, sizeof (SNAKE_MSG));
    if (os_send_ret == -1) {
        status = SEND_TIMEOUT;
    } else if (os_send_ret == -2) {
        status = SEND_GAME_OVER;
    }
        
    return status;
}


/*==============================================================================
 * - _recv_msg()
 *
 * - receive message from job queue. Only used by _job_task
 */
static SNAKE_MSG _recv_msg ()
{
    SNAKE_MSG msg_job;

    os_msgQ_receive (_G_msgQ, (char *)&msg_job, sizeof (SNAKE_MSG));

    return msg_job;
}

/*==============================================================================
 * - _quit()
 *
 * - game over
 */
static void _quit ()
{
    /* free something */
    SNAKE_NODE *pNode = (SNAKE_NODE *)dlist_get (&_G_snake);
    while (pNode != NULL) {
        memH_free (pNode);
        pNode = (SNAKE_NODE *)dlist_get (&_G_snake);
    }

    /* delete job_queue */
    os_msgQ_delete (_G_msgQ);
    _G_msgQ = NULL;
}
#if 0
/*==============================================================================
 * - _eat_process()
 *
 * - show the eating food effect
 */
static int _eat_process ()
{
    int delay = 100 / dlist_count (&_G_snake);
    SNAKE_NODE *pNode = (SNAKE_NODE *)DL_FIRST(&_G_snake);

    pNode = (SNAKE_NODE *)DL_NEXT(pNode);

    while (pNode != NULL) {
        _show_node (pNode->x, pNode->y, FOOD_COLOR);
        os_task_delay (delay);
        _show_node (pNode->x, pNode->y, BODY_COLOR);

        pNode = (SNAKE_NODE *)DL_NEXT(pNode);
    }
    return 0;
}
#endif
/*==============================================================================
 * - _is_occupied()
 *
 * - on screen whether this coordinate is occupied
 *
 * - return value
 *   0: not occupied
 *  !0: occupied
 */
static int _is_occupied (short x, short y)
{
    SNAKE_NODE *pNode = (SNAKE_NODE *)DL_FIRST(&_G_snake);

    while (pNode != NULL) {
        if ((pNode->x == x) && (pNode->y == y)) {
            return 1;
        }
        pNode = (SNAKE_NODE *)DL_NEXT(pNode);
    }
    return 0;
}

/*==============================================================================
 * - _is_food()
 *
 * - is food here
 */
static int _is_food (short x, short y)
{
    if ((_G_food.x == x) && (_G_food.y == y)) {
        return 1;
    } else {
        return 0;
    }
}

/*==============================================================================
 * - _create_food()
 *
 * - create food
 */
static void _create_food ()
{
    short x, y;

    do {
        x = rand() % COL_NUM;
        y = rand() % ROW_NUM;
    } while (_is_occupied (x, y));

    _G_food.x = x;
    _G_food.y = y;

    _show_node(x, y, FOOD_COLOR);
}

/*==============================================================================
 * - _go_ahead()
 *
 * - the snake try to go ahead
 *
 * - return value
 *   MOTION_STATUS_OK:        ok
 *   MOTION_STATUS_ERROR:     can't go ahead 
 *   MOTION_STATUS_GAME_OVER: game over
 */
static int _go_ahead (int dir)
{
    static short X_GO[4] = {-1, 1, 0, 0};
    static short Y_GO[4] = {0, 0, -1, 1};
    short x, y;

    SNAKE_NODE *head = (SNAKE_NODE *)DL_FIRST(&_G_snake);
    SNAKE_NODE *tail = (SNAKE_NODE *)DL_LAST(&_G_snake);

    /* hide tail node */
    _show_node (tail->x, tail->y, ERASE_COLOR);

    /* new head position */
    x = (head->x + X_GO[dir]);
    y = (head->y + Y_GO[dir]);
    x = (x < 0) ? (COL_NUM - 1) : (x % COL_NUM);
    y = (y < 0) ? (ROW_NUM - 1) : (y % ROW_NUM);

    /* show new head */
    _show_node (x, y, HEAD_COLOR);

    /* remove tail node from the list */
    dlist_remove (&_G_snake, &tail->dlist_node);

    /* after remove tail, the head is */
    head = (SNAKE_NODE *)DL_FIRST(&_G_snake);
    if (head != NULL) _show_node (head->x, head->y, BODY_COLOR);

    if (_is_occupied (x, y)) {
        memH_free (tail); /* because this node not in the list, so must be free here */
        return MOTION_STATUS_GAME_OVER;
    } else if (_is_food (x, y)) {
        /* grow up a node */
        SNAKE_NODE *grow = (SNAKE_NODE *)memH_malloc (sizeof(SNAKE_NODE));
        grow->x = tail->x;
        grow->y = tail->y;
        /* add grow node to the list tail */
        dlist_add (&_G_snake, &grow->dlist_node);
        _show_node (grow->x, grow->y, BODY_COLOR);


        //_eat_process();


        /* insert new head node to the list head */
        tail->x = x;
        tail->y = y;
        dlist_insert (&_G_snake, NULL, &tail->dlist_node);

        /* create and show food */
        _create_food();

        /* increase and show score */
        _G_score++;
        _show_node(0, 0, 0x80000000 | _G_score);
    } else {
        /* insert new head node to the list head */
        tail->x = x;
        tail->y = y;
        dlist_insert (&_G_snake, NULL, &tail->dlist_node);
    }

    return MOTION_STATUS_OK;
}

/*==============================================================================
 * - _job_task()
 *
 * - a task that receive SNAKE_MSG and process
 */
static void _job_task ()
{
    SNAKE_MSG msg_job;
    int hold_on = 0;
    int in_auto_mode = 0;
    int dirction = 0;

/* let the snake try to go ahead, if can't, over this task */
#define _GO_AHEAD(dir) \
    { \
        if (hold_on) { break; } \
        dirction = dir; \
        if (_go_ahead (dir) == MOTION_STATUS_GAME_OVER) { \
            goto game_over; \
        } \
    }

    while (1) {
        msg_job = _recv_msg ();

        switch (msg_job) {
            case SNAKE_MSG_LEFT:
                if (dirction != 1) _GO_AHEAD(0);
                break;
            case SNAKE_MSG_RIGHT:
                if (dirction != 0) _GO_AHEAD(1);
                break;
            case SNAKE_MSG_UP:
                if (dirction != 3) _GO_AHEAD(2);
                break;
            case SNAKE_MSG_DOWN:
                if (dirction != 2) _GO_AHEAD(3);
                break;
            case SNAKE_MSG_MOVE:
                if (!in_auto_mode) _GO_AHEAD(dirction);
                break;
            case SNAKE_MSG_AUTO:
                in_auto_mode = 1 - in_auto_mode;
                break;
            case SNAKE_MSG_HOLD:
                hold_on = 1 - hold_on;
                break;
            case SNAKE_MSG_QUIT:
                goto game_over;
            default:
                break;
        }
    }

game_over:
    _quit ();
}

/*==============================================================================
 * - _mov_task()
 *
 * - a task that send SNAKE_MSG_DOWN regularly
 */
static void _mov_task ()
{
    while (1) {
        if (_send_msg (SNAKE_MSG_MOVE) == SEND_GAME_OVER) {
            break;
        }
        os_task_delay (1000 / SNAKE_SPEED);
    }
}



/*==============================================================================
 * - _init_game()
 *
 * - init the game need msgQ & Tasks
 */
static void _init_game ()
{
    SNAKE_NODE *first = (SNAKE_NODE *)memH_malloc (sizeof(SNAKE_NODE));
    dlist_init (&_G_snake);

    /* create msgQ */
    _G_msgQ = os_msgQ_create (JOB_QUEUE_MAX, sizeof (SNAKE_MSG));

    /* create do job task */
    os_task_create ("tS_job", 40960, JOB_TASK_PRI, (void *)_job_task, 0, 0);
    os_task_create ("tS_mov", 10240, MOV_TASK_PRI, (void *)_mov_task, 0, 0);

    /* show the snake baby */
    first->x = COL_NUM / 2;
    first->y = ROW_NUM / 2;
    dlist_add (&_G_snake, &first->dlist_node);
    _show_node (first->x, first->y, BODY_COLOR);

    /* create food */
    _create_food();

    /* clear score */
    _G_score = 0;
}

/*==============================================================================
 * - _the_game()
 *
 * - play the game
 */
static void _the_game ()
{
    int  nread;
    char key[4];

    _init_game ();

/* try to send a message to job queue, if failed, quit this game */
#define _SEND_MSG(m)   if (_send_msg (m) == SEND_GAME_OVER) { break;}

    FOREVER {
        nread = serial_read (key, 3);
        key[nread] = '\0';

        if (strcmp (key, KEY_DOWN) == 0) {          /* Down */
            _SEND_MSG(SNAKE_MSG_DOWN);
        } else if (strcmp (key, KEY_BLANK) == 0) {  /* ' ': blank */
            _SEND_MSG(SNAKE_MSG_MOVE);
        } else if (strcmp (key, KEY_UP) == 0) {     /* Up */
            _SEND_MSG(SNAKE_MSG_UP);
        } else if (strcmp (key, KEY_LEFT) == 0) {   /* Left */
            _SEND_MSG(SNAKE_MSG_LEFT);
        } else if (strcmp (key, KEY_RIGHT) == 0) {  /* Right */
            _SEND_MSG(SNAKE_MSG_RIGHT);
        } else if (strcmp (key, KEY_AUTO) == 0) {   /* '`': auto */
            _SEND_MSG(SNAKE_MSG_AUTO);
        } else if (strcmp (key, KEY_HOLD) == 0) {   /* 'h': hold on */
            _SEND_MSG(SNAKE_MSG_HOLD);
        } else if (strcmp (key, KEY_ESC) == 0) {    /* Esc: quit */
            _SEND_MSG(SNAKE_MSG_QUIT);
            break;
        }
    }

    /* 
     * wait job task process the SNAKE_MSG_QUIT massage over
     */
    while (1) {
        _SEND_MSG(SNAKE_MSG_QUIT);
        delayQ_delay (SYS_CLK_RATE / 10);
    }
}

/*==============================================================================
** FILE END
==============================================================================*/

