/***************************************************************************************
 *	FileName					:	file.c
 *	CopyRight					:	1.0
 *	ModuleName					:	file management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	文件管理的实现文件
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
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	从映像中读取FAT表
 *	@param			fat：FAT保存目标地址
 *					img：映像
 */
void file_readfat(int *fat, unsigned char *img){
	int i, j = 0;
	for (i = 0; i < 2880; i += 2) {
		//TODO 解压缩并读取
		fat[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
}

/**
 *	@description	读取文件
 *	@param			clustno：起始簇号
 *					size：文件总大小
 *					buf：文件读取后保存地址
 *					fat：FAT表
 *					img：磁盘映像
 */
void file_loadfile(int clustno, int size, char *buf, int *fat, char *img){
	int i;
	for (;;) {
		if (size <= 512) {
			for (i = 0; i < size; i++) {
				buf[i] = img[clustno * 512 + i];
			}
			break;
		}
		for (i = 0; i < 512; i++) {
			buf[i] = img[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fat[clustno];
	}
	return;
}

/**
 *	@description	在指定文件信息表中搜索文件
 *	@param			name：欲搜索的文件名
 *					finfo：文件信息表
 *					max：查找最大条目
 *	@return			查找失败返回0，成功返回该文件信息
 */
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max){
	int i, j;
	char s[12];

	//TODO 初始化这个s为全空
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++) {
		if (j >= 11) {
			//TODO 没有找到返回 0
			return 0;
		}
		if (name[i] == '.' && j <= 8) {
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				//TODO 将小写字母转换为大写字母
				s[j] -= 0x20;
			} 
			j++;
		}
	}
	for (i = 0; i < max; ) {
		if (finfo[i].name[0] == 0x00) {
			break;
		}
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}

			//TODO 找到文件，返回
			return finfo + i;
		}
next:
		i++;
	}

	//TODO 查找失败，返回0
	return 0;
}
