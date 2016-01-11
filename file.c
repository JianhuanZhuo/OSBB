/***************************************************************************************
 *	FileName					:	file.c
 *	CopyRight					:	1.0
 *	ModuleName					:	file management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	�ļ������ʵ���ļ�
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
 *	@descrpition	���ҿ��õĴ��̴�
 *	@return			0������ʧ�ܣ�����Ϊ���õĴغ�
 */
static int allocClustno(void);


/**
 *	@description	����ָ����Ϊָ��ֵ
 *	@param			c�������õĴ�
 *					v��ָ��ֵ
 */
static void setClustno(int c, int v);



/**
 *	@description	��ָ���ļ���д�����ֽ�
 *	@param			addr��ָ����ַ
 *					value��ֵ
 */
static void writeDisk(int addr, char value);



/**
 *	@description	��ָ���ļ���Ϣ���������ļ�������Ϊ��Ŀ¼
 *	@param			name�����������ļ�������Ҫ�Ǹ�ȫ��
 *					finfo���ļ���Ϣ������Ϊ��Ŀ¼
 *	@return			����ʧ�ܷ���0���ɹ����ظ��ļ���Ϣ
 *	@notice			�ú��������Ӳ��ҵĶ�����Ŀ¼�����ļ���
 */
struct FILEINFO *fileSearchSub(char *name, struct FILEINFO *finfo);


/**
 *	@description	��ָ���ļ���Ϣ���������ļ�
 *	@param			name�����������ļ���
 *					finfo���ļ���Ϣ��
 *					max�����������Ŀ
 *	@return			����ʧ�ܷ���0���ɹ����ظ��ļ���Ϣ
 */
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max);



/**
 *	@description	��ָ��Ŀ¼ɾ��
 *	@param			fileInfo���ļ���Ϣ��
 *					dir��Ŀ¼
 *	@notice			����һ���ӷ��������Բ���������Ч�Լ�飬��Ҫ�û����б�֤
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
 *	@description	�����ļ�����ϵͳ�Ĳ���
 *	@param			fatp��FAT���ַ
 *					finfop����Ŀ¼�ļ���Ϣ���׵�ַ
 *					imgp���ļ�����ӳ���ַ
 *	@notice			
 */
void setupFS(int *fatp, struct FILEINFO *finfop, unsigned char *imgp){
	fatList		= fatp;
	finfoList	= finfop;
	imgList		= imgp;
}

/**
 *	@description	��ӳ���ж�ȡFAT��
 *	@param			fat��FAT����Ŀ���ַ
 *					img��ӳ��
 */
void file_readfat(int *fat, unsigned char *img){
	int i, j = 0;
	for (i = 0; i < 2880; i += 2) {
		//TODO ��ѹ������ȡ
		fat[i + 0] = (img[j + 0]      | img[j + 1] << 8) & 0xfff;
		fat[i + 1] = (img[j + 1] >> 4 | img[j + 2] << 4) & 0xfff;
		j += 3;
	}
}

/**
 *	@description	��ȡ�ļ�
 *	@param			clustno����ʼ�غ�
 *					size���ļ��ܴ�С
 *					buf���ļ���ȡ�󱣴��ַ
 *					fat��FAT��
 *					img������ӳ��
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
 *	@description	��ָ���ļ���Ϣ���������ļ�
 *	@param			name�����������ļ���
 *					finfo���ļ���Ϣ��
 *					max�����������Ŀ
 *	@return			����ʧ�ܷ���0���ɹ����ظ��ļ���Ϣ
 */
struct FILEINFO *file_search(char *name, struct FILEINFO *finfo, int max){
	int i, j;
	char s[12];

	//TODO ��ʼ�����sΪȫ��
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++) {
		if (j >= 11) {
			//TODO û���ҵ����� 0
			return 0;
		}
		if (name[i] == '.' && j <= 8) {
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				//TODO ��Сд��ĸת��Ϊ��д��ĸ
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
		//TODO �ҵ��ļ�������
		return finfo + i;
next:
		i++;
	}

	//TODO ����ʧ�ܣ�����0
	return 0;
}


/**
 *	@description	���ָ���ļ���Ϣ���Ƿ�ΪĿ¼
 *	@param			finfo���ļ���Ϣ��
 *	@return			��Ŀ¼����1�����򷵻�0
 */
int isFolder(struct FILEINFO *finfo){
	if(finfo->type & 0x10){
		return 1;
	}else{
		return 0;
	}
}


/**
 *	@description	��ָ���ļ���Ϣ���������ļ�
 *	@param			name�����������ļ�������Ҫ�Ǹ�ȫ��
 *					finfo���ļ���Ϣ����Ŀ¼Ϊ0
 *	@return			����ʧ�ܷ���0���ɹ����ظ��ļ���Ϣ
 */
struct FILEINFO *fileSearch(char *name, struct FILEINFO *finfo){
	if(finfo){
		
		return fileSearchSub(name, finfo);
	}else{
		return file_search(name, finfoList, MAX_INFO_ITEN);
	}
}

/**
 *	@description	��ָ���ļ���Ϣ���������ļ�������Ϊ��Ŀ¼
 *	@param			name�����������ļ�������Ҫ�Ǹ�ȫ��
 *					finfo���ļ���Ϣ������Ϊ��Ŀ¼
 *	@return			����ʧ�ܷ���0���ɹ����ظ��ļ���Ϣ
 *	@notice			�ú��������Ӳ��ҵĶ�����Ŀ¼�����ļ���
 */
struct FILEINFO *fileSearchSub(char *name, struct FILEINFO *finfo){
	int i, j;
	char s[12];	//�淶���ļ����洢��
	int size;
	int clustno;
	struct FILEINFO * infoTemp;

	//TODO ����ļ���Ϣ�����Ч��
	if((finfo==0) || !isFolder(finfo)){
		return 0;
	}

	//TODO �淶�������ļ���
	for (j = 0; j < 11; j++) {
		s[j] = ' ';
	}
	j = 0;
	for (i = 0; name[i] != 0; i++) {
		if (j >= 11) {
			//TODO û���ҵ����� 0
			return 0;
		}
		if (name[i] == '.' && j <= 8) {
			j = 8;
		} else {
			s[j] = name[i];
			if ('a' <= s[j] && s[j] <= 'z') {
				//TODO ��Сд��ĸת��Ϊ��д��ĸ
				s[j] -= 0x20;
			} 
			j++;
		}
	}

	size = finfo->size;
	clustno = finfo->clustno;
	while(1) {
		//TODO ����Ƿ�Ϊ���һ���أ���Ҫ����
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

			//TODO ������Ҫע�⣬��Ҫ��Ҫ����
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

	//TODO ����ʧ�ܣ�����0
	return 0;
}

/**
 *	@description	��ʼ��һ���ļ���Ϣ��
 *	@param			file���ļ���Ϣ��
 *					isFolder��1��Ϊ��ʼ��ΪĿ¼��0Ϊ��ͨ�ļ�
 */
void initFileItem(struct FILEINFO * file, char isFolder){
	
	int i;
	for(i=0; i<8; i++){
		file->name[i] = ' ';
	}
	for(i=0; i<3; i++){
		file->ext[i] = ' ';
	}
	file->type		= 0;		//��ͨ�ļ�
	file->time		= 0;
	file->date		= 0;
	file->clustno	= 0xff;
	file->size		= 0;

	//TODO Ŀ¼��־
	if(isFolder){
		file->type |= 0x10;
	}
}

/**
 *	@description	�½�һ���ļ�
 *	@param			filename���ļ���
 *					ext����չ�ļ���
 *					dir����ǰĿ¼�����ڸ�Ŀ¼����Ϊ0
 *					isFolder��0Ϊ�ļ���1ΪĿ¼
 *	@return			0�����ɹ���-1ͬ��Ŀ¼��
 */
int createFile(char *filename, char *ext, struct FILEINFO *dir, int f){
	
	struct FILEINFO fileInfo;
	int i;
	char strTemp[11];

	initFileItem(&fileInfo, f);
	//TODO �ļ�������չ����ֵ
	for(i=0; i<8; i++){
		if((filename[i]!=' ') && (filename[i]!=0x00)){
			if ('a' <= filename[i] && filename[i] <= 'z') {
				//TODO ��Сд��ĸת��Ϊ��д��ĸ
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
				//TODO ��Сд��ĸת��Ϊ��д��ĸ
				ext[i] -= 0x20;
			} 
			fileInfo.ext[i] = ext[i];
		}else{
			break;
		}
	}

	//TODO ����Ƿ�Ϊ��Ŀ¼
	if(dir == 0){
		//TODO �Ǹ�Ŀ¼��ֱ����ӵ���Ŀ¼��
		//TODO ͬ�����
		sprintf(strTemp, "%s.%s", filename, ext);
		if(file_search(strTemp, finfoList, MAX_INFO_ITEN) != 0){
			return -1;
		}
		//TODO ����Ŀ¼β��
		int j;
		for (j = 0; j < MAX_INFO_ITEN; j++) {
			if (finfoList[j].name[0] == 0x00) {
				break;
			}
		}

		

		//TODO ʱ�伱���ﲻ�������飬
		for(i=0; i<8; i++){
			finfoList[j].name[i] = fileInfo.name[i];
		}
		
		for(i=0; i<3; i++){
			finfoList[j].ext[i] = fileInfo.ext[i];
		}
		
		finfoList[j].type		= fileInfo.type;		//��ͨ�ļ�
		finfoList[j].time		= fileInfo.time;
		finfoList[j].date		= fileInfo.date;
		finfoList[j].clustno	= fileInfo.clustno;
		finfoList[j].size		= fileInfo.size;
	}else{ 
		int res = isFolder(dir);
		if(res){
			//TODO ����Ŀ¼
			
			//TODO ͬ�����
			sprintf(strTemp, "%s.%s", filename, ext);
			if(fileSearch(strTemp, dir) != 0){
				return -1;
			}
			//TODO ׷��
			sprintf(debugStr, "%s", "zuijia le ");
			debug(debugLine++, debugStr);
			append((char *)dir, (char *)(&fileInfo), sizeof(struct FILEINFO));
		}else{
			//TODO dir��һ���ļ�������
			return 1;
		}
	}

	return 0;
}


/**
 *	@description	��ָ�����ȵ�����׷�ӵ��ļ���
 *	@param			fileInfo��Ŀ���ļ�
 *					contp�����׷�����ݵ�ָ��
 *					size�����ݳ���
 *	@notice			�����ʱ�򣬰��������˽�л�
 */
void append(struct FILEINFO * fileInfo, char *contp, int size){
	
	int clustno;
	int startAddr ;
	int leftSize;
	//TODO ����ǿ��ļ���
	if(fileInfo->clustno >= 0xfe){
		//TODO ����һ���أ�����Ϊ�״�
		fileInfo->clustno = allocClustno();
	}
	
	clustno = fileInfo->clustno;

	//TODO ������һ����
	while(fatList[clustno] <=0xfe){
		clustno = fatList[clustno];
	}

	startAddr	= clustno*CLUSTNO_SIZE+fileInfo->size%CLUSTNO_SIZE;
	leftSize	= CLUSTNO_SIZE - fileInfo->size%CLUSTNO_SIZE;
	fileInfo->size += size;	//�˴���size����ĵط�������
	//TODO ��ʣ��ռ乻��
	if(leftSize >= size){
		for(; size>0; size--){
			//sprintf(debugStr, "startAddr %d, value %d", startAddr, *contp);
			//debug(debugLine++, debugStr);
			writeDisk(startAddr++, *contp++);	//д������
		}
	}else{
		//TODO ����ʣ��ռ�����
		for(; leftSize>0; leftSize--, size--){
			writeDisk(startAddr++, *contp++);	//д������
		}
		//TODO ����ռ������
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
				writeDisk(startAddr++, *contp++);	//д������
			}
		}//end of while(size>0)
	}
}

/**
 *	@descrpition	���ҿ��õĴ��̴�
 *	@return			0������ʧ�ܣ�����Ϊ���õĴغ�
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
 *	@description	����ָ����Ϊָ��ֵ
 *	@param			c�������õĴ�
 *					v��ָ��ֵ
 */
static void setClustno(int c, int v){
	fatList[c]=v;
}

/**
 *	@description	��ָ���ļ���д�����ֽ�
 *	@param			addr��ָ����ַ
 *					value��ֵ
 */
static void writeDisk(int addr, char value){
	imgList[addr] = value;
}


/**
 *	@description	��ָ��Ŀ¼��ɾ��ָ���ļ������ļ�
 *	@param			name���ļ���
 *					dir��ָ��Ŀ¼
 *	@return			0��ɾ���ɹ�
 *					1���ļ�������
 *					2���ļ�ΪĿ¼
 */
int deleteByName(char* name, struct FILEINFO * dir){
	
	struct FILEINFO *fileInfo;
	
	//TODO �����ļ�
	fileInfo = fileSearch(name, dir);
	if(!fileInfo){
		return 1;
	}

	//TODO �ļ��Ƿ�ΪĿ¼
	if(isFolder(fileInfo)){
		return 2;
	}

	if(dir){
		deleteByClu(fileInfo, dir);
	}else{
		//TODO ����ļ�
		cleanFile(fileInfo);
		//TODO ��ǰ����
		while(fileInfo->name[0]){
			memcpy(fileInfo, fileInfo+1, sizeof(struct FILEINFO));
			fileInfo++;
		}
	}

	return 0;
}

/**
 *	@description	��ָ��Ŀ¼ɾ��
 *	@param			fileInfo���ļ���Ϣ��
 *					dir��Ŀ¼�������Ǹ�Ŀ¼
 *	@notice			����һ���ӷ��������Բ���������Ч�Լ�飬��Ҫ�û����б�֤
 */
static void deleteByClu(struct FILEINFO * fileInfo, struct FILEINFO * dir){

	struct FILEINFO *buf;
	struct FILEINFO *p;
	int i, j;
	//TODO ��մ�
	cleanFile(fileInfo);

	//TODO ɾ��Ŀ¼�µļ�¼
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

		//TODO ��պ���½�ȥ
		cleanFile(dir);
		append(dir, (char *)buf, dir->size-32);
		mfree((char *)buf, dir->size);
		dir->size-=32;
	}
}


/**
 *	@description	����ļ�
 *	@param			fileInfo������յ��ļ��������Ǹ�Ŀ¼
 */
void cleanFile(struct FILEINFO * fileInfo){
	
	int p;
	while(fileInfo->clustno != 0xff){
		p =fileInfo->clustno;
		fileInfo->clustno = fatList[fileInfo->clustno];
		fatList[p] = 0xff;
	}
}


