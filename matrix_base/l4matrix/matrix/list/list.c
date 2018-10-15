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
** ��   ��   ��: list.c
**
** ��   ��   ��: Wang.Dongfang (������)
**
** �ļ���������: 2018 �� 01 �� 09 ��
**
** ��        ��: ����ϵͳ�����������.
*********************************************************************************************************/
#include <Matrix.h>
/*********************************************************************************************************
** ��������: _List_Ring_Add_Ahead
** ��������: �� RING ����ͷָ����Ǹ��ڵ�ǰ����һ���ڵ� (Header ��ָ������ڵ�. ���ѱ�Ĳ���)
** �䡡��  : pringNew      �µĽڵ�
**           ppringHeader  ����ͷ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _List_Ring_Add_Ahead (PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader)
{
    REGISTER MX_LIST_RING_HEADER    pringHeader;
    
    //bspDebugMsg(T_COL_RED"       [%d] pringNew          = %p\033[0m\n", archMpCur(), pringNew);

    pringHeader = *ppringHeader;
    
    if (pringHeader) {                                                  /*  �����ڻ��нڵ�              */
        pringNew->RING_plistNext = pringHeader;
        pringNew->RING_plistPrev = pringHeader->RING_plistPrev;
        pringHeader->RING_plistPrev->RING_plistNext = pringNew;
        pringHeader->RING_plistPrev = pringNew;
        
    } else {                                                            /*  ������û�нڵ�              */
        pringNew->RING_plistPrev = pringNew;                            /*  ֻ���½ڵ�                  */
        pringNew->RING_plistNext = pringNew;                            /*  ���ұ���                    */
    }
    
    *ppringHeader = pringNew;                                           /*  ����ͷָ���½ڵ�            */
}
/*********************************************************************************************************
** ��������: _List_Ring_Add_Front
** ��������: �� RING ����ͷָ����Ǹ��ڵ�����һ���ڵ� (Header �нڵ�ʱ�����仯. ������Ĳ���)
** �䡡��  : pringNew      �µĽڵ�
**           ppringHeader  ����ͷ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _List_Ring_Add_Front (PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader)
{
    REGISTER MX_LIST_RING_HEADER    pringHeader;
    
    pringHeader = *ppringHeader;
    
    if (pringHeader) {                                                  /*  �����ڻ��нڵ�              */
        pringNew->RING_plistPrev = pringHeader;
        pringNew->RING_plistNext = pringHeader->RING_plistNext;
        pringHeader->RING_plistNext->RING_plistPrev = pringNew;
        pringHeader->RING_plistNext = pringNew;
    
    } else {                                                            /*  ������û�нڵ�              */
        pringNew->RING_plistPrev = pringNew;                            /*  ֻ���½ڵ�                  */
        pringNew->RING_plistNext = pringNew;                            /*  ���ұ���                    */
        *ppringHeader = pringNew;                                       /*  ��������ͷ                  */
    }
}
/*********************************************************************************************************
** ��������: _List_Ring_Add_Last
** ��������: �� RING ����ͷָ����Ǹ��ڵ�ǰ����һ���ڵ� (Header �нڵ�ʱ, ���仯)
** �䡡��  : pringNew      �µĽڵ�
**           ppringHeader  ����ͷ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _List_Ring_Add_Last (PMX_LIST_RING  pringNew, MX_LIST_RING_HEADER  *ppringHeader)
{
    REGISTER MX_LIST_RING_HEADER    pringHeader;
    
    pringHeader = *ppringHeader;
    
    if (pringHeader) {                                                  /*  û�и�������                */
        pringHeader->RING_plistPrev->RING_plistNext = pringNew;
        pringNew->RING_plistPrev = pringHeader->RING_plistPrev;
        pringNew->RING_plistNext = pringHeader;
        pringHeader->RING_plistPrev = pringNew;
        
    } else {
        pringNew->RING_plistPrev = pringNew;
        pringNew->RING_plistNext = pringNew;
        *ppringHeader = pringNew;                                       /*  ��������ͷ                  */
    }
}
/*********************************************************************************************************
** ��������: _List_Ring_Del
** ��������: ɾ��һ���ڵ�
** �䡡��  : pringDel      ��Ҫɾ���Ľڵ�
**           ppringHeader  ����ͷ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _List_Ring_Del (PMX_LIST_RING  pringDel, MX_LIST_RING_HEADER  *ppringHeader)
{
    REGISTER MX_LIST_RING_HEADER    pringHeader;
    
    pringHeader = *ppringHeader;
    
    //bspDebugMsg(T_COL_BLUE"       [%d] pringDel          = %p\033[0m\n", archMpCur(), pringDel);

    if (pringDel->RING_plistNext == pringDel) {                         /*  ������ֻ��һ���ڵ�          */
        *ppringHeader = MX_NULL;
        _LIST_RING_INIT_NODE_PTR(pringDel);
        return;
    
    } else if (pringDel == pringHeader) {
        _list_ring_next(ppringHeader);
    }
    

    //bspDebugMsg("[%d] pringDel->RING_plistPrev = %p\n", archMpCur(), pringDel->RING_plistPrev);
    //bspDebugMsg("[%d] pringDel->RING_plistNext = %p\n", archMpCur(), pringDel->RING_plistNext);

    pringHeader = pringDel->RING_plistPrev;                             /*  pringHeader ������ʱ����    */
    pringHeader->RING_plistNext = pringDel->RING_plistNext;
    pringDel->RING_plistNext->RING_plistPrev = pringHeader;
    
    _LIST_RING_INIT_NODE_PTR(pringDel);                                 /*  prev = next = NULL          */
}
/*********************************************************************************************************
** ��������: _List_Line_Add_Ahead
** ��������: ��ǰ���� Line ͷ�в���һ���ڵ� (Header ��ָ������ڵ�)
** �䡡��  : plingNew      �µĽڵ�
**           pplingHeader  ����ͷ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
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
    
    *pplineHeader = plineNew;                                           /*  ָ�����±���                */
}
/*********************************************************************************************************
** ��������: _List_Line_Add_Tail
** ��������: ��ǰ���� Line ͷ�в���һ���ڵ� (Header ����)
** �䡡��  : plingNew      �µĽڵ�
**           pplingHeader  ����ͷ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
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
** ��������: _List_Line_Add_Left
** ��������: ���µĽڵ����ָ���ڵ�����
** �䡡��  : plineNew      �µĽڵ�
**           plineRight    �Ҳ�ڵ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
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
** ��������: _List_Line_Add_Right
** ��������: ���µĽڵ����ָ���ڵ���Ҳ�
** �䡡��  : plineNew      �µĽڵ�
**           plineLeft     ���ڵ�
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
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
** ��������: _List_Line_Del
** ��������: ɾ��һ���ڵ�
** �䡡��  : plingDel      ��Ҫɾ���Ľڵ�
**           pplingHeader  ����ͷ
** �䡡��  : NONE
** ȫ�ֱ���: 
** ����ģ��: 
*********************************************************************************************************/
VOID  _List_Line_Del (PMX_LIST_LINE  plineDel, MX_LIST_LINE_HEADER  *pplineHeader)
{
    if (plineDel->LINE_plistPrev == MX_NULL) {                          /*  Ҫɾ���Ľڵ��Ǳ�ͷԪ��      */
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

