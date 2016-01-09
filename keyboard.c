/***************************************************************************************
 *	FileName					:	keyboard.c
 *	CopyRight					:	1.0
 *	ModuleName					:	keyboard management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	键盘输入输出模块管理
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
#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
/**************************************************************
*	Struct Define Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
struct FIFO32 *keyfifo;		//键盘关联的缓冲区
int keydata0;				//键盘使用的关键字
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	初始化键盘模块
 *	@param			fifo：关联键盘的缓冲
 *					data0：键盘使用的关键字
 */
void init_keyboard(struct FIFO32 *fifo, int data0){
	keyfifo = fifo;
	keydata0 = data0;
	
	//等待键盘准备
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}


/**
 *	@description	等待键盘就绪
 */
void wait_KBC_sendready(void){

	//TODO 循环等待键盘准备就绪
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

/**
 *	@description	键盘中断服务函数
 */
void inthandler21(int *esp){
	int data;
	io_out8(PIC0_OCW2, 0x61);	//清空中断标志位
	data = io_in8(PORT_KEYDAT);
	fifo32_put(keyfifo, data + keydata0);
	return;
}

