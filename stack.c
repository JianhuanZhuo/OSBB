/***************************************************************************************
*	FileName					:	stack.C
*	CopyRight					:	2015/03/09
*	ModuleName					:	ADT/stack
*
*	Create Data					:	2015/03/09
*	Author/Corportation			:	ZhuoJianHuan
*
*	Abstract Description		:	抽象数据结构·栈操作实现源文件
*
*--------------------------------Revision History--------------------------------------
*	No	version		Data		Revised By			Item			Description
*	1	V1.00		2015/03/09	ZhuoJianHuan		All				文件建立
*
***************************************************************************************/
/**************************************************************
*	Debug switch Section
**************************************************************/
#define D_DISP_BASE 
/**************************************************************
*	Include File Section
**************************************************************/
#include "stack.h"
#include <stdio.h>
#include "memory.h"
/**************************************************************
*	Macro Define Section
**************************************************************/
/**************************************************************
*	Struct Define Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
STATUS stack_init(stack_t *s);										//栈初始化
STATUS stack_destory(stack_t *s);									//栈销毁操作
STATUS stack_clear(stack_t *s);										//栈清空操作
STATUS stack_empty(stack_t s, int *r);								//栈空判定
STATUS stack_length(stack_t s, unsigned int *l);					//求栈长度
STATUS stack_getTop(stack_t s, SElemType *e);						//获得栈顶元素，不删除
STATUS stack_push(stack_t *s, SElemType e);							//压栈操作
STATUS stack_pop(stack_t *s, SElemType *e);							//弹栈操作
STATUS stack_traverse(stack_t s, STATUS(*visit)(SElemType e));		//对栈s每个元素做visit遍历
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
//栈操作的唯一实例
stackOper_t stackOper = {
	stack_init,
	stack_destory,
	stack_clear,
	stack_empty,
	stack_length,
	stack_getTop,
	stack_push,
	stack_pop,
	stack_traverse
};
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/


/**
 *	@prototype		STATUS stack_Init(stack_t *s)
 *	@description	初始化顺序栈
 *	@param			s【stack】传入参数，欲初始化的栈
 */
STATUS stack_init(stack_t *s){

	//1. 申请空间
	s->base = (SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));
	if (!s->base){
		return OVERFLOW;
	}

	//2. 初始化数据
	s->stackSize = STACK_INIT_SIZE;
	s->top		= s->base;
	return OK;
}
/**
 *	@prototype		STATUS stack_destory(stack_t *s)
 *	@description	栈销毁操作
 *	@param			s【stack】欲操作的栈
 */
STATUS stack_destory(stack_t *s){
	
	//1. 释放内存
	if (!s->base){
		return OBJINVALID;
	}
	mfree((unsigned int)s->base, s->stackSize * sizeof(SElemType));

	//2. 数据
	s->base			= 0;
	s->stackSize	= 0;
	return OK;
}

/**
 *	@prototype		STATUS stack_clear(stack_t *s)
 *	@description	栈清空操作
 *	@param			s【stack】欲操作的栈
 */
STATUS stack_clear(stack_t *s){
	
	if (!s->base){
		return OBJINVALID;
	}

	s->top		= s->base;
	s->stackSize	= STACK_INIT_SIZE;
	return OK;
}

/**
 *	@prototype		STATUS stack_empty(stack_t s, bool *r)
 *	@description	栈空判定
 *	@param			s【stack】欲操作的栈
 *					r【result】传出参数，判定的结果
 */
STATUS stack_empty(stack_t s, int *r){

	if (!s.base){
		return OBJINVALID;
	}

	*r	= (s.top==s.base);
	return OK;
}

/**
 *	@prototype		STATUS stack_length(stack_t s, unsigned int *l)
 *	@description	求栈长度
 * 	@param			s【stack】欲操作的栈
 *					l【Length】栈的长度
 */
STATUS stack_length(stack_t s, unsigned int *l){

	if (!s.base){
		return OBJINVALID;
	}

	*l	= (unsigned int)(s.top-s.base);
	return OK;
}

/**
 *	@prototype		STATUS stack_getTop(stack_t s, SElemType *e)
 *	@description	获得栈顶元素，不删除
 * 	@param			s【stack】欲操作的栈
 *					e【element】栈顶元素
 */
STATUS stack_getTop(stack_t s, SElemType *e){

	if ((!s.base) | (s.top == s.base)){
		return OBJINVALID;
	}

	*e	= *(s.top-1);

	return OK;
}

/**
 *	@prototype		STATUS stack_push(stack_t *s, SElemType e)
 *	@description	压栈操作，若栈满将试图拓展栈，拓展失败返回OVERFLOW
 * 	@param			s【stack】欲操作的栈
 *					e【element】栈顶元素
 */
STATUS stack_push(stack_t *s, SElemType e){

	SElemType *p;

	//1. 对象有效判定
	if (!s->base){
		return OBJINVALID;
	}

	//2. 栈满判定
	if (s->base+s->stackSize == s->top){
		//2.1. 栈拓展
		//s->base = (SElemType*)realloc(s->base, s->stackSize+STACK_INCR_SIZE);
		//if (!s->base){//拓展失败原数据会丢失??????
		//	return OVERFLOW;
		//}
		p = (SElemType*)malloc((s->stackSize+STACK_INCR_SIZE)*sizeof(SElemType));
		memcpy(p, s->base, s->stackSize);
		mfree((unsigned int)s->base, s->stackSize * sizeof(SElemType));
		s->base = p;
		//
		//////////
		s->top		= s->base + s->stackSize;
		s->stackSize	= s->stackSize+STACK_INCR_SIZE;
	}
	//3. 压栈操作
	*(s->top) = e;
	s->top++;

	return OK;

}

/**
 *	@prototype		STATUS stack_pop(stack_t *s, SElemType *e)
 *	@description	弹栈操作
 * 	@param			s【stack】欲操作的栈
 *					e【element】保存弹出的元素
 */
STATUS stack_pop(stack_t *s, SElemType *e){

	//1. 有效性判定
	if ((!s->base) | (s->base == s->top)){
		return OBJINVALID;
	}
	//2. 弹栈操作
	s->top--;
	*e	= *(s->top);
	 
	return OK;
}

/**
 *	@prototype		STATUS stack_traverse(stack_t s, STATUS(*visit)(SElemType e))
 *	@description	对栈s每个元素做visit遍历
 * 	@param			s【stack】欲操作的栈
 *					visit，指向遍历函数的函数指针
 */
STATUS stack_traverse(stack_t s, STATUS(*visit)(SElemType e)){
	
	SElemType	*p;					//用于辅助遍历的指针
	STATUS		res	= OK;			//用于保存遍历返回的状态

	//1. 有效性判定
	if ((!s.base) | (s.base == s.top)){
		return OBJINVALID;
	}
	if (!visit){
		return ERROR;
	}

	//2. 遍历操作
	for (p = s.base; p != s.top; p++){
		res	=(STATUS)((unsigned int)res | (unsigned int)visit(*p));
	}
	
	return res;
}


















