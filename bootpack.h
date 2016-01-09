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
/* window.c */
void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);

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

/* file.c */
struct FILEINFO {
	unsigned char name[8], ext[3], type;
	char reserve[10];
	unsigned short time, date, clustno;
	unsigned int size;
};
void file_readfat(int *fat, unsigned char *img);
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);


/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif




