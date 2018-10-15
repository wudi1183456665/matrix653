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
** 文   件   名: _ReadyTable.c
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2017 年 12 月 19 日
**
** 描        述: 就绪表操作.
*********************************************************************************************************/
#include <Matrix.h>
#include "k_internal.h"
/*********************************************************************************************************
** 函数名称: _ReadyTableGet
** 功能描述: 获取含有最高优先级线程的就绪表和最高优先级值
** 输　入  : pcpu         包含本地就绪表
**           puiPriority  返回最高优先级
** 输　出  : 包含最高优先级线程的就绪表(要么是本地就绪表, 要么是全局就绪表)
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PMX_READY  _ReadyTableGet (PMX_PHYS_CPU  pcpu,  UINT32 *puiPriority)
{
    REGISTER PMX_READY  preadyReturn;
    REGISTER UINT32     uiLocal, uiGlobal;
 
    if (_BitmapIsEmpty(MX_CPU_RDY_BMAP(pcpu))) {                        /*  CPU 本地就绪表空            */
        if (_BitmapIsEmpty(MX_GLOBAL_RDY_BMAP())) {                     /*  全局就绪表也为空            */
            return  (MX_NULL);                                          /*  就绪表中无任务              */
        }
        *puiPriority = _BitmapHigh(MX_GLOBAL_RDY_BMAP());
        return  (MX_GLOBAL_RDY());                                      /*  返回全局就绪表              */
    
    } else {                                                            /*  CPU 本地就绪表非空          */
        if (_BitmapIsEmpty(MX_GLOBAL_RDY_BMAP())) {                     /*  全局就绪表空                */
            *puiPriority = _BitmapHigh(MX_CPU_RDY_BMAP(pcpu));
            return  (MX_CPU_RDY(pcpu));                                 /*  返回本地就绪表              */
        }

        uiLocal  = _BitmapHigh(MX_CPU_RDY_BMAP(pcpu));
        uiGlobal = _BitmapHigh(MX_GLOBAL_RDY_BMAP());
        
        if (MX_PRIO_IS_HIGH_OR_EQU(uiLocal, uiGlobal)) {                /*  同优先级, 优先执行 local    */
            preadyReturn = MX_CPU_RDY(pcpu);                            /*  返回本地就绪表              */
            *puiPriority = uiLocal;
        
        } else {
            preadyReturn = MX_GLOBAL_RDY();                             /*  返回全局就绪表              */
            *puiPriority = uiGlobal;
        }
        
        return  (preadyReturn);
    }
}
/*********************************************************************************************************
** 函数名称: _ReadyTableAdd
** 功能描述: 将指定线程加入就绪表, 并置 Bitmap
** 输　入  : pvcpu      线程控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _ReadyTableAdd (PMX_VCPU  pvcpu)
{
    PMX_RDYP    prdyp;
    PMX_READY   pready;

    prdyp = _RdyPrioGet(pvcpu);                                         /*  获取线程所属的优先级控制块  */
    _RdyPrioAddVcpu(pvcpu, prdyp, MX_TRUE);                             /*  插入就绪环头, 下次优先调度  */
    
    if (_RdyPrioIsOne(prdyp)) {
        if (pvcpu->VCPU_bCpuAffinity) {
            pready = MX_CPU_RDY(MX_CPU_GET(pvcpu->VCPU_ulCpuIdAffinity));
        } else {
            pready = MX_GLOBAL_RDY();
        }

        _BitmapAdd(&pready->RDY_bmap, pvcpu->VCPU_uiPriority);
    }
}
/*********************************************************************************************************
** 函数名称: _ReadyTableDel
** 功能描述: 当指定线程退出就绪表后, 清 Bitmap
** 输　入  : pvcpu      线程控制块
** 输　出  : NONE
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
VOID  _ReadyTableDel (PMX_VCPU  pvcpu)
{
    PMX_RDYP    prdyp;
    PMX_READY   pready;

    prdyp = _RdyPrioGet(pvcpu);                                         /*  获取线程所属的优先级控制块  */
    _RdyPrioDelVcpu(pvcpu, prdyp);                                      /*  从就绪表中删除              */
    
    if (_RdyPrioIsEmpty(prdyp)) {
        if (pvcpu->VCPU_bCpuAffinity) {
            pready = MX_CPU_RDY(MX_CPU_GET(pvcpu->VCPU_ulCpuIdAffinity));
        } else {
            pready = MX_GLOBAL_RDY();
        }

        _BitmapDel(&pready->RDY_bmap, pvcpu->VCPU_uiPriority);
    }
}
/*********************************************************************************************************
** 函数名称: _ReadyTableSeek
** 功能描述: 获得指定就绪表的指定优先级环表的表头线程
** 输　入  : pready          就绪表
**           uiPriority      优先级
** 输　出  : 就绪表中指定优先级链表头线程
** 全局变量: 
** 调用模块: 
*********************************************************************************************************/
PMX_VCPU  _ReadyTableSeek (PMX_READY  pready, UINT32  uiPriority)
{
    REGISTER PMX_VCPU  pvcpu;
    REGISTER PMX_RDYP  prdyp;
    
    prdyp = &pready->RDY_rdyp[uiPriority];
    pvcpu = _LIST_ENTRY(prdyp->RDYP_pringReadyHeader, 
                        MX_VCPU, 
                        VCPU_ringReady);                                /*  从就绪环中取出一个线程      */

    if (pvcpu->VCPU_usSchedCounter == 0) {                              /*  时间片用完                  */
        pvcpu->VCPU_usSchedCounter = pvcpu->VCPU_usSchedSlice;          /*  补充时间片                  */
        _list_ring_next(&prdyp->RDYP_pringReadyHeader);                 /*  下一个                      */
        pvcpu = _LIST_ENTRY(prdyp->RDYP_pringReadyHeader, 
                           MX_VCPU, 
                           VCPU_ringReady);
    }
    
    return  (pvcpu);
}
/*********************************************************************************************************
  END
*********************************************************************************************************/

