/***************************************************************************************
 *	FileName					:	mtask.c
 *	CopyRight					:	1.0
 *	ModuleName					:	multi task management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	多任务管理的实现文件
 *									task->flags = 2;表示正在运行中
 *									task->flags = 1;表示正在使用中
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
/**
 *	@description	确定任务切换后的下一个执行任务
 */
static void task_switchsub(void);


/**
 *	@description	空闲进程
 */
void task_idle(void);
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
struct TASKCTL *taskctl;
struct TIMER *task_timer;
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	初始化
 *	@param			menman：内存地址
 */
struct TASK *task_init(struct MEMMAN *memman){
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;

	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));

	//TODO 初始化所有任务
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
	}

	//TODO 初始化所有优先级层
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}

	task = task_alloc();
	task->flags = 2;	//
	task->priority = 2; //0.02秒
	task->level = 0;	//最高级别
	task_add(task);
	task_switchsub();	//切换
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);

	//TODO 初始化空闲进程
	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;		//X86的栈是向下增长的，所以需要+64*1024
	idle->tss.eip = (int) &task_idle;
	idle->tss.es = 1 * 8;
	idle->tss.cs = 2 * 8;
	idle->tss.ss = 1 * 8;
	idle->tss.ds = 1 * 8;
	idle->tss.fs = 1 * 8;
	idle->tss.gs = 1 * 8;
	task_run(idle, MAX_TASKLEVELS - 1, 1);

	return task;
}


/**
 *	@description		查询当前运行的任务
 *	@return				当前运行的任务
 */
struct TASK *task_now(void){
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

/**
 *	@description	申请一个任务
 *	@param			申请成功返回任务指针，否则返回0
 */
struct TASK *task_alloc(void){
	int i;
	struct TASK *task;

	//TODO 遍历获得一个空闲的任务
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1;				//置为正在使用
			task->tss.eflags = 0x00000202;	// IF = 1;
			task->tss.eax = 0;				//初始化
			task->tss.ecx = 0;
			task->tss.edx = 0;
			task->tss.ebx = 0;
			task->tss.ebp = 0;
			task->tss.esi = 0;
			task->tss.edi = 0;
			task->tss.es = 0;
			task->tss.ds = 0;
			task->tss.fs = 0;
			task->tss.gs = 0;
			task->tss.ldtr = 0;
			task->tss.iomap = 0x40000000;

			//TODO 返回申请成功的任务指针
			return task;
		}
	}

	//TODO 申请失败
	return 0;
}

/**
 *	@description	以指定优先级运行任务
 *	@param			task：欲运行的任务
 *					level：运行的层
 *					priority：指定的优先级
 */
void task_run(struct TASK *task, int level, int priority){
	
	//TODO 任务level、priority有效性检查
	if (level < 0) {
		level = task->level;
	}
	if (priority > 0) {
		task->priority = priority;
	}

	//TODO level更改
	if (task->flags == 2 && task->level != level) {
		//这里执行后flags会变成1，所以下面的也会执行
		task_remove(task);
	}

	//TODO 以指定优先级唤醒休眠任务
	if (task->flags != 2) {
		task->level = level;
		task_add(task);
	}

	//TODO 下次进行任务切换时，需要检查level
	taskctl->lv_change = 1;
	return;
}

/**
 *	@description	添加任务到任务层中，任务层由任务自身的level指定，后置该任务为运行状态
 */
void task_add(struct TASK *task){
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2;
	return;
}

/**
 *	@description	任务调度
 */
void task_switch(void){
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	struct TASK *new_task, *now_task = tl->tasks[tl->now];
	tl->now++;
	if (tl->now == tl->running) {
		tl->now = 0;
	}
	if (taskctl->lv_change != 0) {
		task_switchsub();
		tl = &taskctl->level[taskctl->now_lv];
	}
	new_task = tl->tasks[tl->now];
	timer_settime(task_timer, new_task->priority);
	if (new_task != now_task) {
		farjmp(0, new_task->sel);
	}
	return;
}

/**
 *	@description	休眠任务
 *	@param			task：欲休眠的任务
 *	@notice			休眠任务自己将会引起任务切换
 */
void task_sleep(struct TASK *task){
	struct TASK *now_task;

	//TODO 任务有效性检查
	if (task->flags == 2) {
		now_task = task_now();
		task_remove(task);
		if (task == now_task) {
			//TODO 当前任务，则立即休眠
			task_switchsub();
			//TODO 获得跳转的目标任务，以跳转
			now_task = task_now();
			farjmp(0, now_task->sel);
		}
	}
	return;
}

/**
 *	@description	移除指定任务
 *	@param			task：欲移除的任务
 */
void task_remove(struct TASK *task){
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];

	//TODO 遍历获得任务在所在层的位置
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			//TODO 找到了就跳出
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--;
	}
	if (tl->now >= tl->running) {
		//若now值出现异常则进行修正
		tl->now = 0;
	}

	//TODO 置为休眠
	task->flags = 1;

	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;

}


/**
 *	@description	确定任务切换后的下一个执行任务
 */
static void task_switchsub(void){
	int i;

	//TODO 获得下一个执行任务
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			//TODO 找到高优先级的层中有任务，则跳出
			break;
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}


/**
 *	@description	空闲进程
 */
void task_idle(void){
	for (;;) {
		io_hlt();
	}
}