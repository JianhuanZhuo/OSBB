/***************************************************************************************
 *	File Name				:		window.h
 *	CopyRight				:		1.0
 *	ModuleName				:		window module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		window的声明文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __WINDOW_H
#define __WINDOW_H
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
/**
 *	@description	创建一个窗体
 *	@param			buf：VRAM地址
 *					xsize：宽度
 *					ysize：高度
 *					title：标题
 *					act：是否处于激活状态
 */
void make_window8(unsigned char *buf, int xsize, int ysize, char *title, char act);

/**
 *	@description	渲染出一个窗口抬头
 *	@param			buf：VRAM的起始地址
 *					xsize：宽度
 *					title：标题
 *					act：是否处于激活状态
 */
void make_wtitle8(unsigned char *buf, int xsize, char *title, char act);

/**
 *	@description	指定图层中渲染字符串
 *	@param			sht：指定图层
 *					x、y：坐标
 *					c：颜色
 *					b：背景色
 *					s：字符串
 *					l：长度
 */
void putfonts8_asc_sht(struct SHEET *sht, int x, int y, int c, int b, char *s, int l);

/**
 *	@description	渲染一个输入框
 *	@param			sht：指定图层
 *					x0、y0：起始坐标
 *					sx、sy：宽与长
 *					c：底色？
 */
void make_textbox8(struct SHEET *sht, int x0, int y0, int sx, int sy, int c);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


