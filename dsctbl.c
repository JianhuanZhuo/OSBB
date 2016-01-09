/***************************************************************************************
 *	FileName					:	dsctbl.c
 *	CopyRight					:	1.0
 *	ModuleName					:	descriptor management module
 *
 *	Create Data					:	2016/01/06
 *	Author/Corportation			:	ZhuoJianhuan
 *
 *	Abstract Description		:	中断记录表与段号记录表管理的实现文件
 *									GDT：Global (segment) Descriptor Table 全局段号记录表
 *									IDT：Interrupt Descriptor Table 中断记录表
 *									这两个记录表存储在内存的固定位置：
 *										GDT存储于：ADR_GDT	0x00270000
 *										IDT存储于：ADR_IDT	0x0026f800
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
 *	@description	初始化中断记录表与段号记录表
 */
void init_gdtidt(void){
	struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;
	struct GATE_DESCRIPTOR    *idt = (struct GATE_DESCRIPTOR    *) ADR_IDT;
	int i;

	//TODO 初始化GDT
	for (i = 0; i <= LIMIT_GDT / 8; i++) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff,   0x00000000, AR_DATA32_RW);	//全部内存
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);	//给bootpack，也就是主程序
	
	//TODO 将段上限(LIMIT_GDT)+(ADR_GDT)赋值为48位寄存器LGDT，用于表明GDT的起始地址
	load_gdtr(LIMIT_GDT, ADR_GDT);

	//TODO 初始化IDT
	for (i = 0; i <= LIMIT_IDT / 8; i++) {
		set_gatedesc(idt + i, 0, 0, 0);
	}

	//TODO 将段上限(LIMIT_IDT)+(ADR_IDT)赋值为48位寄存器LIDT，用于表明IDT的起始地址
	load_idtr(LIMIT_IDT, ADR_IDT);

	//TODO 注册中断服务函数
	set_gatedesc(idt + 0x20, (int) asm_inthandler20, 2 * 8, AR_INTGATE32);	//定时器中断
	set_gatedesc(idt + 0x21, (int) asm_inthandler21, 2 * 8, AR_INTGATE32);	//键盘中断
	set_gatedesc(idt + 0x27, (int) asm_inthandler27, 2 * 8, AR_INTGATE32);	//
	set_gatedesc(idt + 0x2c, (int) asm_inthandler2c, 2 * 8, AR_INTGATE32);	//鼠标中断
	set_gatedesc(idt + 0x40, (int) asm_hrb_api,      2 * 8, AR_INTGATE32);	//API软中断服务函数

	return;
}

/**
 *	@description	设置段
 *	@param			sd：段号记录表
 *					limit：段上限
 *					base：段基址
 *					ar：属性
 */
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar){
	if (limit > 0xfffff) {
		ar |= 0x8000; /* G_bit = 1 */
		limit /= 0x1000;
	}
	sd->limit_low    = limit & 0xffff;
	sd->base_low     = base & 0xffff;
	sd->base_mid     = (base >> 16) & 0xff;
	sd->access_right = ar & 0xff;	//低八位为访问权限
	sd->limit_high   = ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high    = (base >> 24) & 0xff;
	return;
}

/**
 *	@description	注册中断服务函数
 *	@param			gd：中断记录表
 *					offset：偏移量
 *					selector：段选择符
 *					ar：属性
 */
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar){
	gd->offset_low   = offset & 0xffff;
	gd->selector     = selector;
	gd->dw_count     = (ar >> 8) & 0xff;
	gd->access_right = ar & 0xff;
	gd->offset_high  = (offset >> 16) & 0xffff;
	return;
}
