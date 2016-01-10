; ipl文件
; IPL initial program loader 启动程序加载器
; TAB=4
; DB：		data byte		1个字节
; DW：		data word		2个字节
; RESB：	reserve byte	空出字节数
; 最后一行, 为系统引导标识

; 此处的EQU与ORG不占用磁盘的大小，仅作规则定义
CYLS	EQU		10				; 常量定义
		ORG		0x7c00			; 程序装载地址，该地址为IBM/PC标准启动区装载地址，前面是BIOS的装载地址

; 此处开始作为磁盘的启动区

		JMP		entry			; 跳转命令
		DB		0x90			; 
		DB		"HARIBOTE"		; 启动区的名称，8个字节，无则以空格填空
		DW		512				; 每个扇区的大小(512字节)
		DB		1				; 定义簇的大小
		DW		1				; FAT起始位置
		DB		2				; FAT个数
		DW		224				; 根目录大小，224项
		DW		2880			; 磁盘大小，2880扇区
		DB		0xf0			; 媒体描述符
		DW		9				; FAT长度，每个FAT占用扇区数
		DW		18				; 单磁道扇区数
		DW		2				; 磁头数
		DD		0				; 分区，隐藏扇区数
		DD		2880			; 磁盘大小，冗余检验数据
		DB		0,0,0x29		; 固定，磁盘的魔法数字
		DD		0xffffffff		; 卷标号码
		DB		"HARIBOTEOS "	; 11字节的磁盘名称
		DB		"FAT12   "		;  8字节的磁盘格式名称
		RESB	18				; 空出18字节

; 程序主体
entry:
		MOV		AX,0
		MOV		SS,AX			; SS(Stack Segment)栈段寄存器置为0
		MOV		SP,0x7c00		; SP(Stack Point)将栈指针寄存器
		MOV		DS,AX			; DS(Data Segment)数据段寄存器置为0

; 读磁盘

		MOV		AX,0x0820
		MOV		ES,AX
		MOV		CH,0			; 
		MOV		DH,0			; 
		MOV		CL,2			; 
readloop:
		MOV		SI,0			; 
retry:
		MOV		AH,0x02			; AH=0x02 :读盘
		MOV		AL,1			; 1扇区
		MOV		BX,0
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用BIOS中断，13为磁盘操作
		JNC		next			; 没出错时，跳转为next，此处为鲁棒性考虑，磁盘读出数据错误是一件常见的事
		ADD		SI,1			; SI++，即错误计数
		CMP		SI,5			; SI与5比较
		JAE		error			; SI >= 5 跳至error错误
		MOV		AH,0x00
		MOV		DL,0x00			; A驱动器
		INT		0x13			; 调用BIOS中断，13为磁盘操作
		JMP		retry
next:
		MOV		AX,ES			; 内存后移0x200，0x200即512，单个簇数
		ADD		AX,0x0020
		MOV		ES,AX			; ADD ES,0x020 
		ADD		CL,1			; CL+1
		CMP		CL,18			; CL与18
		JBE		readloop		; CL <= 18 跳至readloop
		MOV		CL,1
		ADD		DH,1
		CMP		DH,2
		JB		readloop		; DH < 2 跳至readloop
		MOV		DH,0
		ADD		CH,1
		CMP		CH,CYLS
		JB		readloop		; CH < CYLS 跳至readloop

; 执行haribote.sys文件，该文件保存于img映像中的第一个文件
; 则是FAT12系统中的0x4200起才开始存储文件的
; 而这段img映像是被加载到0x8000的，
; 所谓haribote.sys应该是在内存中的0xc200位置

		MOV		[0x0ff0],CH		; IPL偑偳偙傑偱撉傫偩偺偐傪儊儌
		JMP		0xc200			; 执行haribote.sys文件，该文件保存于

error:
		MOV		SI,msg			; 将msg处的地址作为数据输出
putloop:
		MOV		AL,[SI]
		ADD		SI,1			; SI+1
		CMP		AL,0
		JE		fin
		MOV		AH,0x0e			; 功能号为0x0e
		MOV		BX,15			; 
		INT		0x10			; 显示字符的BOIS函数
		JMP		putloop
fin:
		HLT						; 省电模式
		JMP		fin				; 循环
msg:
		DB		0x0a, 0x0a		;
		DB		"load error"
		DB		0x0a			; 换行
		DB		0

		RESB	0x7dfe-$		; 空出磁盘直到偏移量为510处，为系统引导标识
		DB		0x55, 0xaa		; 系统引导标识
