;-----------------
SELECTOR_KERNEL_CS  equ	8

;-----------------

;------globalVars------
extern dispPos
extern gdtPtr
extern idtPtr
extern SELECTOR_TSS
extern procReadyPtr
extern P_LDT_SEL
extern tss
extern procTable
extern kReenter
extern irqTable
extern INT_M_CTLMASK
extern syscallTable
;----------------------

;------globalFuncs-----
extern cinit
extern minuxMain
extern outIntHandler
extern exceptionHandler
extern delay
extern clockHandler
;----------------------

;--------------
[SECTION .data]
backspace:      db      0ah, 0
breakpoint1:    db      "11111", 0ah, 0
breakpoint2:    db      "22222", 0ah, 0
breakpoint3:    db      "33333", 0ah, 0
clockIntMsg:	db	"*", 0ah, 0
;--------------

;--------------
[SECTION .bss]
StackSpace  resb    2 * 1024
StackTop:
;--------------
global restart
global _start
[SECTION .text]
_start:	
	mov	esp, StackTop
	sgdt	[gdtPtr]	
	call	cinit
	lgdt	[gdtPtr]
	lidt	[idtPtr]
	jmp	SELECTOR_KERNEL_CS:csinit
csinit:
	xor	eax, eax
	mov	ax, SELECTOR_TSS
	ltr	ax
	jmp	minuxMain
        hlt

%include "src/util.asm"
%include "src/interrupt.asm"
%include "src/include/sconst.inc"
%include "src/syscall.asm"
