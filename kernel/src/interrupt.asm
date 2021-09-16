;#define INT_M_CTL       0x20
;#define INT_M_CTLMASK   0x21
;#define INT_S_CTL       0xa0
;#define INT_S_CTLMASK   0xa1

%macro hwint_master 1
	call	save
	in	al, 21h
	or	al, (1 << %1)
	out	21h, al
	mov	al, EOI
	out	INT_M_CTL, al
	sti
	push	%1
	call	[irqTable + 4 * %1]
	pop	ecx
	cli
	in	al, 21h
	and	al, ~(1 << %1)
	out	21h, al
	ret
%endmacro

%macro hwint_slave 1
        push    %1
        call    outIntHandler
        add     esp, 4
        hlt
%endmacro


;-----------------------------

;-----------------------------
align 16
hwint00:
	hwint_master 0
align 16
hwint01:
        hwint_master 1
align 16
hwint02:
        hwint_master 2
align 16
hwint03:
        hwint_master 3
align 16
hwint04:
        hwint_master 4
align 16
hwint05:
        hwint_master 5
align 16
hwint06:
        hwint_master 6
align 16
hwint07:
        hwint_master 7
align 16

hwint08:
	hwint_slave 8
align 16
hwint09:
        hwint_slave 9
align 16
hwint10:
        hwint_slave 10
align 16
hwint11:
        hwint_slave 11
align 16
hwint12:
        hwint_slave 12
align 16
hwint13:
        hwint_slave 13
align 16
hwint14:
        hwint_slave 14
align 16
hwint15:
        hwint_slave 15
align 16

global hwint00
global hwint01
global hwint02
global hwint03
global hwint04
global hwint05
global hwint06
global hwint07
global hwint08
global hwint09
global hwint10
global hwint11
global hwint12
global hwint13
global hwint14
global hwint15
global divideError
global debugException
global nmi
global breakPointException
global overflow
global boundsCheck
global invalidOpcode
global coprUnavailable
global doubleFault
global coprSegOver
global invalidTss
global segNotPresent
global stackException
global generalProtection
global pageFault
global coprError

divideError:
	push	0xffffffff
	push	0
	jmp	exception
debugException:
	push	0xffffffff
	push	1
	jmp	exception
nmi:	
	push	0xffffffff
	push	2
	jmp	exception
breakPointException:
	push	0xffffffff
	push	3
	jmp	exception
overflow:
	push	0xffffffff
	push	4
	jmp	exception
boundsCheck:
	push	0xffffffff
	push	5
	jmp	exception
invalidOpcode:
	push	0xffffffff
	push	6
	jmp	exception
coprUnavailable:
	push	0xffffffff
	push	7
	jmp	exception
doubleFault:
	push	8
	jmp	exception
coprSegOver:
	push	0xffffffff
	push	9
	jmp	exception
invalidTss:
	push	10
	jmp	exception
segNotPresent:
	push	11
	jmp	exception
stackException:
	push	12
	jmp	exception
generalProtection:
	push	13
	jmp	exception
pageFault:
	push	14
	jmp	exception
coprError:
	push	0xffffffff
	push	16
	jmp	exception

exception:
	call	exceptionHandler
	add	esp, 8
	hlt	

;--------------------
save:
	pushad
	push	ds
	push	es
	push	fs
	push	gs
	mov	dx, ss
	mov	ds, dx
	mov	es, dx
	mov	esi, esp
	inc	dword [kReenter]
	cmp	dword [kReenter], 0
	jne	.1
	mov	esp, StackTop
	push	restart
	jmp	[esi + RETADR - P_STACKBASE]
.1:
	push	restartReenter
	jmp	[esi + RETADR - P_STACKBASE]

restart:
        mov     esp, [procReadyPtr]
        lldt    [esp + P_LDT_SEL]
        lea     eax, [esp + P_STACKTOP]
        mov     dword [tss + TSS3_S_SP0], eax
restartReenter:
        dec     dword [kReenter]
        pop     gs
        pop     fs
        pop     es
        pop     ds
        popad
        add     esp, 4
        iretd
;----------------------

;-------------------------
global syscall
syscall:
	call	save
	push	dword [procReadyPtr]
	sti
	push	ecx
	push	ebx
	call	[syscallTable + eax * 4]
	add	esp, 3*4
	mov	[esi + EAXREG -  P_STACKBASE], eax
	cli
	ret
;-------------------------
