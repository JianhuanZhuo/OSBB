/***************************************************************************************
 *	File Name				:		dsctbl.h
 *	CopyRight				:		1.0
 *	ModuleName				:		descriptor management module
 *
 *	Create Data				:		2016/01/06
 *	Author/Corportation		:		ZhuoJianhuan
 *
 *	Abstract Description	:		中断记录表与段号记录表管理的实现文件
 *									GDT：Global (segment) Descriptor Table 全局段号记录表
 *									IDT：Interrupt Descriptor Table 中断记录表
 *									这两个记录表存储在内存的固定位置：
 *										GDT存储于：ADR_GDT	0x00270000
 *										IDT存储于：ADR_IDT	0x0026f800
 *
 *--------------------------------Revision History--------------------------------------
 *	No	version		Data			Revised By			Item			Description
 *	1	v1.0		2016/01/06		ZhuoJianhuan						Create this file
 *
 ***************************************************************************************/
/**************************************************************
*	Multi-Include-Prevent Section
**************************************************************/
#ifndef __DSCTBL_H
#define __DSCTBL_H
/**************************************************************
*	Debug switch Section
**************************************************************/
/**************************************************************
*	Include File Section
**************************************************************/
/**************************************************************
*	Macro Define Section
**************************************************************/
#define ADR_IDT			0x0026f800		//IDT起始地址，0x0026f800~0x0026ffff
#define LIMIT_IDT		0x000007ff		//IDT大小
#define ADR_GDT			0x00270000		//GDT起始地址，0x00270000~0x0027ffff，额，只是随意分配
#define LIMIT_GDT		0x0000ffff		//GDT大小
#define ADR_BOTPAK		0x00280000		//bootpack.h
#define LIMIT_BOTPAK	0x0007ffff		//bootpack大小
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_TSS32		0x0089
#define AR_INTGATE32	0x008e
/**************************************************************
*	Struct Define Section
**************************************************************/
//段号记录表，此番分法，是为了兼容低级的PC机
struct SEGMENT_DESCRIPTOR {
	short limit_low;				//上限低十六位
	short base_low;					//段基址，低十六位
	char base_mid;					//段中址，中八个
	char access_right;				//访问权限
	char limit_high;				//上限高八个
	char base_high;					//段高址，高八个
};

//中断记录表
struct GATE_DESCRIPTOR {
	short offset_low;				//
	short selector;					//
	char dw_count;
	char access_right;
	short offset_high;
};
/**************************************************************
*	Global Variable Declare Section
**************************************************************/
/**************************************************************
*	Prototype Declare Section
**************************************************************/
/**
 *	@description	初始化中断记录表与段号记录表
 */
void init_gdtidt(void);

/**
 *	@description	设置段
 *	@param			sd：段号记录表
 *					limit：段上限
 *					base：段基址
 *					ar：属性
 */
void set_segmdesc(struct SEGMENT_DESCRIPTOR *sd, unsigned int limit, int base, int ar);

/**
 *	@description	注册中断服务函数
 *	@param			gd：中断记录表
 *					offset：偏移量
 *					selector：段选择符
 *					ar：属性
 */
void set_gatedesc(struct GATE_DESCRIPTOR *gd, int offset, int selector, int ar);
/**************************************************************
*	End-Multi-Include-Prevent Section
**************************************************************/
#endif


