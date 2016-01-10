/***************************************************************************************
 *	FileName					:	console.c
 *	CopyRight					:	1.0
 *	ModuleName					:	console management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	控制台管理的实现文件
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
#include <string.h>
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
 *	@description	初始化一个控制台
 *	@param			宿舍
 *	@notice			此处的设定是控制台只有一个！
 */
void console_task(struct SHEET *sheet, unsigned int memtotal){
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, fifobuf[128], *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;	//控制台结构体
	char cmdline[30];		//命令
	cons.sht = sheet;		//画板
	cons.cur_x =  8;		//光标X坐标
	cons.cur_y = 28;		//光标Y坐标
	cons.cur_c = -1;		//光标颜色
	*((int *) 0x0fec) = (int) &cons;

	//TODO 初始化缓冲、定时器、读FAT表
	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));

	//TODO 命令提示符
	cons_putchar(&cons, '>', 1);

	for (;;) {
		//TODO 读缓冲
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) {
				//TODO 光标用定时器
				//即闪烁光标
				if (i != 0) {
					//TODO 下次置0
					timer_init(timer, &task->fifo, 0);
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_FFFFFF;
					}
				} else {
					//TODO 下次置1
					timer_init(timer, &task->fifo, 1);
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_000000;
					}
				}
				//TODO 再设一次定时为0.5秒
				timer_settime(timer, 50);
			}
			if (i == 2) {
				//TODO 光标ON
				cons.cur_c = COL8_FFFFFF;
			}
			if (i == 3) {
				//TODO 光标OFF
				boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				cons.cur_c = -1;
			}

			//TODO 键盘数据缓冲输入
			if (256 <= i && i <= 511) {
				if (i == 8 + 256) {
					//TODO 退格键，则擦除一位
					if (cons.cur_x > 16) {
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				} else if (i == 10 + 256) {
					//TODO 回车键
					//空格擦除后换行
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);
					//TODO 运行命令
					cons_runcmd(cmdline, &cons, fat, memtotal);
					//显示提示符
					cons_putchar(&cons, '>', 1);
				} else {
					//一般字符
					if (cons.cur_x < 240) {
						//TODO 
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						//TODO 显示字符
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			
			//TODO 重新显示光标
			if (cons.cur_c >= 0) {
				boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			}
			sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
		}
	}
}

/**
 *	@description	向控制台中输入一个符号
 *	@param			cons：指定控制台
 *					chr：欲输入的字符
 *					move：是否后移光标			
 */
void cons_putchar(struct CONSOLE *cons, int chr, char move){
	char s[2];
	s[0] = chr;
	s[1] = 0;

	//TODO 判断是否为制表符
	if (s[0] == 0x09) {
		for (;;) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0) {
				//TODO 被32整除则break
				break;
			}
		}
	} else if (s[0] == 0x0a) {
		//TODO 换行符
		cons_newline(cons);
	} else if (s[0] == 0x0d) {
		//TODO 换车
	} else {
		//TODO 一般字符
		putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, s, 1);
		if (move != 0) {
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
		}
	}
	return;
}


/**
 *	@description	新建一行，换行
 *	@param			cons：需要换行的控制台
 */
void cons_newline(struct CONSOLE *cons){
	int x, y;
	struct SHEET *sheet = cons->sht;
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16;
	} else {
		//TODO 滚动控制台显示
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
			}
		}

		//TODO 最后一行刷新
		for (y = 28 + 112; y < 28 + 128; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	}
	cons->cur_x = 8;
	return;
}

/**
 *	@description	向控制台内写入字符串
 *	@param			cons：指定控制台
 *					s：欲显示的字符串
 */
void cons_putstr0(struct CONSOLE *cons, char *s){
	for (; *s != 0; s++) {
		cons_putchar(cons, *s, 1);
	}
	return;
}

/**
 *	@description	指定长度向控制台内写入字符串
 *	@param			cons：指定控制台
 *					s：欲显示的字符串
 *					l：指定长度
 */
void cons_putstr1(struct CONSOLE *cons, char *s, int l){
	int i;
	for (i = 0; i < l; i++) {
		cons_putchar(cons, s[i], 1);
	}
	return;
}

/**
 *	@description	执行命令字符串的实际命令
 *	@param			cmdline：命令字符串
 *					cons：指定控制台
 *					fat：FAT表
 *					memtotal：内存总量
 */
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal){
	if (strcmp(cmdline, "mem") == 0) {
		cmd_mem(cons, memtotal);
	} else if (strcmp(cmdline, "cls") == 0) {
		cmd_cls(cons);
	} else if (strcmp(cmdline, "dir") == 0) {
		cmd_dir(cons);
	} else if (strncmp(cmdline, "type ", 5) == 0) {
		cmd_type(cons, fat, cmdline);
	} else if (cmdline[0] != 0) {
		if (cmd_app(cons, fat, cmdline) == 0) {
			//不是命令也不是应用程序
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
	return;
}

/**
 *	@description	查看内存的命令
 *	@param			cons：控制台
 *					memtotal：内存可用总数
 */
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char s[60];
	sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	return;
}

/**
 *	@description	执行清屏命令
 *	@param			cons：指定的控制台
 */
void cmd_cls(struct CONSOLE *cons){
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + 128; y++) {
		for (x = 8; x < 8 + 240; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + 240, 28 + 128);
	cons->cur_y = 28;
	return;
}

/**
 *	@description	执行查看文件命令
 *	@param			cons：指定控制台
 */
void cmd_dir(struct CONSOLE *cons){
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	int i, j;
	char s[30];
	for (i = 0; i < 224; i++) {
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if (finfo[i].name[0] != 0xe5) {
			if ((finfo[i].type & 0x18) == 0) {
				sprintf(s, "filename.ext   %7d\n", finfo[i].size);
				for (j = 0; j < 8; j++) {
					s[j] = finfo[i].name[j];
				}
				s[ 9] = finfo[i].ext[0];
				s[10] = finfo[i].ext[1];
				s[11] = finfo[i].ext[2];
				cons_putstr0(cons, s);
			}
		}
	}
	cons_newline(cons);
	return;
}

/**
 *	@description	输出文件内容
 *	@param			cons：制定控制台
 *					fat：FAT表
 *					cmdline：命令行，用于解析出参数的
 */
void cmd_type(struct CONSOLE *cons, int *fat, char *cmdline){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo = file_search(cmdline + 5, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	char *p;
	if (finfo != 0) {
		p = (char *) memman_alloc_4k(memman, finfo->size);
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));
		cons_putstr1(cons, p, finfo->size);
		memman_free_4k(memman, (int) p, finfo->size);
	} else {
		cons_putstr0(cons, "File not found.\n");
	}
	cons_newline(cons);
	return;
}

/**
 *	@description	指定应用程序
 *	@param			cons：指定控制台
 *					fat：FAT表
 *					cmdline：命令行，用于解析出参数的
 *	@return			执行成功返回1，否则返回0
 */
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	char name[18], *p;
	int i;

	//TODO 根据命令行生成文件名
	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {
			break;
		}
		name[i] = cmdline[i];
	}
	//TODO 文件名后的置为0
	name[i] = 0;

	//TODO 寻找文件
	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		//TODO 找不到，添加.hrb后缀进行查找
		name[i    ] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	}

	if (finfo != 0) {
		//TODO 若找到文件了
		p = (char *) memman_alloc_4k(memman, finfo->size);
		*((int *) 0xfe8) = (int) p;

		//TODO 载入到内存中
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));

		//TODO 为该应用程序设置段
		set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER);

		//TODO 添加一个启动头
		if (finfo->size >= 8 && strncmp(p + 4, "Hari", 4) == 0) {
			p[0] = 0xe8;
			p[1] = 0x16;
			p[2] = 0x00;
			p[3] = 0x00;
			p[4] = 0x00;
			p[5] = 0xcb;
		}

		//TODO 执行程序
		farcall(0, 1003 * 8);
		//TODO 执行完毕，释放内存
		memman_free_4k(memman, (int) p, finfo->size);
		cons_newline(cons);
		return 1;
	}
	
	//TODO 找不到返回0
	return 0;
}

/**
 *	@description	应用程序软中断调用的功能解析函数
 *	@param			edi,esi, ebp, esp, ebx, edx, ecx, eax：压栈参数
 *					edx：一般指定该寄存器为功能号
 */
void hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax){
	int cs_base = *((int *) 0xfe8);
	struct CONSOLE *cons = (struct CONSOLE *) *((int *) 0x0fec);
	if (edx == 1) {
		cons_putchar(cons, eax & 0xff, 1);
	} else if (edx == 2) {
		cons_putstr0(cons, (char *) ebx + cs_base);
	} else if (edx == 3) {
		cons_putstr1(cons, (char *) ebx + cs_base, ecx);
	}
	return;
}
