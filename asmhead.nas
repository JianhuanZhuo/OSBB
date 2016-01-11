; haribote-os boot asm
; TAB=4
; �ڴ�ֲ����
; 0x0000 0000 ~ 0x000f ffff BIOS��VRAM��		1		MB
; 0x0010 0000 ~ 0x0026 7fff ���ڱ����̵�����	1440	KB
; 0x0026 8000 ~ 0x0026 f7ff ��					30		KB
; 0x0026 f800 ~ 0x0026 ffff IDT					2		KB
; 0x0027 0000 ~ 0x0027 ffff GDT					64		KB
; 0x0028 0000 ~ 0x002f ffff bootpack.hrb		512		KB
; 0x0030 0000 ~ 0x003f ffff ջ������			1		MB
; 0x0040 0000 ~
; 
[INSTRSET "i486p"]				; 486ָ��ʹ�����


VBEMODE	EQU		0x100			; 1024 x  768 x 8bit��ɫ
; VBE����ģʽ��������
;	0x100 :  640 x  400 x 8bit��ɫ
;	0x101 :  640 x  480 x 8bit��ɫ
;	0x103 :  800 x  600 x 8bit��ɫ
;	0x105 : 1024 x  768 x 8bit��ɫ
;	0x107 : 1280 x 1024 x 8bit��ɫ


BOTPAK	EQU		0x00280000		; bootpack�̃��[�h��
DSKCAC	EQU		0x00100000		; �f�B�X�N�L���b�V���̏ꏊ
DSKCAC0	EQU		0x00008000		; �f�B�X�N�L���b�V���̏ꏊ�i���A�����[�h�j

; BOOT_INFO�֌W
CYLS	EQU		0x0ff0			; �u�[�g�Z�N�^���ݒ肷��
LEDS	EQU		0x0ff1			; 
VMODE	EQU		0x0ff2			; �F���Ɋւ�����B���r�b�g�J���[���H
SCRNX	EQU		0x0ff4			; �𑜓x��X
SCRNY	EQU		0x0ff6			; �𑜓x��Y
VRAM	EQU		0x0ff8			; �O���t�B�b�N�o�b�t�@�̊J�n�Ԓn


; ִ��haribote.sys�ļ������ļ�������imgӳ���еĵ�һ���ļ�
; ����FAT12ϵͳ�е�0x4200��ſ�ʼ�洢�ļ���
; �����imgӳ���Ǳ����ص�0x8000�ģ�
; ��νharibote.sysӦ�������ڴ��е�0xc200λ��
		ORG		0xc200			; �ô�Ϊ����ӳ���λ��

; ȷ��VBE�Ƿ����
; ���VBE���ڵĻ���AX����0x004f��������ִ��JNE scrn320

		MOV		AX,0x9000
		MOV		ES,AX
		MOV		DI,0
		MOV		AX,0x4f00
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; ���VBE�İ汾
; ���VBE�İ汾����2.0�����޷�ʹ�ø߷ֱ���

		MOV		AX,[ES:DI+4]
		CMP		AX,0x0200
		JB		scrn320			; if (AX < 0x0200) goto scrn320

; ȡ�û����ģʽ��Ϣ
; ���AX������0x004f���ʾ������

		MOV		CX,VBEMODE
		MOV		AX,0x4f01
		INT		0x10
		CMP		AX,0x004f
		JNE		scrn320

; ����ģʽ��Ϣ��ȷ��

		CMP		BYTE [ES:DI+0x19],8
		JNE		scrn320
		CMP		BYTE [ES:DI+0x1b],4
		JNE		scrn320
		MOV		AX,[ES:DI+0x00]
		AND		AX,0x0080
		JZ		scrn320

; ����ģʽ���л�

		MOV		BX,VBEMODE+0x4000
		MOV		AX,0x4f02
		INT		0x10
		MOV		BYTE [VMODE],8	; ���»���ģʽ
		MOV		AX,[ES:DI+0x12]
		MOV		[SCRNX],AX
		MOV		AX,[ES:DI+0x14]
		MOV		[SCRNY],AX
		MOV		EAX,[ES:DI+0x28]
		MOV		[VRAM],EAX
		JMP		keystatus

; �ͷֱ�ģʽ
scrn320:
		MOV		AL,0x13			; VGAͼ��320x200x8bit��ɫ
		MOV		AH,0x00
		INT		0x10
		MOV		BYTE [VMODE],8	; ���»���ģʽ
		MOV		WORD [SCRNX],320
		MOV		WORD [SCRNY],200
		MOV		DWORD [VRAM],0x000a0000

; ʹ��BIOS��ȡ���̵�ǰ�Ĵ�Сд��״̬��
; ������δ����32λʱʹ��BIOS��
keystatus:
		MOV		AH,0x02
		INT		0x16 			; keyboard BIOS
		MOV		[LEDS],AL

; PIC�ر�һ���ж�
;	����AT���ݻ��Ĺ�����Ҫ��ʼ��PIC
;	������CLI֮ǰ���У���������
;	������PIC�ĳ�ʼ��

		MOV		AL,0xff
		OUT		0x21,AL			; ��ֹPIC0�����ж�
		NOP						; ��������OUT���������ܻ�����쳣
		OUT		0xa1,AL			; ��ֹPIC1�����ж�

		CLI						; ��ֹCPU������ж�

; Ϊ����CPU�ܷ���1MB���ϵ��ڴ�ռ䣬�趨A20GATE

		CALL	waitkbdout
		MOV		AL,0xd1
		OUT		0x64,AL
		CALL	waitkbdout
		MOV		AL,0xdf			; enable A20
		OUT		0x60,AL
		CALL	waitkbdout

; �л�������ģʽ

		LGDT	[GDTR0]			; ������ʱGDT
		MOV		EAX,CR0
		AND		EAX,0x7fffffff	; ��bit31Ϊ0��Ϊ�˽�ֹ�䣩
		OR		EAX,0x00000001	; ��bit0 Ϊ1��Ϊ���л�������ģʽ��
		MOV		CR0,EAX
		JMP		pipelineflush	; �ܵ�����
pipelineflush:
		MOV		AX,1*8			;  �ɶ��Ķ� 32bit
		MOV		DS,AX
		MOV		ES,AX
		MOV		FS,AX
		MOV		GS,AX
		MOV		SS,AX

; bootpack��ת��
		MOV		ESI,bootpack	; ת��Դ
		MOV		EDI,BOTPAK		; ת��Ŀ�ĵ�
		MOV		ECX,512*1024/4	; ת�ͳ���
		CALL	memcpy			; ����ļĴ�����������Ϊ����

; �����������մ��͵��������λ����

; ���ȴ���������ʼ
		MOV		ESI,0x7c00		; ����Դ
		MOV		EDI,DSKCAC		; ����Ŀ�ĵ�
		MOV		ECX,512/4		; ���ͳ���
		CALL	memcpy			; ��������

; ����ʣ�µ�

		MOV		ESI,DSKCAC0+512	; ����Դ
		MOV		EDI,DSKCAC+512	; ����Ŀ�ĵ�
		MOV		ECX,0
		MOV		CL,BYTE [CYLS]
		IMUL	ECX,512*18*2/4	; ���������任Ϊ�ֽ���/4
		SUB		ECX,512/4		; ��ȥIPL
		CALL	memcpy


; bootpack������

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
		MOV		ESP,[EBX+12]	; ջ�ĳ�ʼֵ
		JMP		DWORD 2*8:0x0000001b	; ���κŵģ�����Ķ��ž���ע�����

waitkbdout:
		IN		 AL,0x64
		AND		 AL,0x02
		JNZ		waitkbdout		; 
		RET


; �ڴ濽������
memcpy:
		MOV		EAX,[ESI]
		ADD		ESI,4
		MOV		[EDI],EAX
		ADD		EDI,4
		SUB		ECX,1
		JNZ		memcpy			; �����������0����ѭ������
		RET

		ALIGNB	16
GDT0:
		RESB	8				; NULL selector
		DW		0xffff,0x0000,0x9200,0x00cf	; �ɶ�д�Ķ�(segment)32bit
		DW		0xffff,0x0000,0x9a28,0x0047	; �ɶ�д�Ķ�(segment)32bit(bootpack)

		DW		0
GDTR0:
		DW		8*3-1
		DD		GDT0

		ALIGNB	16
bootpack:
