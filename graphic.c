/***************************************************************************************
 *	FileName					:	graphic.c
 *	CopyRight					:	1.0
 *	ModuleName					:	graphic management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	图形界面的实现文件
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
 *	@description	初始化调色板
 */
void init_palette(void){
	static unsigned char table_rgb[16 * 3] = {
		0x00, 0x00, 0x00,	/*  0:黑   */
		0xff, 0x00, 0x00,	/*  1:亮红 */
		0x00, 0xff, 0x00,	/*  2:亮绿 */
		0xff, 0xff, 0x00,	/*  3:亮黄 */
		0x00, 0x00, 0xff,	/*  4:亮蓝 */
		0xff, 0x00, 0xff,	/*  5:亮紫 */
		0x00, 0xff, 0xff,	/*  6:浅蓝 */
		0xff, 0xff, 0xff,	/*  7:白   */
		0xc6, 0xc6, 0xc6,	/*  8:亮灰 */
		0x84, 0x00, 0x00,	/*  9:暗红 */
		0x00, 0x84, 0x00,	/* 10:暗绿 */
		0x84, 0x84, 0x00,	/* 11:暗黄 */
		0x00, 0x00, 0x84,	/* 12:暗青 */
		0x84, 0x00, 0x84,	/* 13:暗紫 */
		0x00, 0x84, 0x84,	/* 14:浅蓝 */
		0x84, 0x84, 0x84	/* 15:暗灰 */
	};
	set_palette(0, 15, table_rgb);
	return;
}

/**
 *	@description	设置调色板
 *	@param			start：设置起始标号
 *					end：设置终止标号
 *					rgb：调色板颜色
 */
void set_palette(int start, int end, unsigned char *rgb){
	int i, eflags;
	eflags = io_load_eflags();	//记录中断许可标志位的值
	io_cli(); 					//关闭中断
	io_out8(0x03c8, start);
	for (i = start; i <= end; i++) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);	//恢复中断
	return;
}

/**
 *	@description	渲染一个矩形
 *	@param			vram：VRAM起始地址
 *					xsize：屏幕宽度
 *					c：渲染颜色
 *					x0、y0：起始坐标
 *					x1、y1：结束坐标
 *	@notice			此处的屏幕宽度参数存在问题，应该由函数自行读取，而非用户指定
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
 *	@description	初始化屏幕
 *	@param			vram：VRAM起始地址
 *					x：屏幕宽度
 *					y：屏幕高度
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
 *	@description	渲染出一个字符
 *	@param			vram：VRAM起始地址
 *					xsize：屏幕宽度
 *					x、y：字符所在坐标
 *					c：字符颜色
 *					font：字符点阵
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
 *	@description	渲染出一串字符串
 *	@param			vram：VRAM起始地址
 *					xsize：屏幕宽度
 *					x、y：字符串所在起始坐标
 *					c：字符串颜色
 *					s：欲渲染字符串
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
 *	@description	初始化鼠标图形
 *	@param			mouse：存放鼠标图形的目标地址
 *					bc：背景颜色
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
 *	@description	绘制指定矩形的图形
 *	@param			vram：VRAM起始地址
 *					vxsize：屏幕宽度
 *					pxsize、pysize：图形宽高
 *					px0、py0：绘制起始坐标
 *					buf：存放有图形的缓冲区
 *					bxsize：buf的宽度
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
 *	@description	DEBUG指定行输出调试信息
 */
void debug(int line, char *str){
	line+=2;
	boxfill8(binfo->vram, binfo->scrnx, COL8_000000, 0, binfo->scrny-line*16, binfo->scrnx, binfo->scrny-line*16);
	putfonts8_asc(binfo->vram, binfo->scrnx, 0, binfo->scrny-line*16, COL8_FFFFFF, str);
}
