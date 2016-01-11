/***************************************************************************************
 *	File Name				:		pathStack.h
 *	CopyRight				:		1.0
 *	ModuleName				:		console path 	
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		控制台的路径逻辑函数的声明文件
 *									控制台环境
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __PATH_STACK_H
#define __PATH_STACK_H
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
struct pathEnv{
	stack_t pathStack;		//路径栈
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@description	初始化一个路径栈
 *	@param			p：欲初始化的路径栈
 *	@notice			这个函数应该在使用路径之前执行
 */
void initPathEnv(struct pathEnv *p);

/**
 *	@description	获得当前的路径
 *	@param			pEnv：路径环境
 *					path：存储路径字符串的地址
 */
void getPathCurrent(struct pathEnv *pEnv, char *path);

/**
 *	@description	返回当前的目录
 *	@param			pEnv：指定目录
 *	@return			0：根目录，其他：当前的目录
 */
struct FILEINFO* getDirCurrent(struct pathEnv *pEnv);

/**
 *	@description	切换到指定的目录
 *	@param			pEnv：环境
 *					tar：目标目录
 */
void cdToDir(struct pathEnv *pEnv, struct FILEINFO* tar);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


