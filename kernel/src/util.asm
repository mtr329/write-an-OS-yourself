;----------------
global disableInt
disableInt:
	cli
	ret
;----------------

;---------------
global enableInt
enableInt:
	sti
	ret
;---------------

;------------
global memset
memset:		;f(addr_4, value_4, num_4)
	push	edi
	push	eax
	push	edx
	mov	edi, [esp + 16]
	mov	eax, [esp + 20]
	and	eax, 0ffh
	mov	edx, [esp + 24]
	push	ecx
	mov	ecx, 0
.loop:
	cmp	ecx, edx
	jz	.end
	mov	[edi], al
	inc	ecx
	inc	edi
	jmp	.loop
.end:
	pop	ecx
	pop	edx
	pop	eax
	pop	edi
	ret
;------------

;------------
global inByte
inByte:		;int f(port_4), returnV in eax
	push	edx
	mov	edx, [esp + 8]
	xor	eax, eax
	in	al, dx
	pop	edx
	nop	
	nop
	ret
;------------

;-------------
global outByte
outByte:	;void f(port_4, value_1)
	push	edx
	push	ax
	mov	edx, [esp + 10]
	mov	al, [esp + 14]
	out	dx, al
	pop	ax
	pop	edx
	nop
	nop
	ret
;-------------

;--------------
global dispNum4
dispNum4:	;void f(num_4)
	push	eax
	mov	eax, [esp + 8]
	push	ebx
	push	edi
	mov	edi, [dispPos]
	mov	bh, 0fh
	mov	bl, '0'
	mov	[gs:edi], bx
	add	edi, 2
	mov	bl, 'x'
	mov	[gs:edi], bx
	add	edi, 2
	push	cx
	mov	cx, 0
.loop:
	cmp	cx, 8
	jz	.end
	mov	ebx, eax
	shl	eax, 4
	shr	ebx, 28
	and	bl, 0fh
	add	bl, '0'
	cmp	bl, '9'
	jna	.1
	add	bl, 7	
.1:
	mov	bh, 0fh
	mov	[gs:edi], bx
	add	edi, 2
	inc	cx
	jmp	.loop
.end:
	mov	[dispPos], edi
	pop	cx
	pop	edi
	pop	ebx	
	pop	eax
	ret
;--------------

;--------------
global	dispStr
dispStr:	;void f(*begin_4)
	push	edi
	mov	edi, [esp + 8]
	push	ebp
	push	ax
	mov	ebp, [dispPos]
	mov	ah, 0fh
.loop:
	mov	al, [edi]
	cmp	al, 0
	jz	.end
	cmp	al, 10
	jnz	.notNewLine
	push	eax
	push	bx
	mov	eax, ebp
	mov	bl, 160
	div	bl
	and	eax, 0fh
	inc	al
	mul	bl
	mov	ebp, eax
	pop	bx
	pop	eax
	inc	edi
	jmp	.loop
.notNewLine:
	mov	[gs:ebp], ax
	add	ebp, 2
	inc	edi
	jmp	.loop
.end:
	mov	[dispPos], ebp
	pop	ax
	pop	ebp
	pop	edi
	ret
;--------------

;--------------
global	memCopy
memCopy:	;void f(*to_4, *from_4, size_4)
	push	ebx
	push	ebp
	push	edi
	push	ax
	push	ecx
	mov	edi, [esp + 22]	;to
	mov	ebp, [esp + 26]	;from
	mov	ebx, [esp + 30]	;size
	xor	ecx, ecx
.loop:
	cmp	ecx, ebx
	jz	.end
	mov	al, [ebp]
	mov	[edi], al
	inc	ecx
	inc	ebp
	inc	edi
	jmp	.loop
.end:
	pop	ecx
	pop	ax
	pop	edi
	pop	ebp
	pop	ebx
	ret
;--------------
