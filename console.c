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
#include "stack.h"
/**************************************************************
*	Macro Define Section
**************************************************************/
#define consContW	240
#define consContH	128
/*************************************************************
*	Struct Define Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/

/**************************************************************
*	Global Variable Declare Section
**************************************************************/
struct pathEnv pEnv;
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
	int cmdLineNum = 0;		//�������
	char pathLine[30];		
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

	//TODO ����FS
	setupFS(fat, (struct FILEINFO *) (ADR_DISKIMG + 0x002600), (char *) (ADR_DISKIMG + 0x003e00));
	//TODO ·��ջ
	initPathEnv(&pEnv);

	//TODO ������ʾ��
	getPathCurrent(&pEnv, &pathLine);
	cons_putstr0(&cons, pathLine);
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
					if (cmdLineNum > 0) {
						cons_putchar(&cons, ' ', 0);
						cons.cur_x -= 8;
						cmdLineNum--;
					}
				} else if (i == 10 + 256) {
					//TODO �س���
					//�ո��������
					cons_putchar(&cons, ' ', 0);
					cmdline[cmdLineNum++] = 0;
					cons_newline(&cons);
					//TODO ��������
					cons_runcmd(cmdline, &cons, fat, memtotal);
					cmdLineNum = 0;
					//��ʾ��ʾ��
					getPathCurrent(&pEnv, &pathLine);
					cons_putstr0(&cons, pathLine);
					cons_putchar(&cons, '>', 1);
				} else {
					//һ���ַ�
					if (cons.cur_x < consContW) {
						//TODO 
						cmdline[cmdLineNum++] = i - 256;
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
			if (cons->cur_x == 8 + consContW) {
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
			if (cons->cur_x == 8 + consContW) {
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
			for (x = 8; x < 8 + consContW; x++) {
				sheet->buf[x + y * sheet->bxsize] = sheet->buf[x + (y + 16) * sheet->bxsize];
			}
		}

		//TODO ���һ��ˢ��
		for (y = 28 + 112; y < 28 + consContH; y++) {
			for (x = 8; x < 8 + consContW; x++) {
				sheet->buf[x + y * sheet->bxsize] = COL8_000000;
			}
		}
		sheet_refresh(sheet, 8, 28, 8 + consContW, 28 + consContH);
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
	} else if (strcmp(cmdline, "clean") == 0) {
		cmd_clean(cons);
	} else if (strcmp(cmdline, "ls") == 0) {
		cmd_ls(cons);
	} else if (strcmp(cmdline, "mkdir") == 0) {
		cmd_mkdir(cons);
	} else if (strncmp(cmdline, "cat ", 4) == 0) {
		cmd_cat(cons, cmdline);
	} else if (strncmp(cmdline, "cd ", 3) == 0) {
		cmd_cd(cons, cmdline);
	} else if (strncmp(cmdline, "rm ", 3) == 0) {
		cmd_rm(cons, cmdline+3);
	} else if (strncmp(cmdline, "touch ", 6) == 0) {
		cmd_touch(cons, cmdline+6);
	} else if (cmdline[0] != 0) {
		if (cmd_app(cons, fat, cmdline) == 0) {
			//��������Ҳ����Ӧ�ó���
			cons_putstr0(cons, "Bad command.\n\n");
		}
	}
	return;
}


/**
 *	@description	ɾ��һ���ļ�
 *	@param			cons������̨
 *					cmdline��������
 */
void cmd_rm(struct CONSOLE *cons, char *cmdline){
	
	struct FILEINFO* dirCur;
	dirCur = getDirCurrent(&pEnv);
	int i;
	char fileName[12];
	for(i=0; i<11&&cmdline[i]!=0x00; i++){
		fileName[i]=cmdline[i];
	}
	fileName[i] = 0x00;

	deleteByName(fileName, dirCur);
}


/**
 *	@description	����һ���ļ�
 *	@param			cons������̨
 *					cmdline��������
 */
void cmd_touch(struct CONSOLE *cons, char *cmdline){
	
	struct FILEINFO* dirCur;
	dirCur = getDirCurrent(&pEnv);
	int i;
	char fileName[9];
	char ext[4];
	for(i=0; i<8&&cmdline[i]!='.'&&cmdline[i]!=0x00; i++){
		fileName[i]=cmdline[i];
	}
	fileName[i] = 0x00;
	
	if(cmdline[i]=='.'){
		for(cmdline+=(i+1), i=0; i<3&&cmdline[i]!=0x00; i++){
			ext[i]=cmdline[i];
		}
	}
	ext[i]=0x00;

	if(createFile(fileName, ext, dirCur, 0)){
		cons_putstr0(cons, "File has exist!\n\n");
	}
}

/**
 *	@description	�л�·��
 *	@param			cons���ƶ�����̨
 *					cmdline�������У����ڽ�����������
 *	@notice			�˴��ĺ�����Ҫ�޸���֧�����·��
 */
void cmd_cd(struct CONSOLE *cons, char *cmdline){
	
	struct FILEINFO* dirCur;
	struct FILEINFO *dirTar; 
	//TODO ����л�Ŀ��
	if(cmdline[3]=='.' &&cmdline[4]=='.'){
		//TODO ������һ��
		cdBack(&pEnv);
	}else{
		dirCur = getDirCurrent(&pEnv);
		dirTar = fileSearch(cmdline + 3, dirCur);
		if(dirTar){
			if(isFolder(dirTar)){
				cdToDir(&pEnv, dirTar);
			}else{
				cons_putstr0(cons, "Target is not a folder!\n\n");
			}
		}else{
			cons_putstr0(cons, "Target not exist!\n\n");
		}
	}
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
void cmd_clean(struct CONSOLE *cons){
	int x, y;
	struct SHEET *sheet = cons->sht;
	for (y = 28; y < 28 + consContH; y++) {
		for (x = 8; x < 8 + consContW; x++) {
			sheet->buf[x + y * sheet->bxsize] = COL8_000000;
		}
	}
	sheet_refresh(sheet, 8, 28, 8 + consContW, 28 + consContH);
	cons->cur_y = 28;
	return;
}

/**
 *	@description	��ʾĿ¼�µ������ļ���Ϣ
 */
void cmd_ls(struct CONSOLE *cons){
	struct FILEINFO* dirCur= getDirCurrent(&pEnv);
	struct FILEINFO *finfo;
	int size;
	if(dirCur){
		finfo = (struct FILEINFO *)malloc(dirCur->size);
		file_loadfile(dirCur->clustno, dirCur->size, finfo);
		size = dirCur->size/0x20;
	}else{
		finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
		size = 224;
	}
	{
		int i, j;
		char s[30] = "..\n";
		cons_putstr0(cons, s);
		s[1]='\n';
		s[2]=0x00;
		cons_putstr0(cons, s);
		//TODO ����ʾ���ļ���
		for (i = 0; i < size; i++) {
			if (finfo[i].name[0] == 0x00) {
				break;
			}
			if (finfo[i].name[0] != 0xe5) {
				if(finfo[i].type & 0x10){
					//TODO ��Ŀ¼
					sprintf(s, "filename\n");
					for (j = 0; j < 8; j++) {
						s[j] = finfo[i].name[j];
					}
					cons_putstr0(cons, s);
				}
			}
		}
		for (i = 0; i < size; i++) {
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
	}
	if(dirCur){		
		mfree(dirCur->size, finfo);
	}
}

/**
 *	@description	ִ�д���Ŀ¼�ļ�����
 *	@param			cons��ָ������̨
 */
void cmd_mkdir(struct CONSOLE *cons){
	struct FILEINFO *finfo = (struct FILEINFO *) (ADR_DISKIMG + 0x002600);
	struct FILEINFO *fp;

	deleteByName("hello.hrb", 0);
	deleteByName("hello2.hrb", 0);
	deleteByName("hello3.hrb", 0);
	deleteByName("A.hrb", 0);
	deleteByName("make.bat", 0);

	{
		char ss[30];
		createFile("xx", "txt", 0, 0);
		fp = fileSearch("xx.txt", 0);
		append(fp, "xxxxxxxxxx", 10);
		sprintf(ss, "File create %p \n", fp);
		cons_putstr0(cons, ss);
		
		createFile("folder", "", 0, 1);
		fp = fileSearch("folder", 0);
		createFile("xx", "txt", fp, 0);
		createFile("ww", "ww", fp, 0);
	}

	return;
}

/**
 *	@description	����ļ�����
 *	@param			cons���ƶ�����̨
 *					fat��FAT��
 *					cmdline�������У����ڽ�����������
 */
void cmd_cat(struct CONSOLE *cons, char *cmdline){
	struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
	char *p;
	struct FILEINFO *finfo = fileSearch(cmdline + 4, 0);
	if (finfo != 0) {
		p = (char *) memman_alloc_4k(memman, finfo->size);
		//sprintf(debugStr, "clustno %d, size %d", finfo->clustno, finfo->size);
		//debug(debugLine++, debugStr);
		file_loadfile(finfo->clustno, finfo->size, p);
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
	finfo = fileSearch(name, 0);
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
		file_loadfile(finfo->clustno, finfo->size, p);

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
