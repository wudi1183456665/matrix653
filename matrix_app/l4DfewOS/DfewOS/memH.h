/*==============================================================================
** memH.h -- the memory heap module header.
**
** MODIFY HISTORY:
**
** 2016-10-27 wdf Create.
==============================================================================*/

#ifndef __MEMH_H__
#define __MEMH_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*======================================================================
  memH module support routines for other modules
======================================================================*/
void *memH_malloc (size_t nbytes);
void *memH_calloc (size_t elem_num, size_t elem_size);
void *memH_realloc (void *pBlock, size_t newSize);
void  memH_free (void *usr_p);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __MEMH_H__ */

/*==============================================================================
** FILE END
==============================================================================*/

