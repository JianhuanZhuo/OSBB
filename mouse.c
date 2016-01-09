/***************************************************************************************
 *	FileName					:	mouse.c
 *	CopyRight					:	1.0
 *	ModuleName					:	mouse management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	鼠标输入模块管理
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
/**************************************************************
*	Struct Define Section
**************************************************************/
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
struct FIFO32 *mousefifo;		//鼠标关联的缓冲区
int mousedata0;					//鼠标关键字
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	使能鼠标模块
 *	@param			fifo：鼠标关联的缓冲区
 *					data0：鼠标关键字
 *					mdec：鼠标描述符
 */
void enable_mouse(struct FIFO32 *fifo, int data0, struct MOUSE_DEC *mdec){
	mousefifo = fifo;
	mousedata0 = data0;

	//TODO 等待鼠标就绪
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	
	//TODO 初始化鼠标接收状态
	mdec->phase = 0;
	return;
}

/**
 *	@description	解码鼠标
 *	@param			mdec：鼠标描述符
 *					dat：当前的鼠标输入
 *	@return			接收到输入返回1，否则返回0，返回-1表示发生错误
 */
int mouse_decode(struct MOUSE_DEC *mdec, unsigned char dat){
	if (mdec->phase == 0) {
		/* 儅僂僗偺0xfa傪懸偭偰偄傞抜奒 */
		if (dat == 0xfa) {
			mdec->phase = 1;
		}
		return 0;
	}
	if (mdec->phase == 1) {
		/* 儅僂僗偺1僶僀僩栚傪懸偭偰偄傞抜奒 */
		if ((dat & 0xc8) == 0x08) {
			/* 惓偟偄1僶僀僩栚偩偭偨 */
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	if (mdec->phase == 2) {
		/* 儅僂僗偺2僶僀僩栚傪懸偭偰偄傞抜奒 */
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	if (mdec->phase == 3) {
		/* 儅僂僗偺3僶僀僩栚傪懸偭偰偄傞抜奒 */
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->btn = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if ((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if ((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y; /* 儅僂僗偱偼y曽岦偺晞崋偑夋柺偲斀懳 */
		return 1;
	}

	//TODO 程序运行到这里表示发生错误
	return -1;
}

/**
 *	@description	鼠标中断服务函数
 */
void inthandler2c(int *esp){
	int data;
	io_out8(PIC1_OCW2, 0x64);	//清空PIC1 IRQ-12中断标志
	io_out8(PIC0_OCW2, 0x62);	//清空PIC0 IRQ-02中断标志
	data = io_in8(PORT_KEYDAT);
	fifo32_put(mousefifo, data + mousedata0);
	return;
}

