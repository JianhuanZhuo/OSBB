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
		if ((finfo[i].type & 0x18) == 0) {
			for (j = 0; j < 11; j++) {
				if (finfo[i].name[j] != s[j]) {
					goto next;
				}
			}

			//TODO �ҵ��ļ�������
			return finfo + i;
		}
next:
		i++;
	}

	//TODO ����ʧ�ܣ�����0
	return 0;
}
