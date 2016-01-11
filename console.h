/***************************************************************************************
 *	File Name				:		console.h
 *	CopyRight				:		1.0
 *	ModuleName				:		console module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		控制台功能函数的声明文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __CONSOLE_H
#define __CONSOLE_H
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
struct CONSOLE {
	struct SHEET *sht;	//控制台画板
	int cur_x;			//光标x轴
	int cur_y;			//光标y轴
	int cur_c;			//光标颜色
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
void console_task(struct SHEET *sheet, unsigned int memtotal);

/**
 *	@description	向控制台中输入一个符号
 *	@param			cons：指定控制台
 *					chr：欲输入的字符
 *					move：是否后移光标			
 */
void cons_putchar(struct CONSOLE *cons, int chr, char move);

/**
 *	@description	新建一行，换行
 *	@param			cons：需要换行的控制台
 */
void cons_newline(struct CONSOLE *cons);

/**
 *	@description	向控制台内写入字符串
 *	@param			cons：指定控制台
 *					s：欲显示的字符串
 */
void cons_putstr0(struct CONSOLE *cons, char *s);

/**
 *	@description	指定长度想控制台内写入字符串
 *	@param			cons：指定控制台
 *					s：欲显示的字符串
 *					l：指定长度
 */
void cons_putstr1(struct CONSOLE *cons, char *s, int l);

/**
 *	@description	执行命令字符串的实际命令
 *	@param			cmdline：命令字符串
 *					cons：指定控制台
 *					fat：FAT表
 *					memtotal：内存总量
 */
void cons_runcmd(char *cmdline, struct CONSOLE *cons, int *fat, unsigned int memtotal);


/**
 *	@description	创建一个文件
 *	@param			cons：控制台
 *					cmdline；命令行
 */
void cmd_touch(struct CONSOLE *cons, char *cmdline);

/**
 *	@description	查看内存的命令
 *	@param			cons：控制台
 *					memtotal：内存可用总数
 */
void cmd_mem(struct CONSOLE *cons, unsigned int memtotal);

/**
 *	@description	执行清屏命令
 *	@param			cons：指定的控制台
 */
void cmd_clean(struct CONSOLE *cons);

/**
 *	@description	执行查看文件命令
 *	@param			cons：指定控制台
 */
void cmd_ls(struct CONSOLE *cons);

/**
 *	@description	切换路径
 *	@param			cons：制定控制台
 *					cmdline：命令行，用于解析出参数的
 *	@notice			此处的函数需要修改以支持相对路径
 */
void cmd_cd(struct CONSOLE *cons, char *cmdline);

/**
 *	@description	输出文件内容
 *	@param			cons：制定控制台
 *					fat：FAT表
 *					cmdline：命令行，用于解析出参数的
 */
void cmd_cat(struct CONSOLE *cons, char *cmdline);

/**
 *	@description	指定应用程序
 *	@param			cons：指定控制台
 *					fat：FAT表
 *					cmdline：命令行，用于解析出参数的
 *	@return			执行成功返回1，否则返回0
 */
int cmd_app(struct CONSOLE *cons, int *fat, char *cmdline);

/**
 *	@description	应用程序软中断调用的功能解析函数
 *	@param			edi,esi, ebp, esp, ebx, edx, ecx, eax：压栈参数
 *					edx：一般指定该寄存器为功能号
 */
void hrb_api(int edi, int esi, int ebp, int esp, int ebx, int edx, int ecx, int eax);

/**
 *	@description	执行创建目录文件命令
 *	@param			cons：指定控制台
 */
void cmd_mkdir(struct CONSOLE *cons);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


