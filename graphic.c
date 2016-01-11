/***************************************************************************************
 *	FileName					:	graphic.c
 *	CopyRight					:	1.0
 *	ModuleName					:	graphic management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	ͼ�ν����ʵ���ļ�
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
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
extern struct BOOTINFO *binfo;
char *debugStr[300];
int debugLine;
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	��ʼ����ɫ��
 */
void init_palette(void){
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:��   */
		0xff, 0x00, 0x00,	/*  1:���� */
		0x00, 0xff, 0x00,	/*  2:���� */
		0xff, 0xff, 0x00,	/*  3:���� */
		0x00, 0x00, 0xff,	/*  4:���� */
		0xff, 0x00, 0xff,	/*  5:���� */
		0x00, 0xff, 0xff,	/*  6:ǳ�� */
		0xff, 0xff, 0xff,	/*  7:��   */
		0xc6, 0xc6, 0xc6,	/*  8:���� */
		0x84, 0x00, 0x00,	/*  9:���� */
		0x00, 0x84, 0x00,	/* 10:���� */
		0x84, 0x84, 0x00,	/* 11:���� */
		0x00, 0x00, 0x84,	/* 12:���� */
		0x84, 0x00, 0x84,	/* 13:���� */
		0x00, 0x84, 0x84,	/* 14:ǳ�� */
		0x84, 0x84, 0x84	/* 15:���� */
	};
	set_palette(0, 15, table_rgb);
	return;
}

/**
 *	@description	���õ�ɫ��
 *	@param			start��������ʼ���
 *					end��������ֹ���
 *					rgb����ɫ����ɫ
 */
void set_palette(int start, int end, unsigned char *rgb){
	int i, eflags;
	eflags = io_load_eflags();	//��¼�ж���ɱ�־λ��ֵ
	io_cli(); 					//�ر��ж�
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	//�ָ��ж�
	return;
}

/**
 *	@description	��Ⱦһ������
 *	@param			vram��VRAM��ʼ��ַ
 *					xsize����Ļ���
 *					c����Ⱦ��ɫ
 *					x0��y0����ʼ����
 *					x1��y1����������
 *	@notice			�˴�����Ļ��Ȳ����������⣬Ӧ���ɺ������ж�ȡ�������û�ָ��
 */
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1){
	int x, y;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++)
			vram[y * xsize + x] = c;
	}
	return;
}

/**
 *	@description	��ʼ����Ļ
 *	@param			vram��VRAM��ʼ��ַ
 *					x����Ļ���
 *					y����Ļ�߶�
 */
void init_screen8(char *vram, int x, int y){
	boxfill8(vram, x, COL8_008484,  0,     0,      x -  1, y - 29);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 28, x -  1, y - 28);
	boxfill8(vram, x, COL8_FFFFFF,  0,     y - 27, x -  1, y - 27);
	boxfill8(vram, x, COL8_C6C6C6,  0,     y - 26, x -  1, y -  1);

	boxfill8(vram, x, COL8_FFFFFF,  3,     y - 24, 59,     y - 24);
	boxfill8(vram, x, COL8_FFFFFF,  2,     y - 24,  2,     y -  4);
	boxfill8(vram, x, COL8_848484,  3,     y -  4, 59,     y -  4);
	boxfill8(vram, x, COL8_848484, 59,     y - 23, 59,     y -  5);
	boxfill8(vram, x, COL8_000000,  2,     y -  3, 59,     y -  3);
	boxfill8(vram, x, COL8_000000, 60,     y - 24, 60,     y -  3);

	boxfill8(vram, x, COL8_848484, x - 47, y - 24, x -  4, y - 24);
	boxfill8(vram, x, COL8_848484, x - 47, y - 23, x - 47, y -  4);
	boxfill8(vram, x, COL8_FFFFFF, x - 47, y -  3, x -  4, y -  3);
	boxfill8(vram, x, COL8_FFFFFF, x -  3, y - 24, x -  3, y -  3);
	return;
}

/**
 *	@description	��Ⱦ��һ���ַ�
 *	@param			vram��VRAM��ʼ��ַ
 *					xsize����Ļ���
 *					x��y���ַ���������
 *					c���ַ���ɫ
 *					font���ַ�����
 */
void putfont8(char *vram, int xsize, int x, int y, char c, char *font){
	int i;
	char *p, d /* data */;
	for (i = 0; i < 16; i++) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if ((d & 0x80) != 0) { p[0] = c; }
		if ((d & 0x40) != 0) { p[1] = c; }
		if ((d & 0x20) != 0) { p[2] = c; }
		if ((d & 0x10) != 0) { p[3] = c; }
		if ((d & 0x08) != 0) { p[4] = c; }
		if ((d & 0x04) != 0) { p[5] = c; }
		if ((d & 0x02) != 0) { p[6] = c; }
		if ((d & 0x01) != 0) { p[7] = c; }
	}
	return;
}

/**
 *	@description	��Ⱦ��һ���ַ���
 *	@param			vram��VRAM��ʼ��ַ
 *					xsize����Ļ���
 *					x��y���ַ���������ʼ����
 *					c���ַ�����ɫ
 *					s������Ⱦ�ַ���
 */
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s){
	extern char hankaku[4096];
	for (; *s != 0x00; s++) {
		putfont8(vram, xsize, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

/**
 *	@description	��ʼ�����ͼ��
 *	@param			mouse��������ͼ�ε�Ŀ���ַ
 *					bc��������ɫ
 */
void init_mouse_cursor8(char *mouse, char bc){
	static char cursor[16][16] = {
		"**..............",
		"*O*.............",
		"*OO*............",
		"*OOO*...........",
		"*OOOO*..........",
		"*OOOOO*.........",
		"*OOOOOO*........",
		"*OOOOOOO*.......",
		"*OOOOOOOO*......",
		"*OOOOO*****.....",
		"*OO*OO*.........",
		"*O*.*OO*........",
		"**..*OO*........",
		"*....*OO*.......",
		".....*OO*.......",
		"......***......."
	};
	int x, y;

	for (y = 0; y < 16; y++) {
		for (x = 0; x < 16; x++) {
			if (cursor[y][x] == '*') {
				mouse[y * 16 + x] = COL8_000000;
			}
			if (cursor[y][x] == 'O') {
				mouse[y * 16 + x] = COL8_FFFFFF;
			}
			if (cursor[y][x] == '.') {
				mouse[y * 16 + x] = bc;
			}
		}
	}
	return;
}

/**
 *	@description	����ָ�����ε�ͼ��
 *	@param			vram��VRAM��ʼ��ַ
 *					vxsize����Ļ���
 *					pxsize��pysize��ͼ�ο��
 *					px0��py0��������ʼ����
 *					buf�������ͼ�εĻ�����
 *					bxsize��buf�Ŀ��
 */
void putblock8_8(char *vram, int vxsize, int pxsize,int pysize, int px0, int py0, char *buf, int bxsize){
	int x, y;
	for (y = 0; y < pysize; y++) {
		for (x = 0; x < pxsize; x++) {
			vram[(py0 + y) * vxsize + (px0 + x)] = buf[y * bxsize + x];
		}
	}
	return;
}


/**
 *	@description	DEBUGָ�������������Ϣ
 */
void debug(int line, char *str){
	line+=2;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, binfo->scrny-line*16, binfo->scrnx, binfo->scrny-line*16);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, binfo->scrny-line*16, COL8_FFFFFF, str);
}
