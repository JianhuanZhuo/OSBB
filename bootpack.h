/***************************************************************************************
 *	File Name				:		bootpack.h
 *	CopyRight				:		1.0
 *	ModuleName				:		
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __BOOT_PACK_H
#define __BOOT_PACK_H
/**************************************************************
*	Debug switch Section
**************************************************************/

/**************************************************************
*	Include File Section
**************************************************************/
#include "asmhead.h"
#include "naskfunc.h"
#include "fifo.h"
#include "graphic.h"
#include "memory.h"
#include "sheet.h"
#include "timer.h"
#include "mtask.h"
#include "int.h"
#include "dsctbl.h"
#include "keyboard.h"
#include "mouse.h"
#include "window.h"
#include "file.h"
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

/* console.c */
struct CONSOLE {
	struct SHEET *sht;
	int cur_x, cur_y, cur_c;
};
void console_task(struct SHEET *sheet, unsigned int memtotal);
void cons_putchar(struct CONSOLE *cons, int chr, char move);
void cons_newline(struct CONSOLE *cons);
void cons_putstr0(struct CONSOLE *cons, char *s);
void cons_putstr1(struct CONSOLE *cons, char *s, int l);
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal);
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);
void cmd_cls(struct CONSOLE *cons);
void cmd_dir(struct CONSOLE *cons);
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline);
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);
void hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);



/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif




