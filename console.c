/***************************************************************************************
 *	FileName					:	console.c
 *	CopyRight					:	1.0
 *	ModuleName					:	console management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	����̨�����ʵ���ļ�
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
 *	@description	��ʼ��һ������̨
 *	@param			����
 *	@notice			�˴����趨�ǿ���ֻ̨��һ����
 */
void console_task(struct SHEET *sheet, unsigned int memtotal){
	struct TIMER *timer;
	struct TASK *task = task_now();
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	int i, fifobuf[128], *fat = (int *) memman_alloc_4k(memman, 4 * 2880);
	struct CONSOLE cons;	//����̨�ṹ��
	char cmdline[30];		//����
	cons.sht = sheet;		//����
	cons.cur_x =  8;		//���X����
	cons.cur_y = 28;		//���Y����
	cons.cur_c = -1;		//�����ɫ
	*((int *) 0x0fec) = (int) &cons;

	//TODO ��ʼ�����塢��ʱ������FAT��
	fifo32_init(&task->fifo, 128, fifobuf, task);
	timer = timer_alloc();
	timer_init(timer, &task->fifo, 1);
	timer_settime(timer, 50);
	file_readfat(fat, (unsigned char *) (ADR_DISKIMG + 0x000200));

	//TODO ������ʾ��
	cons_putchar(&cons, '>', 1);

	for (;;) {
		//TODO ������
		io_cli();
		if (fifo32_status(&task->fifo) == 0) {
			task_sleep(task);
			io_sti();
		} else {
			i = fifo32_get(&task->fifo);
			io_sti();
			if (i <= 1) {
				//TODO ����ö�ʱ��
				//����˸���
				if (i != 0) {
					//TODO �´���0
					timer_init(timer, &task->fifo, 0);
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_FFFFFF;
					}
				} else {
					//TODO �´���1
					timer_init(timer, &task->fifo, 1);
					if (cons.cur_c >= 0) {
						cons.cur_c = COL8_000000;
					}
				}
				//TODO ����һ�ζ�ʱΪ0.5��
				timer_settime(timer, 50);
			}
			if (i == 2) {
				//TODO ���ON
				cons.cur_c = COL8_FFFFFF;
			}
			if (i == 3) {
				//TODO ���OFF
				boxfill8(sheet->buf, sheet->bxsize, COL8_000000, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
				cons.cur_c = -1;
			}

			//TODO �������ݻ�������
			if (256 <= i && i <= 511) {
				if (i == 8 + 256) {
					//TODO �˸���������һλ
					if (cons.cur_x > 16) {
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
					}
				} else if (i == 10 + 256) {
					//TODO �س���
					//�ո��������
					cons_putchar(&cons, ' ', 0);
					cmdline[cons.cur_x / 8 - 2] = 0;
					cons_newline(&cons);
					//TODO ��������
					cons_runcmd(cmdline, &cons, fat, memtotal);
					//��ʾ��ʾ��
					cons_putchar(&cons, '>', 1);
				} else {
					//һ���ַ�
					if (cons.cur_x < 240) {
						//TODO 
						cmdline[cons.cur_x / 8 - 2] = i - 256;
						//TODO ��ʾ�ַ�
						cons_putchar(&cons, i - 256, 1);
					}
				}
			}
			
			//TODO ������ʾ���
			if (cons.cur_c >= 0) {
				boxfill8(sheet->buf, sheet->bxsize, cons.cur_c, cons.cur_x, cons.cur_y, cons.cur_x + 7, cons.cur_y + 15);
			}
			sheet_refresh(sheet, cons.cur_x, cons.cur_y, cons.cur_x + 8, cons.cur_y + 16);
		}
	}
}

/**
 *	@description	�����̨������һ������
 *	@param			cons��ָ������̨
 *					chr����������ַ�
 *					move���Ƿ���ƹ��			
 */
void cons_putchar(struct CONSOLE *cons, int chr, char move){
	char s[2];
	s[0] = chr;
	s[1] = 0;

	//TODO �ж��Ƿ�Ϊ�Ʊ��
	if (s[0] == 0x09) {
		for (;;) {
			putfonts8_asc_sht(cons->sht, cons->cur_x, cons->cur_y, COL8_FFFFFF, COL8_000000, " ", 1);
			cons->cur_x += 8;
			if (cons->cur_x == 8 + 240) {
				cons_newline(cons);
			}
			if (((cons->cur_x - 8) & 0x1f) == 0) {
				//TODO ��32������break
				break;
			}
		}
	} else if (s[0] == 0x0a) {
		//TODO ���з�
		cons_newline(cons);
	} else if (s[0] == 0x0d) {
		//TODO ����
	} else {
		//TODO һ���ַ�
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
 *	@description	�½�һ�У�����
 *	@param			cons����Ҫ���еĿ���̨
 */
void cons_newline(struct CONSOLE *cons){
	int x, y;
	struct SHEET *sheet = cons->sht;
	if (cons->cur_y < 28 + 112) {
		cons->cur_y += 16;
	} else {
		//TODO ��������̨��ʾ
		for (y = 28; y < 28 + 112; y++) {
			for (x = 8; x < 8 + 240; x++) {
				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
			}
		}

		//TODO ���һ��ˢ��
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
 *	@description	�����̨��д���ַ���
 *	@param			cons��ָ������̨
 *					s������ʾ���ַ���
 */
void cons_putstr0(struct CONSOLE *cons, char *s){
	for (; *s != 0; s++) {
		cons_putchar(cons, *s, 1);
	}
	return;
}

/**
 *	@description	ָ�����������̨��д���ַ���
 *	@param			cons��ָ������̨
 *					s������ʾ���ַ���
 *					l��ָ������
 */
void cons_putstr1(struct CONSOLE *cons, char *s, int l){
	int i;
	for (i = 0; i < l; i++) {
		cons_putchar(cons, s[i], 1);
	}
	return;
}

/**
 *	@description	ִ�������ַ�����ʵ������
 *	@param			cmdline�������ַ���
 *					cons��ָ������̨
 *					fat��FAT��
 *					memtotal���ڴ�����
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
			//��������Ҳ����Ӧ�ó���
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
	return;
}

/**
 *	@description	�鿴�ڴ������
 *	@param			cons������̨
 *					memtotal���ڴ��������
 */
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char s[60];
	sprintf(s, "total   %dMB\nfree %dKB\n\n", memtotal / (1024 * 1024), memman_total(memman) / 1024);
	cons_putstr0(cons, s);
	return;
}

/**
 *	@description	ִ����������
 *	@param			cons��ָ���Ŀ���̨
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
 *	@description	ִ�в鿴�ļ�����
 *	@param			cons��ָ������̨
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
 *	@description	����ļ�����
 *	@param			cons���ƶ�����̨
 *					fat��FAT��
 *					cmdline�������У����ڽ�����������
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
 *	@description	ָ��Ӧ�ó���
 *	@param			cons��ָ������̨
 *					fat��FAT��
 *					cmdline�������У����ڽ�����������
 *	@return			ִ�гɹ�����1�����򷵻�0
 */
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	struct FILEINFO *finfo;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	char name[18], *p;
	int i;

	//TODO ���������������ļ���
	for (i = 0; i < 13; i++) {
		if (cmdline[i] <= ' ') {
			break;
		}
		name[i] = cmdline[i];
	}
	//TODO �ļ��������Ϊ0
	name[i] = 0;

	//TODO Ѱ���ļ�
	finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	if (finfo == 0 && name[i - 1] != '.') {
		//TODO �Ҳ��������.hrb��׺���в���
		name[i    ] = '.';
		name[i + 1] = 'H';
		name[i + 2] = 'R';
		name[i + 3] = 'B';
		name[i + 4] = 0;
		finfo = file_search(name, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), 224);
	}

	if (finfo != 0) {
		//TODO ���ҵ��ļ���
		p = (char *) memman_alloc_4k(memman, finfo->size);
		*((int *) 0xfe8) = (int) p;

		//TODO ���뵽�ڴ���
		file_loadfile(finfo->clustno, finfo->size, p, fat, (char *) (ADR_DISKIMG + 0x003e00));

		//TODO Ϊ��Ӧ�ó������ö�
		set_segmdesc(gdt + 1003, finfo->size - 1, (int) p, AR_CODE32_ER);

		//TODO ���һ������ͷ
		if (finfo->size >= 8 && strncmp(p + 4, "Hari", 4) == 0) {
			p[0] = 0xe8;
			p[1] = 0x16;
			p[2] = 0x00;
			p[3] = 0x00;
			p[4] = 0x00;
			p[5] = 0xcb;
		}

		//TODO ִ�г���
		farcall(0, 1003 * 8);
		//TODO ִ����ϣ��ͷ��ڴ�
		memman_free_4k(memman, (int) p, finfo->size);
		cons_newline(cons);
		return 1;
	}
	
	//TODO �Ҳ�������0
	return 0;
}

/**
 *	@description	Ӧ�ó������жϵ��õĹ��ܽ�������
 *	@param			edi,esi, ebp, esp, ebx, edx, ecx, eax��ѹջ����
 *					edx��һ��ָ���üĴ���Ϊ���ܺ�
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
