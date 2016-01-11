; haribote-os boot asm
; TAB=4
; 内存分布大概
; 0x0000 0000 ~ 0x000f ffff BIOS、VRAM等		1		MB
; 0x0010 0000 ~ 0x0026 7fff 用于保存盘的内容	1440	KB
; 0x0026 8000 ~ 0x0026 f7ff 空					30		KB
; 0x0026 f800 ~ 0x0026 ffff IDT					2		KB
; 0x0027 0000 ~ 0x0027 ffff GDT					64		KB
; 0x0028 0000 ~ 0x002f ffff bootpack.hrb		512		KB
; 0x0030 0000 ~ 0x003f ffff 栈及其他			1		MB
; 0x0040 0000 ~
; 
[INSTRSET "i486p"]				; 486指令使用许可


VBEMODE	EQU		0x100			; 1024 x  768 x 8bit彩色
; VBE画面模式设置如下
;	0x100 :  640 x  400 x 8bit彩色
;	0x101 :  640 x  480 x 8bit彩色
;	0x103 :  800 x  600 x 8bit彩色
;	0x105 : 1024 x  768 x 8bit彩色
;	0x107 : 1280 x 1024 x 8bit彩色


BOTPAK	EQU		0x00280000		; bootpack偺儘乕僪愭
DSKCAC	EQU		0x00100000		; 僨傿僗僋僉儍僢僔儏偺応強
DSKCAC0	EQU		0x00008000		; 僨傿僗僋僉儍僢僔儏偺応強乮儕傾儖儌乕僪乯

; BOOT_INFO娭學
CYLS	EQU		0x0ff0			; 僽乕僩僙僋僞偑愝掕偡傞
LEDS	EQU		0x0ff1			; 
VMODE	EQU		0x0ff2			; 怓悢偵娭偡傞忣曬丅壗價僢僩僇儔乕偐丠
SCRNX	EQU		0x0ff4			; 夝憸搙偺X
SCRNY	EQU		0x0ff6			; 夝憸搙偺Y
VRAM	EQU		0x0ff8			; 僌儔僼傿僢僋僶僢僼傽偺奐巒斣抧


; 执行haribote.sys文件，该文件保存于img映像中的第一个文件
; 则是FAT12系统中的0x4200起才开始存储文件的
; 而这段img映像是被加载到0x8000的，
; 所谓haribote.sys应该是在内存中的0xc200位置
		ORG		0xc200			; 该处为磁盘映像的位置

; 确认VBE是否存在
; 如果VBE存在的话，AX会变成0x004f，并跳过执行JNE scrn320

		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; 检查VBE的版本
; 如果VBE的版本低于2.0，则无法使用高分辨率

		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320			; if (AX < 0x0200) goto scrn320

; 取得画面的模式信息
; 如果AX不等于0x004f则表示不可用

		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; 画面模式信息的确认

		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320

; 画面模式的切换

		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	; 记下画面模式
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

; 低分辨模式
scrn320:
		MOV		AL,0x13			; VGA图、320x200x8bit彩色
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; 记下画面模式
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; 使用BIOS读取键盘当前的大小写锁状态，
; 这是在未进入32位时使用BIOS的
keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; PIC关闭一切中断
;	根据AT兼容机的规格，如果要初始化PIC
;	必须在CLI之前进行，否则会挂起
;	随后进行PIC的初始化

		MOV		AL,0xff
		OUT		0x21,AL			; 禁止PIC0的主中断
		NOP						; 连续进行OUT操作，可能会出现异常
		OUT		0xa1,AL			; 禁止PIC1的主中断

		CLI						; 禁止CPU级别的中断

; 为了让CPU能访问1MB以上的内存空间，设定A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; 切换到保护模式

		LGDT	[GDTR0]			; 设立临时GDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; 设bit31为0（为了禁止颁）
		OR		EAX,0x00000001	; 设bit0 为1（为了切换到保护模式）
		MOV		CR0,EAX
		JMP		pipelineflush	; 管道机制
pipelineflush:
		MOV		AX,1*8			;  可读的段 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack的转送
		MOV		ESI,bootpack	; 转送源
		MOV		EDI,BOTPAK		; 转送目的地
		MOV		ECX,512*1024/4	; 转送长度
		CALL	memcpy			; 上面的寄存器设置是作为参数

; 磁盘数据最终传送到它本身的位置上

; 首先从启动区开始
		MOV		ESI,0x7c00		; 传送源
		MOV		EDI,DSKCAC		; 传送目的地
		MOV		ECX,512/4		; 传送长度
		CALL	memcpy			; 启动传送

; 所有剩下的

		MOV		ESI,DSKCAC0+512	; 传送源
		MOV		EDI,DSKCAC+512	; 传送目的地
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; 从柱面数变换为字节数/4
		SUB		ECX,512/4		; 减去IPL
		CALL	memcpy


; bootpack的启动

		MOV		EBX,BOTPAK
		MOV		ECX,[EBX+16]
		ADD		ECX,3			; ECX += 3;
		SHR		ECX,2			; ECX /= 4;
		JZ		skip			; 
		MOV		ESI,[EBX+20]	; 
		ADD		ESI,EBX
		MOV		EDI,[EBX+12]	; 
		CALL	memcpy
skip:
		MOV		ESP,[EBX+12]	; 栈的初始值
		JMP		DWORD 2*8:0x0000001b	; 带段号的，这里的二号就是注程序的

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; 
		RET


; 内存拷贝命令
memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; 减法结果不是0，则循环拷贝
		RET

		ALIGNB	16
GDT0:
		RESB	8				; NULL selector
		DW		0xffff,0x0000,0x9200,0x00cf	; 可读写的段(segment)32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; 可读写的段(segment)32bit(bootpack)

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
