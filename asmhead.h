/***************************************************************************************
 *	File Name				:		asmhead.h
 *	CopyRight				:		1.0
 *	ModuleName				:		asm module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		汇编功能函数的声明文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __ASM_HEAD_H
#define __ASM_HEAD_H
/**************************************************************
*	Debug switch Section
**************************************************************/

/**************************************************************
*	Include File Section
**************************************************************/
/**************************************************************
*	Macro Define Section
**************************************************************/
#define ADR_BOOTINFO	0x00000ff0
#define ADR_DISKIMG		0x00100000
/**************************************************************
*	Struct Define Section
**************************************************************/
struct BOOTINFO { /* 0x0ff0-0x0fff */
	char cyls;			//启动区读硬盘读到哪为止的
	char leds;			//启动时键盘的LED的状态
	char vmode;			//显卡模式
	char reserve;		//保留
	short scrnx, scrny;	//画面分辨率
	char *vram;			//显存起始地址
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif

