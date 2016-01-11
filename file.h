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
	char reserve[10];			//保留，可以拓展
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
 *	@description	设置文件管理系统的参数
 *	@param			fatp：FAT表地址
 *					finfop：根目录文件信息表首地址
 *					imgp：文件磁盘映像地址
 *	@notice			
 */
void setupFS(int *fatp, struct FILEINFO *finfop, unsigned char *imgp);

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
 */
void file_loadfile(int clustno, int size, char *buf);

/**
 *	@description	新建一个文件
 *	@param			filename：文件名
 *					ext：拓展文件名
 *					dir：当前目录，若在根目录下则为0
 *					isFolder：0为文件，1为目录
 *	@return			0创建成功，-1同名目录，
 */
int createFile(char *filename, char *ext, struct FILEINFO *dir, int isFolder);


/**
 *	@description	将指定长度的内容追加到文件中
 *	@param			fileInfo：目标文件
 *					contp：存放追加内容的指针
 *					size：内容长度
 */
void append(struct FILEINFO * fileInfo, char *contp, int size);


/**
 *	@description	在指定文件信息表中搜索文件
 *	@param			name：欲搜索的文件名，需要是个全名
 *					finfo：文件信息表，根目录为0
 *	@return			查找失败返回0，成功返回该文件信息
 */
struct FILEINFO *fileSearch(char *name, struct FILEINFO *finfo);


/**
 *	@description	检查指定文件信息项是否为目录
 *	@param			finfo：文件信息表
 *	@return			是目录返回1，否则返回0
 */
int isFolder(struct FILEINFO *finfo);


/**
 *	@description	从指定目录中删除指定文件名的文件
 *	@param			name：文件名
 *					dir：指定目录，0为根目录
 *	@return			0：删除成功
 *					1：文件不存在
 *					2：文件为目录
 */
int deleteByName(char* name, struct FILEINFO * dir);

/**
 *	@description	清空文件
 *	@param			fileInfo：欲清空的文件，不得是根目录
 */
void cleanFile(struct FILEINFO * fileInfo);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


