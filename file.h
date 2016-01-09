/***************************************************************************************
 *	File Name				:		file.h
 *	CopyRight				:		1.0
 *	ModuleName				:		
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		file管理函数的声明文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __FILE_H
#define __FILE_H
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
struct FILEINFO {
	unsigned char name[8];		//文件名
	unsigned char ext[3];		//拓展名
	unsigned char type;			//类型
	char reserve[10];			//保留
	unsigned short time;		//时间
	unsigned short date;		//日期
	unsigned short clustno;		//簇号
	unsigned int size;			//大小
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@description	从映像中读取FAT表
 *	@param			fat：FAT保存目标地址
 *					img：映像
 */
void file_readfat(int *fat, unsigned char *img);

/**
 *	@description	读取文件
 *	@param			clustno：起始簇号
 *					size：文件总大小
 *					buf：文件读取后保存地址
 *					fat：FAT表
 *					img：磁盘映像
 */
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img);

/**
 *	@description	在指定文件信息表中搜索文件
 *	@param			name：欲搜索的文件名
 *					finfo：文件信息表
 *					max：查找最大条目
 *	@return			查找失败返回0，成功返回该文件信息
 */
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


