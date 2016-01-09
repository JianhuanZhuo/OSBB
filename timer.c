/***************************************************************************************
 *	FileName					:	timer.c
 *	CopyRight					:	1.0
 *	ModuleName					:	PIT management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	PIT�ɱ�̶�ʱ����ʵ���ļ�
 *									����ʹ�õĶ�ʱ��оƬ��8254оƬ
 *									оƬ���Ͼ���ɲο���http://www.tyut.edu.cn/kecheng1/2008/site04/courseware/chapter7/7.3.2.htm
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
#define PIT_CTRL	0x0043			//����״̬�ֵ�ַ
#define PIT_CNT0	0x0040			//��������д�����ֵ�ַ
#define TIMER_FLAGS_ALLOC		1	//������״̬
#define TIMER_FLAGS_USING		2	//��ʱ��������
/**************************************************************
*	Struct Define Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
struct TIMERCTL timerctl;
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description		��ʼ��PIT�ɱ�̶�ʱ��
 */
void init_pit(void){
	int i;
	struct TIMER *t;
	io_out8(PIT_CTRL, 0x34);		//������0���ȵͺ�ߣ�������ʽ1�������Ƽ���
	io_out8(PIT_CNT0, 0x9c);		//0x2e9c == 11932��
	io_out8(PIT_CNT0, 0x2e);		//�ж�Ƶ��ԼΪ100Hz
	
	//TODO ��ʼ����ʱ�����ƿ�
	timerctl.count = 0;
	for (i = 0; i < MAX_TIMER; i++) {
		timerctl.timers0[i].flags = 0;
	}

	//TODO ��ʼ���ڱ���λ
	t = timer_alloc();//�ڱ���ʱ��
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;
	timerctl.t0 = t;
	timerctl.next = 0xffffffff;

	return;
}

/**
 *	@description	����һ����ʱ��
 *	@return			����ɹ����ض�ʱ��ָ�룬���򷵻�0
 */
struct TIMER *timer_alloc(void){
	int i;
	for (i = 0; i < MAX_TIMER; i++) {
		if (timerctl.timers0[i].flags == 0) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	//TODO ����ʧ�ܷ���0
	return 0;
}

/**
 *	@description	�ͷ�һ����ʱ��
 */
void timer_free(struct TIMER *timer){
	timer->flags = 0;
	return;
}

/**
 *	@description	��ʼ��һ����ʱ��
 *	@param			fifo����ʱ���Ļ�����
 *					data����ʱ��������
 */
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data){
	timer->fifo = fifo;
	timer->data = data;
	return;
}

/**
 *	@description	Ϊһ����ʱ�����ó�ʱʱ��
 *	@param			timer�������õĶ�ʱ��
 *					timeout����ʱʱ��
 */
void timer_settime(struct TIMER *timer, unsigned int timeout){
	int e;
	struct TIMER *t, *s;
	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;
	e = io_load_eflags();
	io_cli();//������;�����жϣ���������
	t = timerctl.t0;
	if (timer->timeout <= t->timeout) {
		//TODO ��������ʱ���ȳ�ʱ
		timerctl.t0 = timer;
		timer->next = t;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}
	

	//TODO ��Ѱ�����λ��
	for (;;) {
		s = t;
		t = t->next;
		if (timer->timeout <= t->timeout) {
			s->next = timer;
			timer->next = t;
			io_store_eflags(e);
			return;
		}
	}
}

/**
 *	@description	��ʱ���жϷ������
 *	@notice			���жϷ�����naskfunc.nas�ļ��еĻ�������ã��������gdtidt����ע�ᵽ�ж���������
 */
void inthandler20(int *esp){
	struct TIMER *timer;
	char ts = 0;				//�������л���־

	io_out8(PIC0_OCW2, 0x60);	//����жϱ�־λ
	timerctl.count++;

	//TODO ��ʱ���
	if (timerctl.next > timerctl.count) {
		return;
	}

	//TODO ����ǰ��ĵ�ַ����timer
	timer = timerctl.t0;
	for (;;) {
		if (timer->timeout > timerctl.count) {
			break;
		}
		timer->flags = TIMER_FLAGS_ALLOC;
		if (timer != task_timer) {
			fifo32_put(timer->fifo, timer->data);
		} else {
			ts = 1;
		}
		timer = timer->next;
	}
	timerctl.t0 = timer;
	timerctl.next = timer->timeout;
	if (ts != 0) {
		//TODO ���������л�
		task_switch();
	}
	return;
}
