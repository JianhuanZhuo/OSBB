/***************************************************************************************
 *	File Name				:		naskfunc.h
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
#ifndef __NASK_FUNC_H
#define __NASK_FUNC_H
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
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
void io_hlt(void);
void io_cli(void);
void io_sti(void);
void io_stihlt(void);
int io_in8(int port);
void io_out8(int port, int data);
int io_load_eflags(void);
void io_store_eflags(int eflags);
void load_gdtr(int limit, int addr);
void load_idtr(int limit, int addr);
int load_cr0(void);
void store_cr0(int cr0);
void load_tr(int tr);
void asm_inthandler20(void);
void asm_inthandler21(void);
void asm_inthandler27(void);
void asm_inthandler2c(void);
unsigned int memtest_sub(unsigned int start, unsigned int end);
void farjmp(int eip, int cs);
void farcall(int eip, int cs);
void asm_hrb_api(void);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


