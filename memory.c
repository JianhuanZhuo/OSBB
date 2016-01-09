/***************************************************************************************
 *	FileName					:	memory.c
 *	CopyRight					:	1.0
 *	ModuleName					:	memory management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	内存管理的实现文件
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
 *	@description		内存测试函数
 *	@param				start：测试内存起始地址
 *						end:测试内存结束地址
 *	@return				正常可用地址
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
 *	@description	初始化内存管理
 *	@param			man：欲初始化的管理块
 */
void memman_init(struct MEMMAN *man){
	man->frees = 0;	
	man->maxfrees = 0;
	man->lostsize = 0;
	man->losts = 0;
	return;
}

/**
 *	@description	检查可用地址大小
 *	@param			man：目标内存管理块
 *	@return			全部可用内存大小
 */
unsigned int memman_total(struct MEMMAN *man){
	unsigned int i, t = 0;
	for (i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

/**
 *	@description	申请一块指定大小的内存块
 *	@param			man：目标内存管理块
 *					size：欲申请的大小
 *					申请成功返回申请到的首地址，否则返回0
 *	@notice			该处使用的是首次适应算法
 */
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size){
	unsigned int i, a;

	//TODO 查找大小大于指定size的可用空闲内存块
	for (i = 0; i < man->frees; i++) {
		if (man->free[i].size >= size) {
			a = man->free[i].addr;
			man->free[i].addr += size;
			man->free[i].size -= size;
			if (man->free[i].size == 0) {
				//TODO 无剩余，则进行清空
				man->frees--;
				for (; i < man->frees; i++) {
					man->free[i] = man->free[i + 1];
				}
			}
			return a;
		}
	}

	//TODO 申请失败返回0
	return 0;
}

/**
 *	@description	释放指定内存
 *	@param			man：指定的内存管理块
 *					addr：指定内存的首地址
 *					size：指定内存的大小
 *	@return			释放成功返回0，否则返回-1
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
		/* 慜偑偁傞 */
		if (man->free[i - 1].addr + man->free[i - 1].size == addr) {
			/* 慜偺偁偒椞堟偵傑偲傔傜傟傞 */
			man->free[i - 1].size += size;
			if (i < man->frees) {
				/* 屻傠傕偁傞 */
				if (addr + size == man->free[i].addr) {
					/* 側傫偲屻傠偲傕傑偲傔傜傟傞 */
					man->free[i - 1].size += man->free[i].size;
					/* man->free[i]偺嶍彍 */
					/* free[i]偑側偔側偭偨偺偱慜傊偮傔傞 */
					man->frees--;
					for (; i < man->frees; i++) {
						man->free[i] = man->free[i + 1]; /* 峔憿懱偺戙擖 */
					}
				}
			}
			return 0; /* 惉岟廔椆 */
		}
	}
	/* 慜偲偼傑偲傔傜傟側偐偭偨 */
	if (i < man->frees) {
		/* 屻傠偑偁傞 */
		if (addr + size == man->free[i].addr) {
			/* 屻傠偲偼傑偲傔傜傟傞 */
			man->free[i].addr = addr;
			man->free[i].size += size;
			return 0; /* 惉岟廔椆 */
		}
	}
	/* 慜偵傕屻傠偵傕傑偲傔傜傟側偄 */
	if (man->frees < MEMMAN_FREES) {
		/* free[i]傛傝屻傠傪丄屻傠傊偢傜偟偰丄偡偒傑傪嶌傞 */
		for (j = man->frees; j > i; j--) {
			man->free[j] = man->free[j - 1];
		}
		man->frees++;
		if (man->maxfrees < man->frees) {
			man->maxfrees = man->frees; /* 嵟戝抣傪峏怴 */
		}
		man->free[i].addr = addr;
		man->free[i].size = size;
		return 0; /* 惉岟廔椆 */
	}
	/* 屻傠偵偢傜偣側偐偭偨 */
	man->losts++;
	man->lostsize += size;

	//TODO 释放失败
	return -1;
}

/**
 *	@description	申请大小为4k倍数的内存
 *	@param			man：内存管理块
 *					size：欲申请的大小
 *	@return			申请到的首地址，申请失败返回0
 *	@notice			此举仅为减少内存碎片，释放时应使用memman_free_4k()函数进行释放
 */
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size){
	unsigned int a;
	size = (size + 0xfff) & 0xfffff000;
	a = memman_alloc(man, size);
	return a;
}

/**
 *	@description	释放通过memman_alloc_4k()函数申请到的内存
 *	@param			man：内存管理块
 *					addr：欲释放的内存的首地址
 *					size：欲释放的内存的大小
 *	@return			释放成功返回0，否则返回-1
 */
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size){
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = memman_free(man, addr, size);
	return i;
}
