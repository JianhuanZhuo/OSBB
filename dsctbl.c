/***************************************************************************************
 *	FileName					:	dsctbl.c
 *	CopyRight					:	1.0
 *	ModuleName					:	descriptor management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	�жϼ�¼����κż�¼������ʵ���ļ�
 *									GDT��Global (segment) Descriptor Table ȫ�ֶκż�¼��
 *									IDT��Interrupt Descriptor Table �жϼ�¼��
 *									��������¼��洢���ڴ�Ĺ̶�λ�ã�
 *										GDT�洢�ڣ�ADR_GDT	0x00270000
 *										IDT�洢�ڣ�ADR_IDT	0x0026f800
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
/**************************************************************
*	Function Define Section
**************************************************************/
/**
 *	@description	��ʼ���жϼ�¼����κż�¼��
 */
void init_gdtidt(void){
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;
	int i;

	//TODO ��ʼ��GDT
	for (i = 0; i <= LIMIT_GDT / 8; i++) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);	//ȫ���ڴ�
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);	//��bootpack��Ҳ����������
	
	//TODO ��������(LIMIT_GDT)+(ADR_GDT)��ֵΪ48λ�Ĵ���LGDT�����ڱ���GDT����ʼ��ַ
	load_gdtr(LIMIT_GDT, ADR_GDT);

	//TODO ��ʼ��IDT
	for (i = 0; i <= LIMIT_IDT / 8; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}

	//TODO ��������(LIMIT_IDT)+(ADR_IDT)��ֵΪ48λ�Ĵ���LIDT�����ڱ���IDT����ʼ��ַ
	load_idtr(LIMIT_IDT, ADR_IDT);

	//TODO ע���жϷ�����
	set_gatedesc(idt + 0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);	//��ʱ���ж�
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);	//�����ж�
	set_gatedesc(idt + 0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);	//
	set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);	//����ж�
	set_gatedesc(idt + 0x40, (int) asm_hrb_api,      2 * 8, AR_INTGATE32);	//API���жϷ�����

	return;
}

/**
 *	@description	���ö�
 *	@param			sd���κż�¼��
 *					limit��������
 *					base���λ�ַ
 *					ar������
 */
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;	//�Ͱ�λΪ����Ȩ��
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

/**
 *	@description	ע���жϷ�����
 *	@param			gd���жϼ�¼��
 *					offset��ƫ����
 *					selector����ѡ���
 *					ar������
 */
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
