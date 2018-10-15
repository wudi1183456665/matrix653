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
** 文   件   名: k_error.h
**
** 创   建   人: Wang.Dongfang (王东方)
**
** 文件创建日期: 2018 年 01 月 09 日
**
** 描        述: 这是系统错误码宏.
*********************************************************************************************************/

#ifndef  __K_ERROR_H
#define  __K_ERROR_H

/*********************************************************************************************************
  完全正确
*********************************************************************************************************/
#define  ERROR_NONE                        0                            /*  系统没有任何错误            */
/*********************************************************************************************************
  系统错误   301 - 500
*********************************************************************************************************/
#define  ERROR_KERNEL_PNAME_NULL         301                            /*  名字指针为 NULL             */
#define  ERROR_KERNEL_PNAME_TOO_LONG     302                            /*  名字太长                    */
#define  ERROR_KERNEL_HANDLE_NULL        303                            /*  句柄出错                    */
#define  ERROR_KERNEL_IN_ISR             304                            /*  系统处于中断中              */
#define  ERROR_KERNEL_RUNNING            305                            /*  系统正在运行                */
#define  ERROR_KERNEL_NOT_RUNNING        306                            /*  系统没有运行                */
#define  ERROR_KERNEL_OBJECT_NULL        307                            /*  OBJECT 为空                 */
#define  ERROR_KERNEL_LOW_MEMORY         308                            /*  缺少内存                    */
#define  ERROR_KERNEL_BUFFER_NULL        309                            /*  缺少缓冲                    */
#define  ERROR_KERNEL_OPTION             310                            /*  选项出错                    */
#define  ERROR_KERNEL_VECTOR_NULL        311                            /*  中断向量出错                */
#define  ERROR_KERNEL_HOOK_NULL          312                            /*  内核钩子出错                */
#define  ERROR_KERNEL_OPT_NULL           313                            /*  内核钩子选项出错            */
#define  ERROR_KERNEL_MEMORY             314                            /*  内存地址出现错误            */
#define  ERROR_KERNEL_LOCK               315                            /*  内核被锁定了                */
#define  ERROR_KERNEL_CPU_NULL           316                            /*  指定 CPU 错误               */
#define  ERROR_KERNEL_HOOK_FULL          317                            /*  hook 表已满                 */
#define  ERROR_KERNEL_KEY_CONFLICT       318                            /*  key 冲突                    */
/*********************************************************************************************************
  线程错误 501 - 1000
*********************************************************************************************************/
#define  ERROR_VCPU_STACKSIZE_LACK       501                            /*  堆栈太小                    */
#define  ERROR_VCPU_STACK_NULL           502                            /*  缺少堆栈                    */
#define  ERROR_VCPU_FP_STACK_NULL        503                            /*  浮点堆栈                    */
#define  ERROR_VCPU_ATTR_NULL            504                            /*  缺少属性块                  */
#define  ERROR_VCPU_PRIORITY_WRONG       505                            /*  优先级错误                  */
#define  ERROR_VCPU_WAIT_TIMEOUT         506                            /*  等待超时                    */
#define  ERROR_VCPU_NULL                 507                            /*  线程句柄无效                */
#define  ERROR_VCPU_FULL                 508                            /*  系统线程已满                */
#define  ERROR_VCPU_NOT_INIT             509                            /*  线程没有初始化              */
#define  ERROR_VCPU_NOT_SUSPEND          510                            /*  线程没有被挂起              */
#define  ERROR_VCPU_VAR_FULL             511                            /*  没有变量控制块可用          */
#define  ERROR_VCPU_VAR_NULL             512                            /*  控制块无效                  */
#define  ERROR_VCPU_VAR_NOT_EXIST        513                            /*  没有找到合适的控制块        */
#define  ERROR_VCPU_NOT_READY            514                            /*  线程没有就绪                */
#define  ERROR_VCPU_IN_SAFE              515                            /*  线程处于安全模式            */
#define  ERROR_VCPU_OTHER_DELETE         516                            /*  已经有其他线程在等待删除    */
#define  ERROR_VCPU_JOIN_SELF            517                            /*  线程合并自己                */
#define  ERROR_VCPU_DETACHED             518                            /*  线程已经设定为不可合并      */
#define  ERROR_VCPU_JOIN                 519                            /*  线程已经和其他线程合并      */
#define  ERROR_VCPU_NOT_SLEEP            520                            /*  线程并没有睡眠              */
#define  ERROR_VCPU_NOTEPAD_INDEX        521                            /*  记事本索引出错              */
#define  ERROR_VCPU_OPTION               522                            /*  线程选项与执行操作不符      */
#define  ERROR_VCPU_RESTART_SELF         523                            /*  没有启动信号系统, 不能      */
/*********************************************************************************************************
  进程错误 1001 - 1500
*********************************************************************************************************/
#define  ERROR_PARTITION_FULL            1001                           /*  缺乏 PARTITION 控制块       */
#define  ERROR_PARTITION_NULL            1002                           /*  PARTITION 相关参数为空      */
/*********************************************************************************************************
  进程间通信错误 1501 - 2000
*********************************************************************************************************/
#define  ERROR_IPC_FULL                  1501                           /*  缺乏 IPC 控制块             */
#define  ERROR_IPC_VAR_NULL              1502                           /*  IPC 相关参数为空            */
/*********************************************************************************************************
  内核对象错误 2001 - 2500
*********************************************************************************************************/
#define  ERROR_KOBJ_NULL                 2001                           /*  没有对应的内核对象          */

#define	 PX_ERROR						 (-1)


#endif                                                                  /*  __K_ERROR_H                 */
/*********************************************************************************************************
  END
*********************************************************************************************************/

