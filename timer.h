/***************************************************************************************
 *	File Name				:		timer.h
 *	CopyRight				:		1.0
 *	ModuleName				:		PIT management module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		定时器管理模块接口文件
 *									一般使用：
 *										1. 初始化PIT				init_pit()
 *										2. 申请一个定时器			timer_alloc()
 *										3. 初始化申请到的定时器		timer_init()
 *										4. 设置定时器定时时间		timer_settime()
 *										5. 调用缓冲的接口获得数据
 *										6. 释放该定时器				timer_free()
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __TIMER_H
#define __TIMER_H
/**************************************************************
*	Debug switch Section
**************************************************************/

/**************************************************************
*	Include File Section
**************************************************************/
/**************************************************************
*	Macro Define Section
**************************************************************/
#define MAX_TIMER		500				//最大定时器数量
/**************************************************************
*	Struct Define Section
**************************************************************/
struct TIMER {
	struct TIMER *next;					//排在这个定时器后面的下一个定时器
	unsigned int timeout;				//这个定时器的超时时间
	unsigned int flags;					//定时器状态标志位
	struct FIFO32 *fifo;				//定时器的缓冲区
	int data;							//数据
};
struct TIMERCTL {
	unsigned int count;					//定时器计数
	unsigned int next;					//当前最近的超时时间
	struct TIMER *t0;					//当前最近的超时定时器，使用链表进行连接，这个是首节点
	struct TIMER timers0[MAX_TIMER];	//全部定时器结构体
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
extern struct TIMERCTL timerctl;
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@description	PIT初始化函数
 */
void init_pit(void);

/**
 *	@description	申请一个定时器
 *	@return			申请成功返回定时器指针，否则返回0
 */
struct TIMER *timer_alloc(void);

/**
 *	@description	释放一个定时器
 */
void timer_free(struct TIMER *timer);

/**
 *	@description	初始化一个定时器
 *	@param			fifo：定时器的缓冲区
 *					data：定时器的数据
 */
void timer_init(struct TIMER *timer, struct FIFO32 *fifo, int data);

/**
 *	@description	为一个定时器设置超时时间
 *	@param			timer：欲设置的定时器
 *					timeout：超时时间
 */
void timer_settime(struct TIMER *timer, unsigned int timeout);

/**
 *	@description	定时器中断服务程序
 *	@notice			该中断服务由naskfunc.nas文件中的汇编语句调用，该语句由gdtidt函数注册到中断向量表中
 */
void inthandler20(int *esp);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


