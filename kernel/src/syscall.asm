INT_VECTOR_SYSCALL	equ	0x90
NR_GET_TICKS		equ	0
NR_WRITE		equ	1


bits 32
[Section .text]
global getTicks
getTicks:
	mov	eax, NR_GET_TICKS
	int	INT_VECTOR_SYSCALL
	ret

global write
write:
	mov	eax, NR_WRITE
	mov	ebx, [esp + 4]
	mov	ecx, [esp + 8]
	int	INT_VECTOR_SYSCALL
	ret
