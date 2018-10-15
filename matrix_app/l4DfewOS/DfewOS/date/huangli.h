/*==============================================================================
** huangli.h -- huang li interface.
**
** MODIFY HISTORY:
**
** 2016-11-08 wdf Create.
==============================================================================*/

#ifndef __HUANGLI_H__
#define __HUANGLI_H__

#ifdef __cplusplus
extern "C" {
#endif

/*======================================================================
  config
======================================================================*/
#define BEI_ZHU_LEN     64

/*======================================================================
  ũ�����ڽṹ��
======================================================================*/
typedef struct huangli_info {
    char yi[9];      /* "" */
    char bu_yi[5];    /* "" */
    char yun[3];     /* "" */
    char jie_qi[5];     /* "" */

} HUANGLI_INFO;


int huangli_get_info (int year, int month, int day, HUANGLI_DATE *pHuangli);

#ifdef __cplusplus
}
#endif

#endif /* __HUANGLI_H__ */

/*==============================================================================
** FILE END
==============================================================================*/