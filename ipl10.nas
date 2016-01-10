; ipl�ļ�
; IPL initial program loader �������������
; TAB=4
; DB��		data byte		1���ֽ�
; DW��		data word		2���ֽ�
; RESB��	reserve byte	�ճ��ֽ���
; ���һ��, Ϊϵͳ������ʶ

; �˴���EQU��ORG��ռ�ô��̵Ĵ�С������������
CYLS	EQU		10				; ��������
		ORG		0x7c00			; ����װ�ص�ַ���õ�ַΪIBM/PC��׼������װ�ص�ַ��ǰ����BIOS��װ�ص�ַ

; �˴���ʼ��Ϊ���̵�������

		JMP		entry			; ��ת����
		DB		0x90			; 
		DB		"HARIBOTE"		; �����������ƣ�8���ֽڣ������Կո����
		DW		512				; ÿ�������Ĵ�С(512�ֽ�)
		DB		1				; ����صĴ�С
		DW		1				; FAT��ʼλ��
		DB		2				; FAT����
		DW		224				; ��Ŀ¼��С��224��
		DW		2880			; ���̴�С��2880����
		DB		0xf0			; ý��������
		DW		9				; FAT���ȣ�ÿ��FATռ��������
		DW		18				; ���ŵ�������
		DW		2				; ��ͷ��
		DD		0				; ����������������
		DD		2880			; ���̴�С�������������
		DB		0,0,0x29		; �̶������̵�ħ������
		DD		0xffffffff		; ������
		DB		"HARIBOTEOS "	; 11�ֽڵĴ�������
		DB		"FAT12   "		;  8�ֽڵĴ��̸�ʽ����
		RESB	18				; �ճ�18�ֽ�

; ��������
entry:
		MOV		AX,0
		MOV		SS,AX			; SS(Stack Segment)ջ�μĴ�����Ϊ0
		MOV		SP,0x7c00		; SP(Stack Point)��ջָ��Ĵ���
		MOV		DS,AX			; DS(Data Segment)���ݶμĴ�����Ϊ0

; ������

		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 
		MOV		DH,0			; 
		MOV		CL,2			; 
readloop:
		MOV		SI,0			; 
retry:
		MOV		AH,0x02			; AH=0x02 :����
		MOV		AL,1			; 1����
		MOV		BX,0
		MOV		DL,0x00			; A������
		INT		0x13			; ����BIOS�жϣ�13Ϊ���̲���
		JNC		next			; û����ʱ����תΪnext���˴�Ϊ³���Կ��ǣ����̶������ݴ�����һ����������
		ADD		SI,1			; SI++�����������
		CMP		SI,5			; SI��5�Ƚ�
		JAE		error			; SI >= 5 ����error����
		MOV		AH,0x00
		MOV		DL,0x00			; A������
		INT		0x13			; ����BIOS�жϣ�13Ϊ���̲���
		JMP		retry
next:
		MOV		AX,ES			; �ڴ����0x200��0x200��512����������
		ADD		AX,0x0020
		MOV		ES,AX			; ADD ES,0x020 
		ADD		CL,1			; CL+1
		CMP		CL,18			; CL��18
		JBE		readloop		; CL <= 18 ����readloop
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; DH < 2 ����readloop
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS ����readloop

; ִ��haribote.sys�ļ������ļ�������imgӳ���еĵ�һ���ļ�
; ����FAT12ϵͳ�е�0x4200��ſ�ʼ�洢�ļ���
; �����imgӳ���Ǳ����ص�0x8000�ģ�
; ��νharibote.sysӦ�������ڴ��е�0xc200λ��

		MOV		[0x0ff0],CH		; IPL���ǂ��܂œǂ񂾂̂�������
		JMP		0xc200			; ִ��haribote.sys�ļ������ļ�������

error:
		MOV		SI,msg			; ��msg���ĵ�ַ��Ϊ�������
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI+1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; ���ܺ�Ϊ0x0e
		MOV		BX,15			; 
		INT		0x10			; ��ʾ�ַ���BOIS����
		JMP		putloop
fin:
		HLT						; ʡ��ģʽ
		JMP		fin				; ѭ��
msg:
		DB		0x0a, 0x0a		;
		DB		"load error"
		DB		0x0a			; ����
		DB		0

		RESB	0x7dfe-$		; �ճ�����ֱ��ƫ����Ϊ510����Ϊϵͳ������ʶ
		DB		0x55, 0xaa		; ϵͳ������ʶ
