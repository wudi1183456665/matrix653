/*********************************************************************************************************
**
**                                    �й������Դ��֯
**
**                                Ƕ��ʽ L4 ΢�ں˲���ϵͳ
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**
** ��   ��   ��: list.h
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 01 �� 09 ��
**
** ��        ��: ����ϵͳʹ�õ������������ͼ���������.
*********************************************************************************************************/

#ifndef __LIST_H
#define __LIST_H

/*********************************************************************************************************
  ˫��������ڵ�, ͷ���Ͷ���. ���ھ���������
*********************************************************************************************************/
typedef struct __list_ring {
    struct __list_ring      *RING_plistNext;                            /*  ���α�ǰ��ָ��              */
    struct __list_ring      *RING_plistPrev;                            /*  ���α����ָ��              */
} MX_LIST_RING;
typedef MX_LIST_RING        *PMX_LIST_RING;
typedef PMX_LIST_RING        MX_LIST_RING_HEADER;                       /*  ���α��ͷ                  */
/*********************************************************************************************************
  ˫����������ڵ�, ͷ���Ͷ���. ����
*********************************************************************************************************/
typedef struct __list_line {
	struct __list_line      *LINE_plistNext;                            /*  ���α�ǰ��ָ��              */
    struct __list_line      *LINE_plistPrev;                            /*  ���α����ָ��              */
} MX_LIST_LINE;
typedef MX_LIST_LINE        *PMX_LIST_LINE;
typedef PMX_LIST_LINE        MX_LIST_LINE_HEADER;                       /*  ���α��ͷ                  */
/*********************************************************************************************************
  ��������ڵ�, ͷ���Ͷ���. ������Դ����
*********************************************************************************************************/
typedef struct __list_mono {
	struct __list_mono      *MONO_plistNext;                            /*  ��Դ��ǰ��ָ��              */
} MX_LIST_MONO;
typedef MX_LIST_MONO        *PMX_LIST_MONO;
typedef PMX_LIST_MONO        MX_LIST_MONO_HEADER;                       /*  ��Դ���ͷ                  */

#if 0
/*********************************************************************************************************
  ��ϣ����ڵ�, ͷ���Ͷ���. ����
*********************************************************************************************************/
typedef struct __hlist_node {
    struct __hlist_node     *HNDE_phndeNext;                            /*  ǰ��ָ��                    */
    struct __hlist_node    **HNDE_pphndePrev;                           /*  ����˫ָ��                  */
} MX_HLIST_NODE;
typedef MX_HLIST_NODE       *PMX_HLIST_NODE;                            /*  �ڵ�ָ������                */

typedef struct __hlist_head {
    struct __hlist_node     *HLST_phndeFirst;                           /*  ��һ���ڵ�                  */
} MX_HLIST_HEAD;
typedef MX_HLIST_HEAD       *PMX_HLIST_HEAD;                            /*  ͷָ������                  */
#endif

/*********************************************************************************************************
  ����ڵ��ʼ��ֵ
*********************************************************************************************************/
#define	_LIST_RING_NULL_NODE()	  { MX_NULL, MX_NULL }
#define _LIST_LINE_NULL_NODE()    { MX_NULL, MX_NULL }
#define _LIST_MONO_NULL_NODE()    { MX_NULL }
/*********************************************************************************************************
  ����ڵ�ṹ��ʼ��
*********************************************************************************************************/
#define	_LIST_RING_INIT_NODE(name) do {                       \
            (name).RING_plistNext = MX_NULL;                  \
            (name).RING_plistPrev = MX_NULL;                  \
        } while (0)
#define _LIST_LINE_INIT_NODE(name) do {                       \
            (name).LINE_plistNext = MX_NULL;                  \
            (name).LINE_plistPrev = MX_NULL;                  \
        } while (0)
#define _LIST_MONO_INIT_NODE(name) do {                       \
            (name).MONO_plistNext = MX_NULL;                  \
        } while (0)
/*********************************************************************************************************
  ����ڵ�ָ���ʼ��
*********************************************************************************************************/
#define _LIST_RING_INIT_NODE_PTR(ptr) do {                    \
            (ptr)->RING_plistNext = MX_NULL;                  \
            (ptr)->RING_plistPrev = MX_NULL;                  \
        } while (0)
#define _LIST_LINE_INIT_NODE_PTR(ptr) do {                    \
            (ptr)->LINE_plistNext = MX_NULL;                  \
            (ptr)->LINE_plistPrev = MX_NULL;                  \
        } while (0)
#define _LIST_MONO_INIT_NODE_PTR(ptr) do {                    \
            (ptr)->MONO_plistNext = MX_NULL;                  \
        } while (0)
/*********************************************************************************************************
  ƫ��������, ��������ڵ��������ڵ�ƫ��
*********************************************************************************************************/
#define _LIST_OFFSETOF(type, member)                          \
        ((ULONG)&((type *)0)->member)
/*********************************************************************************************************
  �õ� ptr(����ڵ��ַ) �������ṹ
*********************************************************************************************************/
#define _LIST_CONTAINER_OF(ptr, type, member)                 \
        ((type *)((ULONG)ptr - _LIST_OFFSETOF(type, member)))
/*********************************************************************************************************
  �������ĸ��ָ��ṹ
*********************************************************************************************************/
#define _LIST_ENTRY(ptr, type, member)                        \
        _LIST_CONTAINER_OF(ptr, type, member)
/*********************************************************************************************************
  �ж������Ƿ�Ϊ��
*********************************************************************************************************/
#define _LIST_RING_IS_EMPTY(ptr)                              \
        ((ptr) == MX_NULL)
#define _LIST_LINE_IS_EMPTY(ptr)                              \
        ((ptr) == MX_NULL)
#define _LIST_MONO_IS_EMPTY(ptr)                              \
        ((ptr) == MX_NULL)
/*********************************************************************************************************
  �жϽڵ��Ƿ�������
*********************************************************************************************************/
#define _LIST_RING_IS_NOTLNK(ptr)                             \
        (((ptr)->RING_plistNext == MX_NULL) || ((ptr)->RING_plistPrev == MX_NULL))
#define _LIST_LINE_IS_NOTLNK(ptr)                             \
        (((ptr)->LINE_plistNext == MX_NULL) && ((ptr)->LINE_plistPrev == MX_NULL))
/*********************************************************************************************************
  ��Դ���ʼ������
*********************************************************************************************************/
#define _LIST_MONO_LINK(ptr, ptrnext)                         \
        ((ptr)->MONO_plistNext = (ptrnext))
/*********************************************************************************************************
  �ƶ�����һ��
*********************************************************************************************************/
static MX_INLINE VOID  _list_ring_next (MX_LIST_RING_HEADER  *phead)
{
    *phead = (*phead)->RING_plistNext;
}
static MX_INLINE VOID  _list_line_next (MX_LIST_LINE_HEADER  *phead)
{
    *phead = (*phead)->LINE_plistNext;
}
static MX_INLINE VOID  _list_mono_next (MX_LIST_MONO_HEADER  *phead)
{
    *phead = (*phead)->MONO_plistNext;
}
/*********************************************************************************************************
  ��ȡ��һ��
*********************************************************************************************************/
static MX_INLINE PMX_LIST_RING  _list_ring_get_next (PMX_LIST_RING  pring)
{
    return  (pring->RING_plistNext);
}
static MX_INLINE PMX_LIST_LINE  _list_line_get_next (PMX_LIST_LINE  pline)
{
    return  (pline->LINE_plistNext);
}
static MX_INLINE PMX_LIST_MONO  _list_mono_get_next (PMX_LIST_MONO  pmono)
{
    return  (pmono->MONO_plistNext);
}
/*********************************************************************************************************
  ��ȡ��һ��
*********************************************************************************************************/
static MX_INLINE PMX_LIST_RING  _list_ring_get_prev (PMX_LIST_RING  pring)
{
    return  (pring->RING_plistPrev);
}
static MX_INLINE PMX_LIST_LINE  _list_line_get_prev (PMX_LIST_LINE  pline)
{
    return  (pline->LINE_plistPrev);
}
/*********************************************************************************************************
  ��Դ����ط�������ղ���
*********************************************************************************************************/
static MX_INLINE PMX_LIST_MONO  _list_mono_alloc (MX_LIST_MONO_HEADER  *phead)
{
    REGISTER  PMX_LIST_MONO  pallo;
    
    pallo  = *phead;                                                    /*  ������ͷ��ȡһ���ڵ�        */
    *phead = (*phead)->MONO_plistNext;
    
    return  (pallo);
}
static MX_INLINE VOID           _list_mono_free (MX_LIST_MONO_HEADER  *phead, PMX_LIST_MONO  pfree)
{
    pfree->MONO_plistNext = *phead;
    *phead = pfree;                                                     /*  ���ڵ��������ͷ            */
}
/*********************************************************************************************************
  ˫�� RING ���������������
*********************************************************************************************************/
VOID  _List_Ring_Add_Ahead(PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader);
VOID  _List_Ring_Add_Front(PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader);
VOID  _List_Ring_Add_Last(PMX_LIST_RING   pringNew, MX_LIST_RING_HEADER  *ppringHeader);
VOID  _List_Ring_Del(PMX_LIST_RING  pringDel, MX_LIST_RING_HEADER  *ppringHeader);
/*********************************************************************************************************
  ˫�� LINE ���������������
*********************************************************************************************************/
VOID  _List_Line_Add_Ahead(PMX_LIST_LINE  plineNew, MX_LIST_LINE_HEADER  *pplineHeader);
VOID  _List_Line_Add_Tail(PMX_LIST_LINE   plineNew, MX_LIST_LINE_HEADER  *pplineHeader);
VOID  _List_Line_Add_Left(PMX_LIST_LINE   plineNew, PMX_LIST_LINE  plineRight);
VOID  _List_Line_Add_Right(PMX_LIST_LINE  plineNew, PMX_LIST_LINE  plineLeft);
VOID  _List_Line_Del(PMX_LIST_LINE  plineDel, MX_LIST_LINE_HEADER  *pplineHeader);

#endif                                                                  /*  __LIST_H                    */
/*********************************************************************************************************
  END
*********************************************************************************************************/

