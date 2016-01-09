/***************************************************************************************
 *	File Name				:		mouse.h
 *	CopyRight				:		1.0
 *	ModuleName				:		mouse module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		鼠标输入函数的声明文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __MOUSE_H
#define __MOUSE_H
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
//鼠标描述符
struct MOUSE_DEC {
	unsigned char buf[3];	//输入的三个字节
	unsigned char phase;	//当前状态
	int x;					//x轴方向上的偏移量
	int y;					//y轴方向上的偏移量
	int tn;					//按键标志
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@description	使能鼠标模块
 *	@param			fifo：鼠标关联的缓冲区
 *					data0：鼠标关键字
 *					mdec：鼠标描述符
 */
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec);

/**
 *	@description	解码鼠标
 *	@param			mdec：鼠标描述符
 *					dat：当前的鼠标输入
 *	@return			接收到输入返回1，否则返回0，返回-1表示发生错误
 */
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat);

/**
 *	@description	鼠标中断服务函数
 */
void inthandler2c(int *esp);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


