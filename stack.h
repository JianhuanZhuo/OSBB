/***************************************************************************************
*	File Name					:	stack.h
*	CopyRight					:	2015/03/09
*	ModuleName					:	ADT/stack
*
*	Create Data					:	2015/03/09
*	Author/Corportation			:	ZhuoJianHuan
*
*	Abstract Description		:	�������ݽṹ��ջ������ͷ�ļ�
*
*--------------------------------Revision History--------------------------------------
*	No	version		Data		Revised By			Item			Description
*	1	V1.00		2015/03/09	ZhuoJianHuan		All				�����ļ�
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
typedef int	SElemType;			//�˴�Ϊջ�����ͣ�Ӧ�޸�Ϊ���������

typedef struct _stack_t{
	SElemType		*base;				//����ַ
	SElemType		*top;				//ջ����ַ
	unsigned int	stackSize;			//ջ��ģ
}stack_t;
/**************************************************************
*	Prototype Declare Section
**************************************************************/
typedef struct _stackOper_t{
	STATUS(*init)(stack_t *s);										//ջ��ʼ��
	STATUS(*destory)(stack_t *s);									//ջ���ٲ���
	STATUS(*clear)(stack_t *s);										//ջ��ղ���
	STATUS(*empty)(stack_t s, int *e);								//ջ���ж�
	STATUS(*length)(stack_t s, unsigned int *l);					//��ջ����
	STATUS(*getTop)(stack_t s, SElemType *e);						//���ջ��Ԫ�أ���ɾ��
	STATUS(*push)(stack_t *s, SElemType e);							//ѹջ����
	STATUS(*pop)(stack_t *s, SElemType *e);							//��ջ����
	STATUS(*traverse)(stack_t s, STATUS(*visit)(SElemType e));		//��ջsÿ��Ԫ����visit����
}const stackOper_t;

/**************************************************************
*	Global Variable Declare Section
**************************************************************/
//ջ������Ψһ�ṹ��ʵ��
extern stackOper_t stackOper;
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif
