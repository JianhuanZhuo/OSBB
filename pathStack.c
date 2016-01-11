/***************************************************************************************
 *	FileName					:	pathStack.c
 *	CopyRight					:	1.0
 *	ModuleName					:	console path management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	控制台的路径逻辑的实现文件
 *									提供的可以用户、路径、环境
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
static char *pathTmp;
static int pathPoint;
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	初始化一个路径栈
 *	@param			pEnv：欲初始化的路径环境
 *	@notice			这个函数应该在使用路径之前执行
 */
void initPathEnv(struct pathEnv *pEnv){
	stackOper.init(&(pEnv->pathStack));
}



STATUS visitPath(SElemType e){
	char*p;
	int i;
	for(i=0, p = ((struct FILEINFO*)e)->name; i<8&&*p!=0x00&& *p!=' '; i++,p++){
		pathTmp[pathPoint++] = *p;
	}
	pathTmp[pathPoint++] = '/';
}

/**
 *	@description	获得当前的路径
 *	@param			pEnv：路径环境
 *					path：存储路径字符串的地址
 */
void getPathCurrent(struct pathEnv *pEnv, char *path){
	int empty;
	stackOper.empty(pEnv->pathStack, &empty);
	if(empty){
		path[0] = '/';
		path[1] = 0x00;
	}else{
		pathTmp = path;
		pathPoint=0;
		pathTmp[pathPoint++] = '/';
		stackOper.traverse(pEnv->pathStack, visitPath);
		pathTmp[pathPoint++] = 0x00;
	}
}

/**
 *	@description	返回当前的目录
 *	@param			pEnv：指定目录
 *	@return			0：根目录，其他：当前的目录
 */
struct FILEINFO* getDirCurrent(struct pathEnv *pEnv){
	
	int addr = 0;
	stackOper.getTop(pEnv->pathStack, &addr);
	if(addr){
		return (struct FILEINFO*)addr;
	}else{
		return 0;
	}
}

void cdBack(struct pathEnv *pEnv){
	int empty;
	stackOper.empty(pEnv->pathStack, &empty);
	if(!empty){
		stackOper.pop(&(pEnv->pathStack), &empty);
	}
}

/**
 *	@description	切换到指定的目录
 *	@param			pEnv：环境
 *					tar：目标目录
 */
void cdToDir(struct pathEnv *pEnv, struct FILEINFO* tar){
	stackOper.push(&(pEnv->pathStack), tar);
}



