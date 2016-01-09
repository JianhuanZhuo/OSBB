/***************************************************************************************
 *	File Name				:		mtask.h
 *	CopyRight				:		1.0
 *	ModuleName				:		multi task management module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		多任务管理接口文件
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __MTASK_H
#define __MTASK_H
/**************************************************************
*	Debug switch Section
**************************************************************/

/**************************************************************
*	Include File Section
**************************************************************/
/**************************************************************
*	Macro Define Section
**************************************************************/
#define MAX_TASKS		1000	//最大任务数
#define TASK_GDT0		3		//起始GDT标号
#define MAX_TASKS_LV	100		//单个任务层可容纳任务数
#define MAX_TASKLEVELS	10		//最大任务层数
/**************************************************************
*	Struct Define Section
**************************************************************/
//任务关联的寄存器结构体
struct TSS32 {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
};

//任务结构体，用于表示一个任务
struct TASK {
	int sel;				//任务选择器
	int flags;				//任务标志位，1为休眠，2为运行中
	int level, priority;	//任务priority，用于指明该任务的时间片宽度，宽度为0.01秒*priority
	struct FIFO32 fifo;		//任务关联的缓冲
	struct TSS32 tss;		//任务关联的寄存器结构体
};

//任务等级结构体，每个任务都有一个等级，以表明任务执行的优先
struct TASKLEVEL {
	int running;								//该等级的当前运行的总数
	int now;									//该等级的正在运行的任务
	struct TASK *tasks[MAX_TASKS_LV];			//该等级的所有任务的指针
};

//多任务控制块
struct TASKCTL {
	int now_lv;									//当前任务的level（优先级）
	char lv_change;								//下次进行任务切换时，需要检查level的标志位，1为检查，0为不检查
	struct TASKLEVEL level[MAX_TASKLEVELS];		//所有的任务层level
	struct TASK tasks0[MAX_TASKS];				//所有的任务
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
extern struct TIMER *task_timer;				//多任务切换定时器
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@description	初始化
 *	@param			menman：内存地址
 */
struct TASK *task_init(struct MEMMAN *memman);

/**
 *	@description		查询当前运行的任务
 *	@return				当前运行的任务
 */
struct TASK *task_now(void);

/**
 *	@description	申请一个任务
 *	@param			申请成功返回任务指针，否则返回0
 */
struct TASK *task_alloc(void);

/**
 *	@description	以指定优先级运行任务
 *	@param			task：欲运行的任务
 *					level：运行的层
 *					priority：指定的优先级
 */
void task_run(struct TASK *task, int level, int priority);

/**
 *	@description	添加任务到任务层中，任务层由任务自身的level指定，后置该任务为运行状态
 */
void task_add(struct TASK *task);

/**
 *	@description	任务调度
 */
void task_switch(void);

/**
 *	@description	休眠任务
 *	@param			task：欲休眠的任务
 *	@notice			休眠任务自己将会引起任务切换
 */
void task_sleep(struct TASK *task);

/**
 *	@description	移除指定任务
 *	@param			task：欲移除的任务
 */
void task_remove(struct TASK *task);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


