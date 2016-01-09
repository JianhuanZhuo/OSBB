/***************************************************************************************
 *	FileName					:	mtask.c
 *	CopyRight					:	1.0
 *	ModuleName					:	multi task management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	多任务管理的实现文件
 *									task->flags = 2;表示正在运行中
 *									task->flags = 1;表示正在使用中
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
#include <stdio.h>
/**************************************************************
*	Macro Define Section
**************************************************************/
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
 *	@description	初始化中断PIC设置
 */
void init_pic(void){
	io_out8(PIC0_IMR,  0xff  ); //禁止PIC0所有中断
	io_out8(PIC1_IMR,  0xff  ); //禁止PIC1所有中断

	io_out8(PIC0_ICW1, 0x11  ); //边沿触发模式
	io_out8(PIC0_ICW2, 0x20  ); //IRQ0-7由INT20-27接收
	io_out8(PIC0_ICW3, 1 << 2); //PIC1由IRQ2连接
	io_out8(PIC0_ICW4, 0x01  ); //无缓冲区模式

	io_out8(PIC1_ICW1, 0x11  ); //边沿触发模式
	io_out8(PIC1_ICW2, 0x28  ); //IRQ8-15由INT28-2f接收
	io_out8(PIC1_ICW3, 2     ); //PIC1由IRQ2连接
	io_out8(PIC1_ICW4, 0x01  ); //无缓冲区模式

	io_out8(PIC0_IMR,  0xfb  ); //11111011 PIC1之外全部禁止
	io_out8(PIC1_IMR,  0xff  ); //禁止PIC1所有中断

	return;
}

void inthandler27(int *esp)
/* PIC0偐傜偺晄姰慡妱傝崬傒懳嶔 */
/* Athlon64X2婡側偳偱偼僠僢僾僙僢僩偺搒崌偵傛傝PIC偺弶婜壔帪偵偙偺妱傝崬傒偑1搙偩偗偍偙傞 */
/* 偙偺妱傝崬傒張棟娭悢偼丄偦偺妱傝崬傒偵懳偟偰壗傕偟側偄偱傗傝夁偛偡 */
/* 側偤壗傕偟側偔偰偄偄偺丠
	仺  偙偺妱傝崬傒偼PIC弶婜壔帪偺揹婥揑側僲僀僘偵傛偭偰敪惗偟偨傕偺側偺偱丄
		傑偠傔偵壗偐張棟偟偰傗傞昁梫偑側偄丅									*/
{
	io_out8(PIC0_OCW2, 0x67); /* IRQ-07庴晅姰椆傪PIC偵捠抦 */
	return;
}
