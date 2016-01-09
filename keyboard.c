/***************************************************************************************
 *	FileName					:	keyboard.c
 *	CopyRight					:	1.0
 *	ModuleName					:	keyboard management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	�����������ģ�����
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
#define PORT_KEYSTA				0x0064
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
/**************************************************************
*	Struct Define Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
struct FIFO32 *keyfifo;		//���̹����Ļ�����
int keydata0;				//����ʹ�õĹؼ���
/**************************************************************
*	File Static Variable Define Section
**************************************************************/
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	��ʼ������ģ��
 *	@param			fifo���������̵Ļ���
 *					data0������ʹ�õĹؼ���
 */
void init_keyboard(struct FIFO32 *fifo, int data0){
	keyfifo = fifo;
	keydata0 = data0;
	
	//�ȴ�����׼��
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
	return;
}


/**
 *	@description	�ȴ����̾���
 */
void wait_KBC_sendready(void){

	//TODO ѭ���ȴ�����׼������
	for (;;) {
		if ((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) {
			break;
		}
	}
	return;
}

/**
 *	@description	�����жϷ�����
 */
void inthandler21(int *esp){
	int data;
	io_out8(PIC0_OCW2, 0x61);	//����жϱ�־λ
	data = io_in8(PORT_KEYDAT);
	fifo32_put(keyfifo, data + keydata0);
	return;
}

