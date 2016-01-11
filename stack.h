/***************************************************************************************
*	File Name					:	stack.h
*	CopyRight					:	2015/03/09
*	ModuleName					:	ADT/stack
*
*	Create Data					:	2015/03/09
*	Author/Corportation			:	ZhuoJianHuan
*
*	Abstract Description		:	抽象数据结构·栈操作的头文件
*
*--------------------------------Revision History--------------------------------------
*	No	version		Data		Revised By			Item			Description
*	1	V1.00		2015/03/09	ZhuoJianHuan		All				建立文件
*
***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __FN_STACK_H
#define __FN_STACK_H
/**************************************************************
*	Debug switch Section
**************************************************************/
/**************************************************************
*	Include File Section
**************************************************************/
#include "ADT.h"
/**************************************************************
*	Macro Define Section
**************************************************************/
//const unsigned int	STACK_INIT_SIZE = 100;
//const unsigned int  STACK_INCR_SIZE = 10;
#define STACK_INCR_SIZE		10
#define STACK_INIT_SIZE		100
/**************************************************************
*	Struct Define Section
**************************************************************/
typedef int	SElemType;			//此处为栈的类型，应修改为所需的类型

typedef struct _stack_t{
	SElemType		*base;				//基地址
	SElemType		*top;				//栈顶地址
	unsigned int	stackSize;			//栈规模
}stack_t;
/**************************************************************
*	Prototype Declare Section
**************************************************************/
typedef struct _stackOper_t{
	STATUS(*init)(stack_t *s);										//栈初始化
	STATUS(*destory)(stack_t *s);									//栈销毁操作
	STATUS(*clear)(stack_t *s);										//栈清空操作
	STATUS(*empty)(stack_t s, int *e);								//栈空判定
	STATUS(*length)(stack_t s, unsigned int *l);					//求栈长度
	STATUS(*getTop)(stack_t s, SElemType *e);						//获得栈顶元素，不删除
	STATUS(*push)(stack_t *s, SElemType e);							//压栈操作
	STATUS(*pop)(stack_t *s, SElemType *e);							//弹栈操作
	STATUS(*traverse)(stack_t s, STATUS(*visit)(SElemType e));		//对栈s每个元素做visit遍历
}const stackOper_t;

/**************************************************************
*	Global Variable Declare Section
**************************************************************/
//栈操作的唯一结构体实例
extern stackOper_t stackOper;
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif
