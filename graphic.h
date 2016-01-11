/***************************************************************************************
 *	File Name				:		graphic.h
 *	CopyRight				:		1.0
 *	ModuleName				:		graphic management module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		图形管理接口文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __GRAPHIC_H
#define __GRAPHIC_H
/**************************************************************
*	Debug switch Section
**************************************************************/

/**************************************************************
*	Include File Section
**************************************************************/
/**************************************************************
*	Macro Define Section
**************************************************************/
#define COL8_000000		0
#define COL8_FF0000		1
#define COL8_00FF00		2
#define COL8_FFFF00		3
#define COL8_0000FF		4
#define COL8_FF00FF		5
#define COL8_00FFFF		6
#define COL8_FFFFFF		7
#define COL8_C6C6C6		8
#define COL8_840000		9
#define COL8_008400		10
#define COL8_848400		11
#define COL8_000084		12
#define COL8_840084		13
#define COL8_008484		14
#define COL8_848484		15
/**************************************************************
*	Struct Define Section
**************************************************************/
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
extern char *debugStr[300];
extern int debugLine;
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@description	初始化调色板
 */
void init_palette(void);

/**
 *	@description	设置调色板
 *	@param			start：设置起始标号
 *					end：设置终止标号
 *					rgb：调色板颜色
 */
void set_palette(int start, int end, unsigned char *rgb);

/**
 *	@description	渲染一个矩形
 *	@param			vram：VRAM起始地址
 *					xsize：屏幕宽度
 *					c：渲染颜色
 *					x0、y0：起始坐标
 *					x1、y1：结束坐标
 *	@notice			此处的屏幕宽度参数存在问题，应该由函数自行读取，而非用户指定
 */
void boxfill8(unsigned char *vram, int xsize, unsigned char c, int x0, int y0, int x1, int y1);

/**
 *	@description	初始化屏幕
 *	@param			vram：VRAM起始地址
 *					x：屏幕宽度
 *					y：屏幕高度
 */
void init_screen8(char *vram, int x, int y);

/**
 *	@description	渲染出一个字符
 *	@param			vram：VRAM起始地址
 *					xsize：屏幕宽度
 *					x、y：字符所在坐标
 *					c：字符颜色
 *					font：字符点阵
 */
void putfont8(char *vram, int xsize, int x, int y, char c, char *font);

/**
 *	@description	渲染出一串字符串
 *	@param			vram：VRAM起始地址
 *					xsize：屏幕宽度
 *					x、y：字符串所在起始坐标
 *					c：字符串颜色
 *					s：欲渲染字符串
 */
void putfonts8_asc(char *vram, int xsize, int x, int y, char c, unsigned char *s);

/**
 *	@description	初始化鼠标图形
 *	@param			mouse：存放鼠标图形的目标地址
 *					bc：背景颜色
 */
void init_mouse_cursor8(char *mouse, char bc);

/**
 *	@description	绘制指定矩形的图形
 *	@param			vram：VRAM起始地址
 *					vxsize：屏幕宽度
 *					pxsize、pysize：图形宽高
 *					px0、py0：绘制起始坐标
 *					buf：存放有图形的缓冲区
 *					bxsize：buf的宽度
 */
void putblock8_8(char *vram, int vxsize, int pxsize,int pysize, int px0, int py0, char *buf, int bxsize);

/**
 *	@description	DEBUG指定行输出调试信息
 */
void debug(int line, char *str);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


