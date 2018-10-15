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
** 文   件   名: list.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 09 日
**
** 描        述: 这是系统链表操作定义.
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** 函数名称: _List_Ring_Add_Ahead
** 功能描述: 向 RING 链表头指向的那个节点前插入一个节点 (Header 将指向这个节点. 唤醒表的操作)
** 输　入  : pringNew      新的节点
**           ppringHeader  链表头
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Ring_Add_Ahead (PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader)
{
    REGISTER MX_LIST_RING_HEADER    pringHeader;
    
    //bspDebugMsg(T_COL_RED"       [%d] pringNew          = %p\033[0m\n", archMpCur(), pringNew);

    pringHeader = *ppringHeader;
    
    if (pringHeader) {                                                  /*  链表内还有节点              */
        pringNew->RING_plistNext = pringHeader;
        pringNew->RING_plistPrev = pringHeader->RING_plistPrev;
        pringHeader->RING_plistPrev->RING_plistNext = pringNew;
        pringHeader->RING_plistPrev = pringNew;
        
    } else {                                                            /*  链表内没有节点              */
        pringNew->RING_plistPrev = pringNew;                            /*  只有新节点                  */
        pringNew->RING_plistNext = pringNew;                            /*  自我抱环                    */
    }
    
    *ppringHeader = pringNew;                                           /*  将表头指向新节点            */
}
/*********************************************************************************************************
** 函数名称: _List_Ring_Add_Front
** 功能描述: 向 RING 链表头指向的那个节点后插入一个节点 (Header 有节点时，不变化. 就绪表的操作)
** 输　入  : pringNew      新的节点
**           ppringHeader  链表头
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Ring_Add_Front (PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader)
{
    REGISTER MX_LIST_RING_HEADER    pringHeader;
    
    pringHeader = *ppringHeader;
    
    if (pringHeader) {                                                  /*  链表内还有节点              */
        pringNew->RING_plistPrev = pringHeader;
        pringNew->RING_plistNext = pringHeader->RING_plistNext;
        pringHeader->RING_plistNext->RING_plistPrev = pringNew;
        pringHeader->RING_plistNext = pringNew;
    
    } else {                                                            /*  链表内没有节点              */
        pringNew->RING_plistPrev = pringNew;                            /*  只有新节点                  */
        pringNew->RING_plistNext = pringNew;                            /*  自我抱环                    */
        *ppringHeader = pringNew;                                       /*  更新链表头                  */
    }
}
/*********************************************************************************************************
** 函数名称: _List_Ring_Add_Last
** 功能描述: 向 RING 链表头指向的那个节点前插入一个节点 (Header 有节点时, 不变化)
** 输　入  : pringNew      新的节点
**           ppringHeader  链表头
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Ring_Add_Last (PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader)
{
    REGISTER MX_LIST_RING_HEADER    pringHeader;
    
    pringHeader = *ppringHeader;
    
    if (pringHeader) {                                                  /*  没有更新链表                */
        pringHeader->RING_plistPrev->RING_plistNext = pringNew;
        pringNew->RING_plistPrev = pringHeader->RING_plistPrev;
        pringNew->RING_plistNext = pringHeader;
        pringHeader->RING_plistPrev = pringNew;
        
    } else {
        pringNew->RING_plistPrev = pringNew;
        pringNew->RING_plistNext = pringNew;
        *ppringHeader = pringNew;                                       /*  更新链表头                  */
    }
}
/*********************************************************************************************************
** 函数名称: _List_Ring_Del
** 功能描述: 删除一个节点
** 输　入  : pringDel      需要删除的节点
**           ppringHeader  链表头
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Ring_Del (PMX_LIST_RING  pringDel, MX_LIST_RING_HEADER  *ppringHeader)
{
    REGISTER MX_LIST_RING_HEADER    pringHeader;
    
    pringHeader = *ppringHeader;
    
    //bspDebugMsg(T_COL_BLUE"       [%d] pringDel          = %p\033[0m\n", archMpCur(), pringDel);

    if (pringDel->RING_plistNext == pringDel) {                         /*  链表中只有一个节点          */
        *ppringHeader = MX_NULL;
        _LIST_RING_INIT_NODE_PTR(pringDel);
        return;
    
    } else if (pringDel == pringHeader) {
        _list_ring_next(ppringHeader);
    }
    

    //bspDebugMsg("[%d] pringDel->RING_plistPrev = %p\n", archMpCur(), pringDel->RING_plistPrev);
    //bspDebugMsg("[%d] pringDel->RING_plistNext = %p\n", archMpCur(), pringDel->RING_plistNext);

    pringHeader = pringDel->RING_plistPrev;                             /*  pringHeader 属于临时变量    */
    pringHeader->RING_plistNext = pringDel->RING_plistNext;
    pringDel->RING_plistNext->RING_plistPrev = pringHeader;
    
    _LIST_RING_INIT_NODE_PTR(pringDel);                                 /*  prev = next = NULL          */
}
/*********************************************************************************************************
** 函数名称: _List_Line_Add_Ahead
** 功能描述: 从前面向 Line 头中插入一个节点 (Header 将指向这个节点)
** 输　入  : plingNew      新的节点
**           pplingHeader  链表头
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Line_Add_Ahead (PMX_LIST_LINE  plineNew, MX_LIST_LINE_HEADER  *pplineHeader)
{
    REGISTER MX_LIST_LINE_HEADER    plineHeader;
    
    plineHeader = *pplineHeader;
    
    plineNew->LINE_plistNext = plineHeader;
    plineNew->LINE_plistPrev = MX_NULL;
    
    if (plineHeader) {    
        plineHeader->LINE_plistPrev = plineNew;
    }
    
    *pplineHeader = plineNew;                                           /*  指向最新变量                */
}
/*********************************************************************************************************
** 函数名称: _List_Line_Add_Tail
** 功能描述: 从前面向 Line 头中插入一个节点 (Header 不变)
** 输　入  : plingNew      新的节点
**           pplingHeader  链表头
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Line_Add_Tail (PMX_LIST_LINE  plineNew, MX_LIST_LINE_HEADER  *pplineHeader)
{
    REGISTER MX_LIST_LINE_HEADER    plineHeader;
    
    plineHeader = *pplineHeader;
    
    if (plineHeader) {
        if (plineHeader->LINE_plistNext) {
            plineNew->LINE_plistNext = plineHeader->LINE_plistNext;
            plineNew->LINE_plistPrev = plineHeader;
            plineHeader->LINE_plistNext->LINE_plistPrev = plineNew;
            plineHeader->LINE_plistNext = plineNew;
        
        } else {
            plineHeader->LINE_plistNext = plineNew;
            plineNew->LINE_plistPrev = plineHeader;
            plineNew->LINE_plistNext = MX_NULL;
        }
    
    } else {
        plineNew->LINE_plistPrev = MX_NULL;
        plineNew->LINE_plistNext = MX_NULL;
        
        *pplineHeader = plineNew;
    }
}
/*********************************************************************************************************
** 函数名称: _List_Line_Add_Left
** 功能描述: 将新的节点插入指定节点的左侧
** 输　入  : plineNew      新的节点
**           plineRight    右侧节点
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Line_Add_Left (PMX_LIST_LINE  plineNew, PMX_LIST_LINE  plineRight)
{
    REGISTER PMX_LIST_LINE      plineLeft = plineRight->LINE_plistPrev;
    
    plineNew->LINE_plistNext = plineRight;
    plineNew->LINE_plistPrev = plineLeft;
    
    if (plineLeft) {
        plineLeft->LINE_plistNext = plineNew;
    }
    
    plineRight->LINE_plistPrev = plineNew;
}
/*********************************************************************************************************
** 函数名称: _List_Line_Add_Right
** 功能描述: 将新的节点插入指定节点的右侧
** 输　入  : plineNew      新的节点
**           plineLeft     左侧节点
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Line_Add_Right (PMX_LIST_LINE  plineNew, PMX_LIST_LINE  plineLeft)
{
    REGISTER PMX_LIST_LINE      plineRight = plineLeft->LINE_plistNext;
    
    plineNew->LINE_plistNext = plineRight;
    plineNew->LINE_plistPrev = plineLeft;
    
    if (plineRight) {
        plineRight->LINE_plistPrev = plineNew;
    }
    
    plineLeft->LINE_plistNext = plineNew;
}
/*********************************************************************************************************
** 函数名称: _List_Line_Del
** 功能描述: 删除一个节点
** 输　入  : plingDel      需要删除的节点
**           pplingHeader  链表头
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _List_Line_Del (PMX_LIST_LINE  plineDel, MX_LIST_LINE_HEADER  *pplineHeader)
{
    if (plineDel->LINE_plistPrev == MX_NULL) {                          /*  要删除的节点是表头元素      */
        *pplineHeader = plineDel->LINE_plistNext;
    } else {
        plineDel->LINE_plistPrev->LINE_plistNext = plineDel->LINE_plistNext;
    }
    
    if (plineDel->LINE_plistNext) {
        plineDel->LINE_plistNext->LINE_plistPrev = plineDel->LINE_plistPrev;
    }

    _LIST_LINE_INIT_NODE_PTR(plineDel);                                 /*  prev = next = NULL          */
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

