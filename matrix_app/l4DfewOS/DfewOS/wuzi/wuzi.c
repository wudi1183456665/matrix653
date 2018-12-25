/*==============================================================================
** wuzi.c -- Wuzi Qi Game. <white> ---- <black>
**
** MODIFY HISTORY:
**
** 2012-04-17 wdf Create.
==============================================================================*/
#include "../string.h" /* memset */
#include "wuzi.h"

/*======================================================================
  Constant
======================================================================*/
#define ROW_NUM                 WUZI_ROW_NUM    /* just for short */
#define COL_NUM                 WUZI_COL_NUM
#define CHESSMAN_NUM            (ROW_NUM * COL_NUM)
#define WIN_CASE_NUM            (ROW_NUM * (COL_NUM - 4) + \
                                 (ROW_NUM - 4) * COL_NUM + \
                                 (ROW_NUM - 4) * (COL_NUM - 4) + \
                                 (ROW_NUM - 4) * (COL_NUM - 4))
/*======================================================================
  Chessman
======================================================================*/
#define CHESSMAN_WHITE          0   /* ������ */
#define CHESSMAN_BLACK          1   /* ������ */
#define CHESSMAN_NONE           2   /* ������ */

/*======================================================================
  Occupy
======================================================================*/
#define WIN_NEVER               6   /* �����־ */

/*======================================================================
  Helper
======================================================================*/
#define FOR_EACH_WIN_CASE(i)    for (i = 0; i < WIN_CASE_NUM; i++)
#define FOR_EACH_ROW(row)       for (row = 0; row < ROW_NUM; row++)
#define FOR_EACH_COL(col)       for (col = 0; col < COL_NUM; col++)

/*======================================================================
  ��¼��ǰ�����ϵ����Ӹ��� [0 -- CHESSMAN_NUM]
======================================================================*/
int  _G_chessman_num;

/*======================================================================
  ��¼��ǰ˫�����Ӱڷ� [0, 1, 2]
  CHESSMAN_NONE  : ��λ��������
  CHESSMAN_WHITE : white �����Ӱڷ��ڴ�λ��
  CHESSMAN_BLACK : black �����Ӱڷ��ڴ�λ��
======================================================================*/
char _G_chessboard[ROW_NUM][COL_NUM];

/*======================================================================
  ��¼ white �� black ��ÿ�ֻ�ʤ��ʽ���Ѿ��ڷŵ������� �� �����־
  [0, 1, 2, 3, 4, 5, WIN_NEVER]
  0 -- 5    : white �� black ���ڸû�ʤ��ʽ�аڷ��� 0 -- 5 ������
  WIN_NEVER : white �� black ���ڸû�ʤ��ʽ���Ѿ��޷���ȡʤ
======================================================================*/
unsigned char _G_occupy_num[2][WIN_CASE_NUM];

/*======================================================================
  ÿһ�ֻ�ʤ��ʽ����������ռ�ݵ�λ�� [0, 1]
  0 : ���ֻ�ʤ��ʽ��ռ�ݴ�λ��
  1 : ���ֻ�ʤ��ʽռ�ݴ�λ��
======================================================================*/
unsigned char _G_win_case_table[ROW_NUM][COL_NUM][WIN_CASE_NUM];

/*==============================================================================
 * - wuzi_start()
 *
 * - initialize global varibles
 */
void wuzi_start ()
{
    static int initialized =0;
	int i;
    int row, col;
    int win_case_index = 0;

    /*
     * clear
     */
    _G_chessman_num = 0;
    memset (_G_chessboard, CHESSMAN_NONE, sizeof(_G_chessboard)); /* ����״̬�� 0 */
    memset (_G_occupy_num, 0, sizeof(_G_occupy_num)); /* ռ����Ŀ�� 0 */

    if (initialized == 1) {
        return;
    }

    /*
     * init win case table only once
     */
    FOR_EACH_ROW(row) { /* �趨ˮƽ����Ļ�ʤλ�� */
        for (col = 0; col < COL_NUM - 4; col++) {
            for (i = 0; i < 5; i++) {
                _G_win_case_table[row][col + i][win_case_index] = 1;
            }
            win_case_index++; /* ������һ�ֻ�ʤ��ʽ */
        }
    }
    /* �趨��ֱ����Ļ�ʤλ�� */
    FOR_EACH_COL(col) {
        for (row = 0; row < ROW_NUM - 4; row++) {
            for (i = 0; i < 5; i++) {
                _G_win_case_table[row + i][col][win_case_index] = 1;
            }
            win_case_index++; /* ������һ�ֻ�ʤ��ʽ */
        }
    
    }
    /* �趨���Խ��߷���Ļ�ʤλ�� */
    for (row = 0; row < ROW_NUM - 4; row++) {
        for (col = 0; col < COL_NUM - 4; col++) {
            for (i = 0; i < 5; i++) {
                _G_win_case_table[row + i][col + i][win_case_index] = 1;
            }
            win_case_index++; /* ������һ�ֻ�ʤ��ʽ */
        }
    }
    /* �趨���Խ��߷���Ļ�ʤλ�� */
    for (row = 0; row < ROW_NUM - 4; row++) {
        for (col = COL_NUM - 1; col >= 4; col--) {
            for (i = 0; i < 5; i++) {
                _G_win_case_table[row + i][col - i][win_case_index] = 1;
            }
            win_case_index++; /* ������һ�ֻ�ʤ��ʽ */
        }
    }

    initialized = 1;
}

/*==============================================================================
 * - _put_a_chessman()
 *
 * - put a chessman on (row, col).
 *   <chessman> must be [CHESSMAN_WHITE] or [CHESSMAN_BLACK]
 */
static int _put_a_chessman (int chessman, int row, int col)
{
    int i;

    /* �Ƿ񳬳����̷�Χ */
	if (row >= ROW_NUM || col >= COL_NUM || row < 0 || col < 0) {
		return WUZI_NONLEGAL;
	}

    /* �Ƿ��Ѿ���ռ�� */
    if (_G_chessboard[row][col] != CHESSMAN_NONE) {
        return WUZI_NONLEGAL;
    }

    /* ���� white �� black ÿһ�ֻ�ʤ������Ѿ��������Ӹ��� */
    FOR_EACH_WIN_CASE(i) {
        if (_G_win_case_table[row][col][i]) {

            /* chessman �ڵ� i �ֻ�ʤ��ʽ���Ѿ��������Ӹ����� 1 */
            if (_G_occupy_num[chessman][i] != WIN_NEVER) {
                _G_occupy_num[chessman][i]++;

                /* chessman ��ʤ */
                if (_G_occupy_num[chessman][i] == 5) {
                    return chessman;
                }
            }

            /* ��һ���ڵ� i �ֻ�ʤ��ʽ�в������ٻ�ʤ, ������Ϊ WIN_NEVER */
            _G_occupy_num[1 - chessman][i] = WIN_NEVER;   
        }
    }
    
    _G_chessman_num++;
    _G_chessboard[row][col] = chessman; /* ���� chessman ������ */

    if (_G_chessman_num == CHESSMAN_NUM) {
        return WUZI_DOGFALL; /* ƽ�� */
    }

    return WUZI_GO_AHEAD;
}

/*==============================================================================
 * - _player_do()
 *
 * - player put his chessman at <row, col>
 */
static int _player_do (int row, int col)
{
    return wuzi_white_do (row, col);
}

/*==============================================================================
 * - _computer_do()
 *
 * - computer reply at <*p_row, *p_col>
 */
static int _computer_do (int *p_row, int *p_col)
{
    int i, j;
	int k;
    int score[7] = {1, 5, 50, 100, 400, 0, 0};
    int player_grades[ROW_NUM][COL_NUM];
    int comput_grades[ROW_NUM][COL_NUM];
    int max_player_grade = -1;
    int max_comput_grade = -1;
    int defend_row, defend_col;
    int attack_row, attack_col;
    
    /* ���� player �� computer �ڿո����ϵķ��� */
    FOR_EACH_ROW(i) {
        FOR_EACH_COL(j) {

            player_grades[i][j] = 0;
            comput_grades[i][j] = 0;

            if (_G_chessboard[i][j] == CHESSMAN_NONE) { /* λ��i,jΪ�� */
                /* ���� player �� computer �ڿո����ϵķ��� */
                FOR_EACH_WIN_CASE(k) {
                    if (_G_win_case_table[i][j][k]) { /* λ��i,j�ڵ�k�ֻ�ʤ��ʽ�� */
                        player_grades[i][j] += score[_G_occupy_num[CHESSMAN_WHITE][k]];
                        comput_grades[i][j] += score[_G_occupy_num[CHESSMAN_BLACK][k]];
                    } 
                }

                /* ���� player �� computer �ڿո����ϵ������� */
                if (player_grades[i][j] > max_player_grade) {
                    max_player_grade = player_grades[i][j];   
                    defend_row = i;
                    defend_col = j;
                }
                if (comput_grades[i][j] > max_comput_grade) {
                    max_comput_grade = comput_grades[i][j];   
                    attack_row = i;
                    attack_col = j;
                }
            }
        }
    }

    /* �õ� computer ��Ӧ��λ�� */
    if ((max_player_grade >= max_comput_grade) && (max_player_grade > 100)) { /* ���� */
        *p_row = defend_row;
        *p_col = defend_col;
    } else { /* ���� */
        *p_row = attack_row;
        *p_col = attack_col;
    }

    return wuzi_black_do (*p_row, *p_col);
}

/*==============================================================================
 * - wuzi_white_do()
 *
 * - white put his chessman on (row, col)
 */
int wuzi_white_do (int row, int col)
{
    return _put_a_chessman (CHESSMAN_WHITE, row, col);
}

/*==============================================================================
 * - wuzi_black_do()
 *
 * - black put her chessman on (row, col)
 */
int wuzi_black_do (int row, int col)
{
    return _put_a_chessman (CHESSMAN_BLACK, row, col);
}

/*==============================================================================
 * - wuzi_round_do()
 *
 * - do a player & computer round
 */
int wuzi_round_do (int row, int col, int *p_row, int *p_col)
{
    int chess_status;

    /* ���� player �岽 */
    chess_status = _player_do (row, col);
    if (chess_status != WUZI_GO_AHEAD) {
        return chess_status;
    }

    /* computer ��Ӧ�岽 */
    return _computer_do (p_row, p_col);
}

/*==============================================================================
** FILE END
==============================================================================*/

#if 0 /* Test In VC++6.0*/
#include <stdio.h>
int main ()
{
    int row, col;
    int chess_status;
    char *msg[3] = {"��һ�ʤ", "�����ʤ", "ƽ��"};

_start:

    wuzi_start ();
	chess_status = WUZI_GO_AHEAD;
	
    while (chess_status == WUZI_GO_AHEAD ||
           chess_status == WUZI_NONLEGAL) {
        scanf ("%d %d", &row, &col);
        chess_status = wuzi_round_do (row, col, &row, &col);

        if (chess_status != WUZI_NONLEGAL) {
            printf ("computer reply : (%d, %d)\n", row, col);
        } else {
            printf ("(%d, %d) �ѱ�ռ��!\n", row, col);
        }
    }
    printf ("%s\n", msg[chess_status]);

    goto _start;

    return 0;
}
#endif
