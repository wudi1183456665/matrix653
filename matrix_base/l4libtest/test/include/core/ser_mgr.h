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
** 文   件   名: ser_mgr.h
**
** 创   建   人: Cheng.Yongbin (程永斌)
**
** 文件创建日期: 2018 年 2 月 1 日
**
** 描        述: 服务管理
*********************************************************************************************************/

#ifndef __T_SER_MGR_H
#define __T_SER_MGR_H

/*********************************************************************************************************
  service 定义
*********************************************************************************************************/
/*
 * 服务代码
 */
#define TSER_CODE_ROOT      1                                           /* 系统服务                     */
#define TSER_CODE_LOG       2                                           /* 日志服务                     */
/*
 * 服务名称
 */
extern CPCHAR   TG_strSerName[];
/*
 * 服务结构体
 */
typedef struct {
    UINT32              uiSerCode;
    MX_OBJECT_ID        objSer;
    MX_LIST_LINE        lineNode;

} T_SERVICE;
typedef T_SERVICE       *PT_SERVICE;
/*
 * 服务管理器
 */
typedef struct {
    MX_LIST_LINE_HEADER lineService;

}T_SER_MGR;
typedef T_SER_MGR       *PT_SER_MGR;

/*********************************************************************************************************
  ser_mgr 接口
*********************************************************************************************************/
/*
 * 初始化
 */
void T_serMgrInit ();
/*
 * 注册服务
 */
void T_serRegist(UINT32  uiSerCode, MX_OBJECT_ID  objSer);
/*
 * 查找服务
 */
MX_OBJECT_ID T_serFind(UINT32  uiSerCode);

/*********************************************************************************************************
  服务模型
*********************************************************************************************************/


#endif                                                                  /*  __T_SER_MGR_H               */
/*********************************************************************************************************
  END
*********************************************************************************************************/
