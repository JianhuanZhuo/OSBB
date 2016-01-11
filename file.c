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
#include <stdio.h>
#include <string.h>
/**************************************************************
*	Macro Define Section
**************************************************************/
#define MAX_INFO_ITEN	224
#define CLUSTNO_SIZE	0x200
/**************************************************************
*	Struct Define Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@descrpition	查找可用的磁盘簇
 *	@return			0：查找失败，其他为可用的簇号
 */
static int allocClustno(void);


/**
 *	@description	设置指定簇为指定值
 *	@param			c：欲设置的簇
 *					v：指定值
 */
static void setClustno(int c, int v);



/**
 *	@description	向指定文件区写单个字节
 *	@param			addr：指定地址
 *					value：值
 */
static void writeDisk(int addr, char value);



/**
 *	@description	在指定文件信息表中搜索文件，不能为根目录
 *	@param			name：欲搜索的文件名，需要是个全名
 *					finfo：文件信息表，不能为根目录
 *	@return			查找失败返回0，成功返回该文件信息
 *	@notice			该函数将无视查找的对象是目录还是文件夹
 */
struct FILEINFO *fileSearchSub(char *name, struct FILEINFO *finfo);


/**
 *	@description	在指定文件信息表中搜索文件
 *	@param			name：欲搜索的文件名
 *					finfo：文件信息表
 *					max：查找最大条目
 *	@return			查找失败返回0，成功返回该文件信息
 */
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);



/**
 *	@description	从指定目录删除
 *	@param			fileInfo：文件信息项
 *					dir：目录
 *	@notice			这是一个子方法，所以不做参数有效性检查，需要用户自行保证
 */
static void deleteByClu(struct FILEINFO * fileInfo, struct FILEINFO * dir);
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
int clustnoTotal = 2880;
int *fatList;
struct FILEINFO *finfoList;
unsigned char *imgList;
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	设置文件管理系统的参数
 *	@param			fatp：FAT表地址
 *					finfop：根目录文件信息表首地址
 *					imgp：文件磁盘映像地址
 *	@notice			
 */
void setupFS(int *fatp, struct FILEINFO *finfop, unsigned char *imgp){
	fatList		= fatp;
	finfoList	= finfop;
	imgList		= imgp;
}

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
void file_loadfile(int clustno, int size, char *buf){
	int i;
	for (;;) {
		if (size <= 512) {
			for (i = 0; i < size; i++) {
				buf[i] = imgList[clustno * 512 + i];
			}
			break;
		}
		for (i = 0; i < 512; i++) {
			buf[i] = imgList[clustno * 512 + i];
		}
		size -= 512;
		buf += 512;
		clustno = fatList[clustno];
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
		for (j = 0; j < 11; j++) {
			if (finfo[i].name[j] != s[j]) {
				goto next;
			}
		}
		//TODO 找到文件，返回
		return finfo + i;
next:
		i++;
	}

	//TODO 查找失败，返回0
	return 0;
}


/**
 *	@description	检查指定文件信息项是否为目录
 *	@param			finfo：文件信息表
 *	@return			是目录返回1，否则返回0
 */
int isFolder(struct FILEINFO *finfo){
	if(finfo->type & 0x10){
		return 1;
	}else{
		return 0;
	}
}


/**
 *	@description	在指定文件信息表中搜索文件
 *	@param			name：欲搜索的文件名，需要是个全名
 *					finfo：文件信息表，根目录为0
 *	@return			查找失败返回0，成功返回该文件信息
 */
struct FILEINFO *fileSearch(char *name, struct FILEINFO *finfo){
	if(finfo){
		
		return fileSearchSub(name, finfo);
	}else{
		return file_search(name, finfoList, MAX_INFO_ITEN);
	}
}

/**
 *	@description	在指定文件信息表中搜索文件，不能为根目录
 *	@param			name：欲搜索的文件名，需要是个全名
 *					finfo：文件信息表，不能为根目录
 *	@return			查找失败返回0，成功返回该文件信息
 *	@notice			该函数将无视查找的对象是目录还是文件夹
 */
struct FILEINFO *fileSearchSub(char *name, struct FILEINFO *finfo){
	int i, j;
	char s[12];	//规范化文件名存储处
	int size;
	int clustno;
	struct FILEINFO * infoTemp;

	//TODO 检查文件信息项的有效性
	if((finfo==0) || !isFolder(finfo)){
		return 0;
	}

	//TODO 规范化查找文件名
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

	size = finfo->size;
	clustno = finfo->clustno;
	while(1) {
		//TODO 检查是否为最后一个簇，需要跳出
		if (size <= 512) {
			for (i = 0; i < size; i+=32) {
				for (j = 0; j < 11; j++) {
					infoTemp = (struct FILEINFO *)&(imgList[clustno * 512 + i]);
					if (infoTemp->name[j] != s[j]) {
						break;
					}
				}
				if(11 == j){
					return infoTemp;
				}
			}

			//TODO 这里需要注意，需要需要跳出
			break;
		}
		for (i = 0; i < 512; i++) {
			for (j = 0; j < 11; j++) {
				infoTemp = (struct FILEINFO *)&(imgList[clustno * 512 + i]);
				if (infoTemp->name[j] != s[j]) {
					break;
				}
			}
			if(11 == j){
				return infoTemp;
			}
		}
		size -= 512;
		clustno = fatList[clustno];
	}

	//TODO 查找失败，返回0
	return 0;
}

/**
 *	@description	初始化一个文件信息项
 *	@param			file：文件信息项
 *					isFolder：1则为初始化为目录，0为普通文件
 */
void initFileItem(struct FILEINFO * file, char isFolder){
	
	int i;
	for(i=0; i<8; i++){
		file->name[i] = ' ';
	}
	for(i=0; i<3; i++){
		file->ext[i] = ' ';
	}
	file->type		= 0;		//普通文件
	file->time		= 0;
	file->date		= 0;
	file->clustno	= 0xff;
	file->size		= 0;

	//TODO 目录标志
	if(isFolder){
		file->type |= 0x10;
	}
}

/**
 *	@description	新建一个文件
 *	@param			filename：文件名
 *					ext：拓展文件名
 *					dir：当前目录，若在根目录下则为0
 *					isFolder：0为文件，1为目录
 *	@return			0创建成功，-1同名目录，
 */
int createFile(char *filename, char *ext, struct FILEINFO *dir, int f){
	
	struct FILEINFO fileInfo;
	int i;
	char strTemp[11];

	initFileItem(&fileInfo, f);
	//TODO 文件名与拓展名赋值
	for(i=0; i<8; i++){
		if((filename[i]!=' ') && (filename[i]!=0x00)){
			if ('a' <= filename[i] && filename[i] <= 'z') {
				//TODO 将小写字母转换为大写字母
				filename[i] -= 0x20;
			} 
			fileInfo.name[i] = filename[i];
		}else{
			break;
		}
	}
	for(i=0; i<3; i++){
		if((ext[i]!=' ') && (ext[i]!=0x00)){
			if ('a' <= ext[i] && ext[i] <= 'z') {
				//TODO 将小写字母转换为大写字母
				ext[i] -= 0x20;
			} 
			fileInfo.ext[i] = ext[i];
		}else{
			break;
		}
	}

	//TODO 检查是否为根目录
	if(dir == 0){
		//TODO 是根目录，直接添加到根目录下
		//TODO 同名检查
		sprintf(strTemp, "%s.%s", filename, ext);
		if(file_search(strTemp, finfoList, MAX_INFO_ITEN) != 0){
			return -1;
		}
		//TODO 查找目录尾部
		int j;
		for (j = 0; j < MAX_INFO_ITEN; j++) {
			if (finfoList[j].name[0] == 0x00) {
				break;
			}
		}

		

		//TODO 时间急这里不做溢出检查，
		for(i=0; i<8; i++){
			finfoList[j].name[i] = fileInfo.name[i];
		}
		
		for(i=0; i<3; i++){
			finfoList[j].ext[i] = fileInfo.ext[i];
		}
		
		finfoList[j].type		= fileInfo.type;		//普通文件
		finfoList[j].time		= fileInfo.time;
		finfoList[j].date		= fileInfo.date;
		finfoList[j].clustno	= fileInfo.clustno;
		finfoList[j].size		= fileInfo.size;
	}else{ 
		int res = isFolder(dir);
		if(res){
			//TODO 是子目录
			
			//TODO 同名检查
			sprintf(strTemp, "%s.%s", filename, ext);
			if(fileSearch(strTemp, dir) != 0){
				return -1;
			}
			//TODO 追加
			sprintf(debugStr, "%s", "zuijia le ");
			debug(debugLine++, debugStr);
			append((char *)dir, (char *)(&fileInfo), sizeof(struct FILEINFO));
		}else{
			//TODO dir是一个文件？？？
			return 1;
		}
	}

	return 0;
}


/**
 *	@description	将指定长度的内容追加到文件中
 *	@param			fileInfo：目标文件
 *					contp：存放追加内容的指针
 *					size：内容长度
 *	@notice			后面的时候，把这个函数私有化
 */
void append(struct FILEINFO * fileInfo, char *contp, int size){
	
	int clustno;
	int startAddr ;
	int leftSize;
	//TODO 如果是空文件，
	if(fileInfo->clustno >= 0xfe){
		//TODO 申请一个簇，并作为首簇
		fileInfo->clustno = allocClustno();
	}
	
	clustno = fileInfo->clustno;

	//TODO 获得最后一个簇
	while(fatList[clustno] <=0xfe){
		clustno = fatList[clustno];
	}

	startAddr	= clustno*CLUSTNO_SIZE+fileInfo->size%CLUSTNO_SIZE;
	leftSize	= CLUSTNO_SIZE - fileInfo->size%CLUSTNO_SIZE;
	fileInfo->size += size;	//此处加size；别的地方还不行
	//TODO 簇剩余空间够用
	if(leftSize >= size){
		for(; size>0; size--){
			//sprintf(debugStr, "startAddr %d, value %d", startAddr, *contp);
			//debug(debugLine++, debugStr);
			writeDisk(startAddr++, *contp++);	//写入内容
		}
	}else{
		//TODO 先用剩余空间填满
		for(; leftSize>0; leftSize--, size--){
			writeDisk(startAddr++, *contp++);	//写入内容
		}
		//TODO 申请空间继续填
		while(size>0){
			fatList[clustno] = allocClustno();
			clustno			= fatList[clustno];
			startAddr		= imgList[clustno*CLUSTNO_SIZE];
			if(size>CLUSTNO_SIZE){
				leftSize	= CLUSTNO_SIZE;
			}else{
				leftSize	= size;
			}
			for(; leftSize>0; leftSize--, size--){
				writeDisk(startAddr++, *contp++);	//写入内容
			}
		}//end of while(size>0)
	}
}

/**
 *	@descrpition	查找可用的磁盘簇
 *	@return			0：查找失败，其他为可用的簇号
 */
static int allocClustno(void){
	int p;
	for(p=0; p<clustnoTotal; p++){
		if(0x00 == fatList[p]){
			setClustno(p, 0xff);
			return p;
		}
	}
	return 0;
}

/**
 *	@description	设置指定簇为指定值
 *	@param			c：欲设置的簇
 *					v：指定值
 */
static void setClustno(int c, int v){
	fatList[c]=v;
}

/**
 *	@description	向指定文件区写单个字节
 *	@param			addr：指定地址
 *					value：值
 */
static void writeDisk(int addr, char value){
	imgList[addr] = value;
}


/**
 *	@description	从指定目录中删除指定文件名的文件
 *	@param			name：文件名
 *					dir：指定目录
 *	@return			0：删除成功
 *					1：文件不存在
 *					2：文件为目录
 */
int deleteByName(char* name, struct FILEINFO * dir){
	
	struct FILEINFO *fileInfo;
	
	//TODO 搜索文件
	fileInfo = fileSearch(name, dir);
	if(!fileInfo){
		return 1;
	}

	//TODO 文件是否为目录
	if(isFolder(fileInfo)){
		return 2;
	}

	if(dir){
		deleteByClu(fileInfo, dir);
	}else{
		//TODO 清空文件
		cleanFile(fileInfo);
		//TODO 向前覆盖
		while(fileInfo->name[0]){
			memcpy(fileInfo, fileInfo+1, sizeof(struct FILEINFO));
			fileInfo++;
		}
	}

	return 0;
}

/**
 *	@description	从指定目录删除
 *	@param			fileInfo：文件信息项
 *					dir：目录，不得是根目录
 *	@notice			这是一个子方法，所以不做参数有效性检查，需要用户自行保证
 */
static void deleteByClu(struct FILEINFO * fileInfo, struct FILEINFO * dir){

	struct FILEINFO *buf;
	struct FILEINFO *p;
	int i, j;
	//TODO 清空簇
	cleanFile(fileInfo);

	//TODO 删除目录下的记录
	if(0 != dir){
		buf		= (struct FILEINFO *)malloc(dir->size);
		file_loadfile(dir->clustno, dir->size, (char *)buf);
		for(i=0; i<dir->size; i+=32){
			for(j=0; j<11; j++){
				if(buf[i].name[j] != fileInfo->name[j]){
					break;
				}
			}
			if(j>=11){
				break;	
			}
		}
		p = (struct FILEINFO *)&(buf[i]);
		while(p->name[0]){
			memcpy(p, p+1, sizeof(struct FILEINFO));
			p++;
		}

		//TODO 清空后更新进去
		cleanFile(dir);
		append(dir, (char *)buf, dir->size-32);
		mfree((char *)buf, dir->size);
		dir->size-=32;
	}
}


/**
 *	@description	清空文件
 *	@param			fileInfo：欲清空的文件，不得是根目录
 */
void cleanFile(struct FILEINFO * fileInfo){
	
	int p;
	while(fileInfo->clustno != 0xff){
		p =fileInfo->clustno;
		fileInfo->clustno = fatList[fileInfo->clustno];
		fatList[p] = 0xff;
	}
}


