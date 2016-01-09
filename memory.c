/***************************************************************************************
 *	FileName					:	memory.c
 *	CopyRight					:	1.0
 *	ModuleName					:	memory management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	�ڴ�����ʵ���ļ�
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
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000
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
 *	@description		�ڴ���Ժ���
 *	@param				start�������ڴ���ʼ��ַ
 *						end:�����ڴ������ַ
 *	@return				�������õ�ַ
 */
unsigned int memtest(unsigned int start, unsigned int end){
	char flg486 = 0;
	unsigned int eflg, cr0, i;

	eflg = io_load_eflags();
	eflg |= EFLAGS_AC_BIT; /* AC-bit = 1 */
	io_store_eflags(eflg);
	eflg = io_load_eflags();
	if ((eflg & EFLAGS_AC_BIT) != 0) {
		flg486 = 1;
	}
	eflg &= ~EFLAGS_AC_BIT; /* AC-bit = 0 */
	io_store_eflags(eflg);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	if (flg486 != 0) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}

	return i;
}

/**
 *	@description	��ʼ���ڴ����
 *	@param			man������ʼ���Ĺ����
 */
void memman_init(struct MEMMAN *man){
	man->frees = 0;	
	man->maxfrees = 0;
	man->lostsize = 0;
	man->losts = 0;
	return;
}

/**
 *	@description	�����õ�ַ��С
 *	@param			man��Ŀ���ڴ�����
 *	@return			ȫ�������ڴ��С
 */
unsigned int memman_total(struct MEMMAN *man){
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

/**
 *	@description	����һ��ָ����С���ڴ��
 *	@param			man��Ŀ���ڴ�����
 *					size��������Ĵ�С
 *					����ɹ��������뵽���׵�ַ�����򷵻�0
 *	@notice			�ô�ʹ�õ����״���Ӧ�㷨
 */
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size){
	unsigned int i, a;

	//TODO ���Ҵ�С����ָ��size�Ŀ��ÿ����ڴ��
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				//TODO ��ʣ�࣬��������
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1];
				}
			}
			return a;
		}
	}

	//TODO ����ʧ�ܷ���0
	return 0;
}

/**
 *	@description	�ͷ�ָ���ڴ�
 *	@param			man��ָ�����ڴ�����
 *					addr��ָ���ڴ���׵�ַ
 *					size��ָ���ڴ�Ĵ�С
 *	@return			�ͷųɹ�����0�����򷵻�-1
 */
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size){
	int i, j;
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].addr > addr) {
			break;
		}
	}
	/* free[i - 1].addr < addr < free[i].addr */
	if (i > 0) {
		/* �O������ */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* �O�̂����̈�ɂ܂Ƃ߂��� */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* �������� */
				if (addr + size == man->free[i].addr) {
					/* �Ȃ�ƌ��Ƃ��܂Ƃ߂��� */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]�̍폜 */
					/* free[i]���Ȃ��Ȃ����̂őO�ւ߂� */
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* �\���̂̑�� */
					}
				}
			}
			return 0; /* �����I�� */
		}
	}
	/* �O�Ƃ͂܂Ƃ߂��Ȃ����� */
	if (i < man->frees) {
		/* ��낪���� */
		if (addr + size == man->free[i].addr) {
			/* ���Ƃ͂܂Ƃ߂��� */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* �����I�� */
		}
	}
	/* �O�ɂ����ɂ��܂Ƃ߂��Ȃ� */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]�������A���ւ��炵�āA�����܂���� */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* �ő�l���X�V */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* �����I�� */
	}
	/* ���ɂ��点�Ȃ����� */
	man->losts++;
	man->lostsize += size;

	//TODO �ͷ�ʧ��
	return -1;
}

/**
 *	@description	�����СΪ4k�������ڴ�
 *	@param			man���ڴ�����
 *					size��������Ĵ�С
 *	@return			���뵽���׵�ַ������ʧ�ܷ���0
 *	@notice			�˾ٽ�Ϊ�����ڴ���Ƭ���ͷ�ʱӦʹ��memman_free_4k()���������ͷ�
 */
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size){
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

/**
 *	@description	�ͷ�ͨ��memman_alloc_4k()�������뵽���ڴ�
 *	@param			man���ڴ�����
 *					addr�����ͷŵ��ڴ���׵�ַ
 *					size�����ͷŵ��ڴ�Ĵ�С
 *	@return			�ͷųɹ�����0�����򷵻�-1
 */
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size){
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
