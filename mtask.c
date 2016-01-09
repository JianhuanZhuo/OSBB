/***************************************************************************************
 *	FileName					:	mtask.c
 *	CopyRight					:	1.0
 *	ModuleName					:	multi task management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	����������ʵ���ļ�
 *									task->flags = 2;��ʾ����������
 *									task->flags = 1;��ʾ����ʹ����
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
 *	@description	ȷ�������л������һ��ִ������
 */
static void task_switchsub(void);


/**
 *	@description	���н���
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
 *	@description	��ʼ��
 *	@param			menman���ڴ��ַ
 */
struct TASK *task_init(struct MEMMAN *memman){
	int i;
	struct TASK *task, *idle;
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;

	taskctl = (struct TASKCTL *) memman_alloc_4k(memman, sizeof (struct TASKCTL));

	//TODO ��ʼ����������
	for (i = 0; i < MAX_TASKS; i++) {
		taskctl->tasks0[i].flags = 0;
		taskctl->tasks0[i].sel = (TASK_GDT0 + i) * 8;
		set_segmdesc(gdt + TASK_GDT0 + i, 103, (int) &taskctl->tasks0[i].tss, AR_TSS32);
	}

	//TODO ��ʼ���������ȼ���
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		taskctl->level[i].running = 0;
		taskctl->level[i].now = 0;
	}

	task = task_alloc();
	task->flags = 2;	//
	task->priority = 2; //0.02��
	task->level = 0;	//��߼���
	task_add(task);
	task_switchsub();	//�л�
	load_tr(task->sel);
	task_timer = timer_alloc();
	timer_settime(task_timer, task->priority);

	//TODO ��ʼ�����н���
	idle = task_alloc();
	idle->tss.esp = memman_alloc_4k(memman, 64 * 1024) + 64 * 1024;		//X86��ջ�����������ģ�������Ҫ+64*1024
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
 *	@description		��ѯ��ǰ���е�����
 *	@return				��ǰ���е�����
 */
struct TASK *task_now(void){
	struct TASKLEVEL *tl = &taskctl->level[taskctl->now_lv];
	return tl->tasks[tl->now];
}

/**
 *	@description	����һ������
 *	@param			����ɹ���������ָ�룬���򷵻�0
 */
struct TASK *task_alloc(void){
	int i;
	struct TASK *task;

	//TODO �������һ�����е�����
	for (i = 0; i < MAX_TASKS; i++) {
		if (taskctl->tasks0[i].flags == 0) {
			task = &taskctl->tasks0[i];
			task->flags = 1;				//��Ϊ����ʹ��
			task->tss.eflags = 0x00000202;	// IF = 1;
			task->tss.eax = 0;				//��ʼ��
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

			//TODO ��������ɹ�������ָ��
			return task;
		}
	}

	//TODO ����ʧ��
	return 0;
}

/**
 *	@description	��ָ�����ȼ���������
 *	@param			task�������е�����
 *					level�����еĲ�
 *					priority��ָ�������ȼ�
 */
void task_run(struct TASK *task, int level, int priority){
	
	//TODO ����level��priority��Ч�Լ��
	if (level < 0) {
		level = task->level;
	}
	if (priority > 0) {
		task->priority = priority;
	}

	//TODO level����
	if (task->flags == 2 && task->level != level) {
		//����ִ�к�flags����1�����������Ҳ��ִ��
		task_remove(task);
	}

	//TODO ��ָ�����ȼ�������������
	if (task->flags != 2) {
		task->level = level;
		task_add(task);
	}

	//TODO �´ν��������л�ʱ����Ҫ���level
	taskctl->lv_change = 1;
	return;
}

/**
 *	@description	�������������У�����������������levelָ�������ø�����Ϊ����״̬
 */
void task_add(struct TASK *task){
	struct TASKLEVEL *tl = &taskctl->level[task->level];
	tl->tasks[tl->running] = task;
	tl->running++;
	task->flags = 2;
	return;
}

/**
 *	@description	�������
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
 *	@description	��������
 *	@param			task�������ߵ�����
 *	@notice			���������Լ��������������л�
 */
void task_sleep(struct TASK *task){
	struct TASK *now_task;

	//TODO ������Ч�Լ��
	if (task->flags == 2) {
		now_task = task_now();
		task_remove(task);
		if (task == now_task) {
			//TODO ��ǰ��������������
			task_switchsub();
			//TODO �����ת��Ŀ����������ת
			now_task = task_now();
			farjmp(0, now_task->sel);
		}
	}
	return;
}

/**
 *	@description	�Ƴ�ָ������
 *	@param			task�����Ƴ�������
 */
void task_remove(struct TASK *task){
	int i;
	struct TASKLEVEL *tl = &taskctl->level[task->level];

	//TODO ����������������ڲ��λ��
	for (i = 0; i < tl->running; i++) {
		if (tl->tasks[i] == task) {
			//TODO �ҵ��˾�����
			break;
		}
	}

	tl->running--;
	if (i < tl->now) {
		tl->now--;
	}
	if (tl->now >= tl->running) {
		//��nowֵ�����쳣���������
		tl->now = 0;
	}

	//TODO ��Ϊ����
	task->flags = 1;

	for (; i < tl->running; i++) {
		tl->tasks[i] = tl->tasks[i + 1];
	}

	return;

}


/**
 *	@description	ȷ�������л������һ��ִ������
 */
static void task_switchsub(void){
	int i;

	//TODO �����һ��ִ������
	for (i = 0; i < MAX_TASKLEVELS; i++) {
		if (taskctl->level[i].running > 0) {
			//TODO �ҵ������ȼ��Ĳ���������������
			break;
		}
	}
	taskctl->now_lv = i;
	taskctl->lv_change = 0;
	return;
}


/**
 *	@description	���н���
 */
void task_idle(void){
	for (;;) {
		io_hlt();
	}
}