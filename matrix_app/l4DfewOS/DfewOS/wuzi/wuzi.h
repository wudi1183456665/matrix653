/*==============================================================================
** wuzi.h -- 
**
** MODIFY HISTORY:
**
** 2012-04-17 wdf Create.
==============================================================================*/

#ifndef __WUZI_H__
#define __WUZI_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*======================================================================
  Configs
======================================================================*/
#define WUZI_ROW_NUM                 15
#define WUZI_COL_NUM                 15

/*======================================================================
  The API wuzi_action return values
======================================================================*/
#define WUZI_WHITE_WIN               0                /* �׷�ƽ�� */
#define WUZI_BLACK_WIN               1                /* �ڷ�ƽ�� */
#define WUZI_DOGFALL                 2                /* ˫��ƽ�� */
#define WUZI_GO_AHEAD                3                /* ��ּ��� */
#define WUZI_NONLEGAL                4                /* �Ƿ����� */
#define WUZI_PLAYER_WIN              WUZI_WHITE_WIN   /* ��һ�ʤ */
#define WUZI_COMPUT_WIN              WUZI_BLACK_WIN   /* �����ʤ */


/*======================================================================
  Wuzi Moudle APIs
======================================================================*/
void wuzi_start ();
int  wuzi_round_do (int row, int col, int *p_row, int *p_col);
int  wuzi_white_do (int row, int col);
int  wuzi_black_do (int row, int col);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WUZI_H__ */

/*==============================================================================
** FILE END
==============================================================================*/

