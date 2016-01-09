/***************************************************************************************
 *	FileName					:	fifo.c
 *	CopyRight					:	1.0
 *	ModuleName					:	Collection management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	队列缓冲逻辑的实现文件
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
 *	@description	初始化一个32位为缓冲单位的fifo缓冲区
 *	@param			fifo：欲初始化的缓冲区
 *					size：缓冲区大小
 *					buf：缓冲区的地址
 *					task：关联任务
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
 *	@description	将指定数据放入缓冲
 *	@param			fifo：目标缓冲
 *					data：欲放入的数据
 *	@return			放入成功否则返回0，否则返回-1，可能是缓冲已满
 */
int fifo32_put(struct FIFO32 *fifo, int data){
	//TODO 缓冲有效区间检查
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
		//TODO 任务唤醒
		if (fifo->task->flags != 2) {
			task_run(fifo->task, -1, 0);
		}
	}
	return 0;
}

/**
 *	@description	从指定32位缓冲区中获取一个数据
 *	@param			fifo：指定的缓冲区
 *	@return			若缓冲区无数据返回-1，否则返回其中的一个数据
 */
int fifo32_get(struct FIFO32 *fifo){
	int data;

	//TODO 检查缓冲区数据有效性
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
 *	@description	检查指定缓冲区的缓冲数据量
 *	@return			缓冲数据数量
 */
int fifo32_status(struct FIFO32 *fifo){
	return fifo->size - fifo->free;
}
