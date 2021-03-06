/***************************************************************************************
 *	File Name				:		fifo.h
 *	CopyRight				:		1.0
 *	ModuleName				:		Collection management module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		队列缓冲逻辑的实现文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __FIFO_H
#define __FIFO_H
/**************************************************************
*	Debug switch Section
**************************************************************/

/**************************************************************
*	Include File Section
**************************************************************/
/**************************************************************
*	Macro Define Section
**************************************************************/
/**************************************************************
*	Struct Define Section
**************************************************************/
//32位为单位的缓冲区结构体
struct FIFO32 {
	int *buf;			//缓冲区首地址
	int p;				//写入位置
	int q;				//读取位置
	int size;			//缓冲区大小
	int free;			//缓冲区当前可写入数量
	int flags;			//缓冲区标志位，缓冲满时为1
	struct TASK *task;	//关联任务
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@description	初始化一个32位为缓冲单位的fifo缓冲区
 *	@param			fifo：欲初始化的缓冲区
 *					size：缓冲区大小
 *					buf：缓冲区的地址
 *					task：关联任务
 */
void fifo32_init(struct FIFO32 *fifo, int size, int *buf, struct TASK *task);

/**
 *	@description	将指定数据放入缓冲
 *	@param			fifo：目标缓冲
 *					data：欲放入的数据
 *	@return			放入成功否则返回0，否则返回-1，可能是缓冲已满
 */
int fifo32_put(struct FIFO32 *fifo, int data);

/**
 *	@description	从指定32位缓冲区中获取一个数据
 *	@param			fifo：指定的缓冲区
 *	@return			若缓冲区无数据返回-1，否则返回其中的一个数据
 */
int fifo32_get(struct FIFO32 *fifo);

/**
 *	@description	检查指定缓冲区的缓冲数据量
 *	@return			缓冲数据数量
 */
int fifo32_status(struct FIFO32 *fifo);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


