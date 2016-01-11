/***************************************************************************************
*	FileName					:	stack.C
*	CopyRight					:	2015/03/09
*	ModuleName					:	ADT/stack
*
*	Create Data					:	2015/03/09
*	Author/Corportation			:	ZhuoJianHuan
*
*	Abstract Description		:	�������ݽṹ��ջ����ʵ��Դ�ļ�
*
*--------------------------------Revision History--------------------------------------
*	No	version		Data		Revised By			Item			Description
*	1	V1.00		2015/03/09	ZhuoJianHuan		All				�ļ�����
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
STATUS stack_init(stack_t *s);										//ջ��ʼ��
STATUS stack_destory(stack_t *s);									//ջ���ٲ���
STATUS stack_clear(stack_t *s);										//ջ��ղ���
STATUS stack_empty(stack_t s, int *r);								//ջ���ж�
STATUS stack_length(stack_t s, unsigned int *l);					//��ջ����
STATUS stack_getTop(stack_t s, SElemType *e);						//���ջ��Ԫ�أ���ɾ��
STATUS stack_push(stack_t *s, SElemType e);							//ѹջ����
STATUS stack_pop(stack_t *s, SElemType *e);							//��ջ����
STATUS stack_traverse(stack_t s, STATUS(*visit)(SElemType e));		//��ջsÿ��Ԫ����visit����
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
//ջ������Ψһʵ��
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
 *	@description	��ʼ��˳��ջ
 *	@param			s��stack���������������ʼ����ջ
 */
STATUS stack_init(stack_t *s){

	//1. ����ռ�
	s->base = (SElemType *)malloc(STACK_INIT_SIZE * sizeof(SElemType));
	if (!s->base){
		return OVERFLOW;
	}

	//2. ��ʼ������
	s->stackSize = STACK_INIT_SIZE;
	s->top		= s->base;
	return OK;
}
/**
 *	@prototype		STATUS stack_destory(stack_t *s)
 *	@description	ջ���ٲ���
 *	@param			s��stack����������ջ
 */
STATUS stack_destory(stack_t *s){
	
	//1. �ͷ��ڴ�
	if (!s->base){
		return OBJINVALID;
	}
	mfree((unsigned int)s->base, s->stackSize * sizeof(SElemType));

	//2. ����
	s->base			= 0;
	s->stackSize	= 0;
	return OK;
}

/**
 *	@prototype		STATUS stack_clear(stack_t *s)
 *	@description	ջ��ղ���
 *	@param			s��stack����������ջ
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
 *	@description	ջ���ж�
 *	@param			s��stack����������ջ
 *					r��result�������������ж��Ľ��
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
 *	@description	��ջ����
 * 	@param			s��stack����������ջ
 *					l��Length��ջ�ĳ���
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
 *	@description	���ջ��Ԫ�أ���ɾ��
 * 	@param			s��stack����������ջ
 *					e��element��ջ��Ԫ��
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
 *	@description	ѹջ��������ջ������ͼ��չջ����չʧ�ܷ���OVERFLOW
 * 	@param			s��stack����������ջ
 *					e��element��ջ��Ԫ��
 */
STATUS stack_push(stack_t *s, SElemType e){

	SElemType *p;

	//1. ������Ч�ж�
	if (!s->base){
		return OBJINVALID;
	}

	//2. ջ���ж�
	if (s->base+s->stackSize == s->top){
		//2.1. ջ��չ
		//s->base = (SElemType*)realloc(s->base, s->stackSize+STACK_INCR_SIZE);
		//if (!s->base){//��չʧ��ԭ���ݻᶪʧ??????
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
	//3. ѹջ����
	*(s->top) = e;
	s->top++;

	return OK;

}

/**
 *	@prototype		STATUS stack_pop(stack_t *s, SElemType *e)
 *	@description	��ջ����
 * 	@param			s��stack����������ջ
 *					e��element�����浯����Ԫ��
 */
STATUS stack_pop(stack_t *s, SElemType *e){

	//1. ��Ч���ж�
	if ((!s->base) | (s->base == s->top)){
		return OBJINVALID;
	}
	//2. ��ջ����
	s->top--;
	*e	= *(s->top);
	 
	return OK;
}

/**
 *	@prototype		STATUS stack_traverse(stack_t s, STATUS(*visit)(SElemType e))
 *	@description	��ջsÿ��Ԫ����visit����
 * 	@param			s��stack����������ջ
 *					visit��ָ����������ĺ���ָ��
 */
STATUS stack_traverse(stack_t s, STATUS(*visit)(SElemType e)){
	
	SElemType	*p;					//���ڸ���������ָ��
	STATUS		res	= OK;			//���ڱ���������ص�״̬

	//1. ��Ч���ж�
	if ((!s.base) | (s.base == s.top)){
		return OBJINVALID;
	}
	if (!visit){
		return ERROR;
	}

	//2. ��������
	for (p = s.base; p != s.top; p++){
		res	=(STATUS)((unsigned int)res | (unsigned int)visit(*p));
	}
	
	return res;
}


















