/***************************************************************************************
 *	File Name				:		memory.h
 *	CopyRight				:		1.0
 *	ModuleName				:		memory management module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		内存管理接口文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __MEMORY_H
#define __MEMORY_H
/**************************************************************
*	Debug switch Section
**************************************************************/

/**************************************************************
*	Include File Section
**************************************************************/
/**************************************************************
*	Macro Define Section
**************************************************************/
#define MEMMAN_FREES		4090			//最大可用内存碎片
#define MEMMAN_ADDR			0x003c0000
/**************************************************************
*	Struct Define Section
**************************************************************/
//可用信息
struct FREEINFO {
	unsigned int addr;	//首地址
	unsigned int size;	//大小
};

//内存管理
struct MEMMAN {
	int frees;								//可用信息数
	int maxfrees;							//最大可用空间
	int lostsize;							//释放失败的总和
	int losts;								//释放失败次数
	struct FREEINFO free[MEMMAN_FREES];		//全部可用信息
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/

/**
 *	@description		内存测试函数
 *	@param				start：测试内存起始地址
 *						end:测试内存结束地址
 *	@return				正常可用地址
 */
unsigned int memtest(unsigned int start, unsigned int end);

/**
 *	@description	初始化内存管理
 *	@param			man：欲初始化的管理块
 */
void memman_init(struct MEMMAN *man);

/**
 *	@description	检查可用地址大小
 *	@param			man：目标内存管理块
 *	@return			全部可用内存大小
 */
unsigned int memman_total(struct MEMMAN *man);

/**
 *	@description	申请一块指定大小的内存块
 *	@param			man：目标内存管理块
 *					size：欲申请的大小
 *					申请成功返回申请到的首地址，否则返回0
 *	@notice			该处使用的是首次适应算法
 */
unsigned int memman_alloc(struct MEMMAN *man, unsigned int size);

/**
 *	@description	释放指定内存
 *	@param			man：指定的内存管理块
 *					addr：指定内存的首地址
 *					size：指定内存的大小
 *	@return			释放成功返回0，否则返回-1
 */
int memman_free(struct MEMMAN *man, unsigned int addr, unsigned int size);

/**
 *	@description	申请大小为4k倍数的内存
 *	@param			man：内存管理块
 *					size：欲申请的大小
 *	@return			申请到的首地址，申请失败返回0
 *	@notice			此举仅为减少内存碎片，释放时应使用memman_free_4k()函数进行释放
 */
unsigned int memman_alloc_4k(struct MEMMAN *man, unsigned int size);

/**
 *	@description	释放通过memman_alloc_4k()函数申请到的内存
 *	@param			man：内存管理块
 *					addr：欲释放的内存的首地址
 *					size：欲释放的内存的大小
 *	@return			释放成功返回0，否则返回-1
 */
int memman_free_4k(struct MEMMAN *man, unsigned int addr, unsigned int size);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


