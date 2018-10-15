/*********************************************************************************************************
**
**                                    中国软件开源组织
**
**                                嵌入式 L4 微内核操作系统
**
**                                SylixOS(TM)  MX : matrix
**
**                               Copyright All Rights Reserved
**
**--------------文件信息--------------------------------------------------------------------------------
**
** 文   件   名: list.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 09 日
**
** 描        述: 这是系统使用的所有链表类型及操作定义.
*********************************************************************************************************/

#ifndef __LIST_H
#define __LIST_H

/*********************************************************************************************************
  双向环形链表节点, 头类型定义. 用在就绪队列中
*********************************************************************************************************/
typedef struct __list_ring {
    struct __list_ring      *RING_plistNext;                            /*  环形表前向指针              */
    struct __list_ring      *RING_plistPrev;                            /*  环形表后向指针              */
} MX_LIST_RING;
typedef MX_LIST_RING        *PMX_LIST_RING;
typedef PMX_LIST_RING        MX_LIST_RING_HEADER;                       /*  环形表表头                  */
/*********************************************************************************************************
  双向线形链表节点, 头类型定义. 用在
*********************************************************************************************************/
typedef struct __list_line {
	struct __list_line      *LINE_plistNext;                            /*  线形表前向指针              */
    struct __list_line      *LINE_plistPrev;                            /*  线形表后向指针              */
} MX_LIST_LINE;
typedef MX_LIST_LINE        *PMX_LIST_LINE;
typedef PMX_LIST_LINE        MX_LIST_LINE_HEADER;                       /*  线形表表头                  */
/*********************************************************************************************************
  单向链表节点, 头类型定义. 用在资源分配
*********************************************************************************************************/
typedef struct __list_mono {
	struct __list_mono      *MONO_plistNext;                            /*  资源表前向指针              */
} MX_LIST_MONO;
typedef MX_LIST_MONO        *PMX_LIST_MONO;
typedef PMX_LIST_MONO        MX_LIST_MONO_HEADER;                       /*  资源表表头                  */

#if 0
/*********************************************************************************************************
  哈希链表节点, 头类型定义. 用在
*********************************************************************************************************/
typedef struct __hlist_node {
    struct __hlist_node     *HNDE_phndeNext;                            /*  前向指针                    */
    struct __hlist_node    **HNDE_pphndePrev;                           /*  后向双指针                  */
} MX_HLIST_NODE;
typedef MX_HLIST_NODE       *PMX_HLIST_NODE;                            /*  节点指针类型                */

typedef struct __hlist_head {
    struct __hlist_node     *HLST_phndeFirst;                           /*  第一个节点                  */
} MX_HLIST_HEAD;
typedef MX_HLIST_HEAD       *PMX_HLIST_HEAD;                            /*  头指针类型                  */
#endif

/*********************************************************************************************************
  链表节点初始化值
*********************************************************************************************************/
#define	_LIST_RING_NULL_NODE()	  { MX_NULL, MX_NULL }
#define _LIST_LINE_NULL_NODE()    { MX_NULL, MX_NULL }
#define _LIST_MONO_NULL_NODE()    { MX_NULL }
/*********************************************************************************************************
  链表节点结构初始化
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
  链表节点指针初始化
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
  偏移量计算, 计算链表节点在容器内的偏移
*********************************************************************************************************/
#define _LIST_OFFSETOF(type, member)                          \
        ((ULONG)&((type *)0)->member)
/*********************************************************************************************************
  得到 ptr(链表节点地址) 的容器结构
*********************************************************************************************************/
#define _LIST_CONTAINER_OF(ptr, type, member)                 \
        ((type *)((ULONG)ptr - _LIST_OFFSETOF(type, member)))
/*********************************************************************************************************
  获得链表母体指针结构
*********************************************************************************************************/
#define _LIST_ENTRY(ptr, type, member)                        \
        _LIST_CONTAINER_OF(ptr, type, member)
/*********************************************************************************************************
  判断链表是否为空
*********************************************************************************************************/
#define _LIST_RING_IS_EMPTY(ptr)                              \
        ((ptr) == MX_NULL)
#define _LIST_LINE_IS_EMPTY(ptr)                              \
        ((ptr) == MX_NULL)
#define _LIST_MONO_IS_EMPTY(ptr)                              \
        ((ptr) == MX_NULL)
/*********************************************************************************************************
  判断节点是否不链表中
*********************************************************************************************************/
#define _LIST_RING_IS_NOTLNK(ptr)                             \
        (((ptr)->RING_plistNext == MX_NULL) || ((ptr)->RING_plistPrev == MX_NULL))
#define _LIST_LINE_IS_NOTLNK(ptr)                             \
        (((ptr)->LINE_plistNext == MX_NULL) && ((ptr)->LINE_plistPrev == MX_NULL))
/*********************************************************************************************************
  资源表初始化连接
*********************************************************************************************************/
#define _LIST_MONO_LINK(ptr, ptrnext)                         \
        ((ptr)->MONO_plistNext = (ptrnext))
/*********************************************************************************************************
  移动到下一个
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
  获取下一个
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
  获取上一个
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
  资源缓冲池分配与回收操作
*********************************************************************************************************/
static MX_INLINE PMX_LIST_MONO  _list_mono_alloc (MX_LIST_MONO_HEADER  *phead)
{
    REGISTER  PMX_LIST_MONO  pallo;
    
    pallo  = *phead;                                                    /*  从链表头中取一个节点        */
    *phead = (*phead)->MONO_plistNext;
    
    return  (pallo);
}
static MX_INLINE VOID           _list_mono_free (MX_LIST_MONO_HEADER  *phead, PMX_LIST_MONO  pfree)
{
    pfree->MONO_plistNext = *phead;
    *phead = pfree;                                                     /*  将节点放入链表头            */
}
/*********************************************************************************************************
  双向 RING 链表操作函数声明
*********************************************************************************************************/
VOID  _List_Ring_Add_Ahead(PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader);
VOID  _List_Ring_Add_Front(PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader);
VOID  _List_Ring_Add_Last(PMX_LIST_RING   pringNew, MX_LIST_RING_HEADER  *ppringHeader);
VOID  _List_Ring_Del(PMX_LIST_RING  pringDel, MX_LIST_RING_HEADER  *ppringHeader);
/*********************************************************************************************************
  双向 LINE 链表操作函数声明
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

