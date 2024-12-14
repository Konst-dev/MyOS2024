use16
KERNEL_SEG		EQU	1000h
C_SEGMENT		EQU 8192

START:
	cli
	mov ax,KERNEL_SEG
	mov	es,ax
	mov ds,ax
	mov ss,ax
	mov sp,STACK
	sti
	
	mov si,84h
	push es
	xor ax,ax
	mov es,ax
	mov ax,INT21
	mov [es:si],ax
	mov word[es:si+2],KERNEL_SEG
	pop es
	

	
	mov ax,3
	mov bx,0500h
	int 21h
	
	mov ax,4
	mov bl,0Ch
	int 21h

	mov ax,1
	int 21h
	
	mov ax,5
	int 21h
	
	
	mov ax,KERNEL_SEG
	mov es,ax
	mov ds,ax
	mov si,str1
	mov ax,5
	int 21h
	
	mov bl,127
	mov ax,6
	int 21h

	mov ax,5
	int 21h

	mov ax,KERNEL_SEG
	mov es,ax
	mov ds,ax
	mov si,str1
	mov ax,5
	int 21h
	
	mov bx,125
	mov ax,7
	int 21h

	mov ax,5
	int 21h
	
	mov ax,KERNEL_SEG
	mov es,ax
	mov ds,ax
	mov si,str1
	mov ax,5
	int 21h
	
	mov ebx,1234567890
	mov ax,8
	int 21h

	mov ax,5
	int 21h
	
	mov ax,4
	mov bl,02h
	int 21h
	
	mov ax,KERNEL_SEG
	mov es,ax
	mov ds,ax
	mov si,PMstr
	mov ax,5
	int 21h
	
	call progress
	
	jmp KERNEL32
	
INT21:	
	push bx
	cli
	mov bx,cs
	mov	es,bx
	mov ds,bx
	mov ss,bx
	;mov sp,STACK
	sti
	
	mov bl,3
	mul bl
	mov bx,SWITCH
	add ax,bx
	pop bx
	jmp ax
	
SWITCH:
	jmp near FUNC00
	jmp near FUNC01
	jmp near FUNC02
	jmp near FUNC03
	jmp near FUNC04
	jmp near FUNC05
	jmp near FUNC06
	jmp near FUNC07
	jmp near FUNC08
	
FUNC00:
	mov bx,0001h
	xor ax,ax
	iret

OSName db 'MyOS 2024 v0.01',10,'Copyright (c) MyOS 2024',0
FUNC01:;возвращает в es:si адрес строки с названием ОС
	mov si,OSName
	mov ax,KERNEL_SEG
	mov es,ax
	xor ax,ax
	iret
FUNC02:	;Очистка экрана 
	mov ax,0B000h
	mov es,ax
	mov si,8000h
	mov cx,1000
F02_1:
	mov eax,07000700h
	mov [es:si],eax
	add si,4
	loop F02_1
	mov ax,KERNEL_SEG
	mov ds,ax
	mov byte[cursX],0
	mov byte[cursY],0
	mov ah,2
	mov bh,0
	xor dx,dx
	int 10h
	xor ax,ax	
	iret

FUNC03:;задает координаты курсора, bh - Y, bl - X 
	mov ax,KERNEL_SEG
	mov ds,ax
	mov [cursX],bl
	mov [cursY],bh
	mov ah,2
	mov bh,0
	mov dl,[cursX]
	mov dh,[cursY]
	int 10h
	xor ax,ax
iret

FUNC04:;задает аттрибуты текста из bl 
	mov ax,KERNEL_SEG
	mov ds,ax
	mov [textAttr],bl
	xor ax,ax
iret

strCount	dw 	0
FUNC05:;выводит строку с глобальной позиции курсора
		;перемещает курсор. вход es:si - адрес строки
	;mov ax,0B000h
	mov ax,KERNEL_SEG
	mov ds,ax
	mov di,8000h

	mov al,160
	mul byte[cursY]
	mov bl,[cursX]
	and bx,00FFh
	shl bx,1
	add ax,bx
	add di,ax
	
	mov ax,0B000h
	mov ds,ax
F05_2:	
	cmp byte[es:si],0
	je	F05_1
	cmp byte[es:si],0Ah
	je F05_4
	mov al,[es:si]
	mov [ds:di],al
	push ds
	mov ax,KERNEL_SEG
	mov ds,ax
	mov al,[textAttr]
	pop ds
	mov [ds:di+1],al
	push ds
	mov ax,KERNEL_SEG
	mov ds,ax
	inc byte[cursX]
	cmp byte[cursX],80
	jne F05_3
	inc byte[cursY]
	mov byte[cursX],0
	cmp byte[cursY],25
	jne F05_3
	mov byte[cursY],24
	call Shift
	mov di,8F00h
F05_3:	
	pop ds
	inc si
	add di,2
	jmp F05_2 
F05_4:
	push ds
	mov ax,KERNEL_SEG
	mov ds,ax
	inc byte[cursY]
	mov byte[cursX],0
	cmp byte[cursY],25
	jne F05_5
	mov byte[cursY],24
	call Shift
F05_5:	
	mov al,160
	mul byte[cursY]
	mov di,8000h
	add di,ax
	inc si
	pop ds
	jmp F05_2
F05_1:
	mov ax,KERNEL_SEG
	mov ds,ax
	;inc word[strCount]
	mov ah,2
	mov bh,0
	mov dl,[cursX]
	mov dh,[cursY]
	int 10h
	xor ax,ax
	
iret

hex		db	'00',0
hexStr	db	'0123456789ABCDEF'
FUNC06:; Преобразует байт bl в hex и возвращает адрес строки c числом в es:si
	mov ax,KERNEL_SEG
	mov ds,ax
	mov es,ax
	movzx ax,bl
	mov bl,16
	div bl
	mov di,hexStr
	mov si,hex
	movzx bx,ah
	add di,bx
	mov bl,[ds:di]
	mov	[es:si+1],bl
	
	mov di,hexStr
	movzx bx,al
	add di,bx
	mov bl,[ds:di]
	mov	[es:si],bl
	xor ax,ax
iret

decN		db '00000',0
FUNC07:;преобразует слово в bx в десятичную запись
		;выход es:si - адрес строки с числом
	mov ax,KERNEL_SEG
	mov ds,ax
	mov es,ax
	mov cx,5
	mov si,decN+4
	mov ax,bx

F07_2:	
	xor dx,dx
	mov bx,10
	div bx
	mov di,hexStr
	add di,dx
	mov bl,[ds:di]
	mov [es:si],bl
	cmp ax,0
	je F07_1
	dec si
	loop F07_2
	
F07_1:
	xor ax,ax
iret

decN32		db '0000000000',0
FUNC08:;преобразует двойное слово в ebx в десятичную запись
		;выход es:si - адрес строки с числом
	mov ax,KERNEL_SEG
	mov ds,ax
	mov es,ax
	mov cx,10
	mov si,decN32+9
	mov eax,ebx

F08_2:	
	xor edx,edx
	mov ebx,10
	div ebx
	mov di,hexStr
	add di,dx
	mov bl,[ds:di]
	mov [es:si],bl
	cmp eax,0
	je F08_1
	dec si
	loop F08_2
	
F08_1:
	xor eax,eax
iret

Shift:;сдвигает экран, если переполнен
	push si
	push es
	push ds
	mov ax,0B000h
	mov	ds,ax
	mov es,ax
	mov si,8000h
	mov di,80A0h
	mov cx,1000
Shift_01:
	mov eax,[ds:di]
	mov [es:si],eax
	add si,4
	add di,4
	loop Shift_01
	pop ds
	pop es
	pop si
ret

delay:; задержка. вход - ebx - количество циклов задержки
	xor ecx,ecx
.l1:
	cmp ecx,ebx
	je .l2
	inc ecx
	jmp .l1
.l2:	
ret

strDot		db	'.',0
strDone		db	'Done',0
progress:
.l1:	
	mov ax,KERNEL_SEG
	mov es,ax
	mov ds,ax
	mov si,strDot
	mov ax,5
	int 21h
	mov ebx,100000
	call delay
	cmp byte[cursX],76
	jne .l1
	mov si,strDone
	mov ax,5
	int 21h

ret

cursX		db	0
cursY		db	0
textAttr	db	0Fh 
str1		db	10,0
str2		db	' ',0	
PMstr		db	10,'Switching to Protected Mode...',0
times 1024 db 0
STACK:

GDT:
	dd 0,0
	db 0FFh,0FFh,00h,00h,01h,10011010b,11001111b,00h; сегмент кода
	db 0FFh,0FFh,00h,00h,01h,10010010b,11001111b,00h; сегмент данных
	db 0FFh,0FFh,00h,80h,0Bh,10010010b,01000000b,00h; видеобуфер
	db 0FFh,0FFh,00h,00h,01h,10011010b,00000000b,00h; код реального режима
GDT_size equ $-GDT
GDTR	dw GDT_size-1
		dd GDT
IDT:
		dd 0,0;0
		dw syscall_handler,0008h,1000111000000000b,0;1
		dd 0,0
		dd 0,0
		dd 0,0
		dd 0,0
		dd 0,0
		dd 0,0
		dw int8_handler,0008h,1000111000000000b,0;системный таймер
		dw int9_handler,0008h,1000111000000000b,0;клавиатура
		dw int_EOI, 0008h,1000111000000000b,0;ведомый контроллер прерываний
		dw int_EOI, 0008h,1000111000000000b,0;COM2
		dw int_EOI, 0008h,1000111000000000b,0;COM1
		dw exGP_handler,0008h,1000111000000000b,0;General Protection Fault
		dw int_EOI, 0008h,1000111000000000b,0;FDD
		dw int_EOI, 0008h,1000111000000000b,0;LPT1
		dw int_EOI, 0008h,1000111000000000b,0
		dw int_EOI, 0008h,1000111000000000b,0
		dw int_EOI, 0008h,1000111000000000b,0
		dw int_EOI, 0008h,1000111000000000b,0
		dw int_EOI, 0008h,1000111000000000b,0
		dw int_EOI, 0008h,1000111000000000b,0
		dw int_EOI, 0008h,1000111000000000b,0
		dw int_EOI, 0008h,1000111000000000b,0
IDT_size equ $-IDT
IDTR	dw IDT_size-1
		dd IDT
REAL_IDTR 	dw 3FFh
			dd 0

KERNEL32:
	cli
	mov ax,cs
	mov ds,ax
	mov es,ax
	mov ss,ax
	mov sp, STACK
	mov bp,sp
	
	;открываем линию A20
	in al,92h
	or al,2
	out 92h,al
	

	in al,70h
	or al,80h
	out 70h,al;запрет NMI
	
	;теперь надо вычислить линейный адрес GDT
	xor eax,eax
	mov ax,cs
	shl eax,4
	add ax,GDT
	mov dword[GDTR+2],eax
	
	;теперь надо вычислить линейный адрес IDT
	xor eax,eax
	mov ax,cs
	shl eax,4
	add ax,IDT
	mov dword[IDTR+2],eax

	lgdt [GDTR];загрузка GDTR
	lidt [IDTR];загрузка IDT
	
	;переходим в защищенный режим 
	mov eax,cr0
	or al,1
	mov cr0,eax
	
	db 66h; префикс изменения разрядности операнда
	db 0EAh; опкод команды JMP FAR
	ENTRY_OFF dd PROTECTED_ENTRY;32-битное смещение
	dw 8; селектор первого дескриптора
	
use32
PROTECTED_ENTRY:	
	mov ax, 00010000b;DATA
	mov ds,ax
	mov ss,ax
	mov ax,00011000b;Video
	mov es,ax
	
	in al,70h
	and al,7Fh
	out 70h,al
	sti

	;вызов внешней функции, написанной на C. Таблица с адресами функций прикреплена по адресу C_SEGMENT
	;сразу за таблицей идет код, написанный на C

	
	mov eax,1
	call getFnAddress
	xor ebx,ebx
	mov bl,[cursY]
	push ebx
	mov bl,[cursX]
	push ebx
	call eax
	pop eax
	pop eax



	mov eax,8
	call getFnAddress
	call eax

	
	push eax
	push numStr
	mov eax,3
	call getFnAddress
	call eax
	pop eax
	pop eax

	mov eax,9
	call getFnAddress
	push numStr
	call eax 
	pop eax

	mov eax,9
	call getFnAddress
	push strDet
	call eax 
	pop eax

	mov eax,0
	call getFnAddress
	call eax
	;call testCursor

	jmp $
testStr		db	'Test String',10,0	
numStr times 12 db 0
strDet		db	' Mb RAM Detected',10,0

getFnAddress: ;возвращает адресс функции из блока С по ее номеру. Вход eax, выход, eax
	inc eax
	shl eax,5 ;умножаем на 32
	add eax,C_SEGMENT
	mov eax,[eax]
	add eax,C_SEGMENT
	add eax, [C_SEGMENT]
ret

testCursor:
	mov ecx,1012

	    mov al, 0fh             ;Cursor Location Low Register --
        mov dx, 3d4h            ;VGA port 3D4h
        out dx, al
        mov ax, cx              ;restore 'postion' back to AX
        mov dx, 3d5h            ;VGA port 3D5h
        out dx, al              ;send to VGA hardware

        ;cursor HIGH port to vga INDEX register
        mov al, 0eh
        mov dx, 3d4h            ;VGA port 3D4h
        out dx, al
        mov ax, cx              ;restore 'position' back to AX
        shr ax, 8               ;get high byte in 'position'
        mov dx, 3d5h            ;VGA port 3D5h
        out dx, al
ret

;обработчик прерываний защищенного режима
;=========================================

;IRQ 0 обработчик системного таймера
int8_handler:
	;inc byte[es:158]
	;inc dword[ds:100204];

	push eax
	mov eax,10
	call getFnAddress
	call eax
	pop eax
	jmp int_EOI
	
;IRQ 1 - клавиатура
int9_handler:
	push ax
	push edi
	xor ax,ax
	
	in al,60h;считываем позиционный код клавиши
	
	dec al
	jnz _continue_handling
	
	mov ax,3
	push 10h
	call REAL_MODE_SWITCH_SERVICE
	mov dword[cursor],0

	jmp Ack
	
_continue_handling:
	mov ah,al
	and ah,80h
	jnz clear_request
	
;преобразуем позиционный код в ASCII по таблице
	and al,7Fh
	push edi
	mov edi,ascii
	add di,ax
	mov al,[edi]
	pop edi
	
; выводим символы на экран один за другим
	mov edi,dword[cursor]
	shl edi,1
	mov byte[es:edi],al
	inc dword[cursor]
	call checkCursor
;посылаем подтверждение обработки в порт клавиатуры
;установка и сброс 7-го бита порта 61h
Ack:
	in al,61h
	or al,80h
	out 61h,al
	xor al,80h
	out 61h,al
	
clear_request:
	pop edi
	pop ax
	jmp int_EOI

;Пустой обработчик
int_EOI:
	push ax
	mov al,20h
	out 20h,al
	out 0A0h,al
	pop ax
	iretd

exGP_handler:
	pop eax
	mov esi,gp
	int 1
	iretd

syscall_handler:	
	pushad
_puts:
	lodsb
	mov edi,dword[cursor]
	mov [es:edi*2],al
	mov bl,byte[attr]
	mov byte[es:edi*2+1],bl
	inc dword[cursor]
	call checkCursor
	test al,al
	jnz _puts
	popad
	iretd

checkCursor:
	cmp dword[cursor],2000
	jne cCexit
	pushad
	mov edi,0
	mov ecx,960
_l009:
	mov eax,[es:edi+160]
	mov [es:edi],eax
	add edi,4
	loop _l009
	
	mov edi,3840
	xor eax,eax
	mov ah,[attr]
	shl eax,16
	mov ah,[attr]
	mov ecx,40
_l010:
	mov [es:edi],eax
	add edi,4
	loop _l010
	
	mov dword[cursor],1920
	popad
cCexit:
ret

old_ax 	dw 0
old_cl 	db 0	
REAL_MODE_SWITCH_SERVICE:
	mov [old_cl],cl
	mov [old_ax],ax
	
	mov cl,[esp+4]
	
	pushfd
	cli
	in al,70h
	or al,80h
	out 70h,al;запрет всех прерываний
	
;переключаемся обратно в реальный режим
	lidt [REAL_IDTR]
;загружаем в cs селектор 16-битного сегмента
	jmp 00100000b:_CONT
	
use16
_CONT:
;мы в 16-битном сегменте, переключаемся в реальный режим
	mov eax,cr0
	and al,0FEh
	mov cr0,eax
	jmp KERNEL_SEG:REAL_ENTRY
	
;код реального режима
REAL_ENTRY:
	mov ax,cs
	mov ds,ax
	mov ss,ax
	mov es,ax

;разрешаем прерывания
	in al,70h
	and al,7Fh
	out 70h,al
	sti
;****************************************	
	;mov ax,0B000h
	;mov es,ax
	;mov ax,8002h
	;mov si,ax
	;mov byte[es:si],65
	;mov byte[es:si+1],12h

	;mov ax,3
	;int 10h
	
;*******************************	
	
	mov [int_no],cl
	mov ax,[old_ax]
	mov cl,[old_cl]
	
	db 0CDh; int XX
	int_no db 0
	
	mov [old_ax],ax
	
;запрет всех прерываний
	cli
	in al,70h
	or al,80h
	out 70h,al
	
;переключаемся в PM
	mov eax,cr0
	or al,1
	mov cr0,eax
	
	db 66h,0EAh
	E_OFF dd RESTORE_ENTRY
	dw 8
	
	;jmp 00001000b:RESTORE_ENTRY
	
;мы снова в PM

use32
RESTORE_ENTRY:
	lidt [IDTR]
	mov ax,00010000b;DATA
	mov ds,ax
	mov ss,ax
	mov ax,00011000b;Video
	mov es,ax

;разрешаем прерывания
	in al,70h
	and al,7Fh
	out 70h,al
	popfd
	sti
	mov ax,[old_ax]
	
	ret 4
	
ascii db 0,'1234567890-+',0,0,'QWERTYUIOP[]',0,0,'ASDFGHJKL;',"'`",0,0,'ZXCVBNM,./',0,'*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,'789-456+1230.',0,0
string db ' Switched to Protected Mode. Press <Esc> to clear display',0
gp db '**GENERAL PROTECTION FAULT**',0
cursor dd 80
attr db 0Fh
addr dd 00000040h
times C_SEGMENT -$+START db 0