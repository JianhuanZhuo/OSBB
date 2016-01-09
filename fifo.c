/***************************************************************************************
 *	FileName					:	fifo.c
 *	CopyRight					:	1.0
 *	ModuleName					:	Collection management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	���л����߼���ʵ���ļ�
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Date			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Debug switch Section
**************************************************************/

/**************************************************************
*	Include File Section
**************************************************************/
#include "bootpack.h"
/**************************************************************
*	Macro Define Section
**************************************************************/
#define FLAGS_OVERRUN		0x0001
/**************************************************************
*	Struct Define Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	��ʼ��һ��32λΪ���嵥λ��fifo������
 *	@param			fifo������ʼ���Ļ�����
 *					size����������С
 *					buf���������ĵ�ַ
 *					task����������
 */
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task){
	fifo->size = size;
	fifo->buf = buf;
	fifo->free = size;
	fifo->flags = 0;
	fifo->p = 0;
	fifo->q = 0;
	fifo->task = task;
	return;
}


/**
 *	@description	��ָ�����ݷ��뻺��
 *	@param			fifo��Ŀ�껺��
 *					data�������������
 *	@return			����ɹ����򷵻�0�����򷵻�-1�������ǻ�������
 */
int fifo32_put(struct FIFO32 *fifo, int data){
	//TODO ������Ч������
	if (fifo->free == 0) {
		fifo->flags |= FLAGS_OVERRUN;
		return -1;
	}
	fifo->buf[fifo->p] = data;
	fifo->p++;
	if (fifo->p == fifo->size) {
		fifo->p = 0;
	}
	fifo->free--;
	if (fifo->task != 0) {
		//TODO ������
		if (fifo->task->flags != 2) {
			task_run(fifo->task, -1, 0);
		}
	}
	return 0;
}

/**
 *	@description	��ָ��32λ�������л�ȡһ������
 *	@param			fifo��ָ���Ļ�����
 *	@return			�������������ݷ���-1�����򷵻����е�һ������
 */
int fifo32_get(struct FIFO32 *fifo){
	int data;

	//TODO ��黺����������Ч��
	if (fifo->free == fifo->size) {
		return -1;
	}
	data = fifo->buf[fifo->q];
	fifo->q++;
	if (fifo->q == fifo->size) {
		fifo->q = 0;
	}
	fifo->free++;
	return data;
}


/**
 *	@description	���ָ���������Ļ���������
 *	@return			������������
 */
int fifo32_status(struct FIFO32 *fifo){
	return fifo->size - fifo->free;
}
