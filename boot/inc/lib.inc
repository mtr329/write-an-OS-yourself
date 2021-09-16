;--------------------------------------------------------------------
ReadSector:		;from number ax sector, reading cl sectors to [es:bx]
	push	ax
	push	cx
	push	dx
	push	bx
	mov	bl, byte [BPB_SecPerTrk]
	div	bl
	mov	bx, ax
	mov	ah, 02h
	mov	al, cl
	mov	ch, bl
	shr	ch, 1
	mov	cl, bh
	inc	cl
	mov	dh, bl
	and	dh, 1
	mov	dl, byte [BS_DrvNum]
	pop	bx
.Reading:
	int 13h
	jc	.Reading
	pop	dx
	pop	cx
	pop	ax
	ret
;--------------------------------------------------------------------

;------------------------------------------------------------------------------------------------------
FatTrans:	;es = BaseOfFat, bx = OffsetOfFat, ax is the sector number, return next sec(or 0) by ax
	push	bx
	push	cx
	push	bp
	sub	ax, 31
	mov	bp, ax
	and	bp, 1
	mov	cx, ax
	shr	cx, 1
	shr	ax, 1
	add	cx, ax
	add	cx, ax
	add	bx, cx	;base = es:bx
	mov	cx, 0
	mov	ax, 0
	cmp	bp, 0
	jz	.even
	inc	bx
	mov	al, byte [es:bx]
	shr	al, 4
	inc	bx
	mov	cl, byte [es:bx]
	shl	cx, 4
	or	ax, cx
	jmp	.judge
.even:
	mov	al, byte [es:bx]
	inc	bx
	mov	cl, byte [es:bx]
	and	cl, 0fh
	shl	cx, 8
	or	ax, cx
.judge:
	cmp	ax, 0xff7
	jc	.ret
	mov	ax, 0
	pop	bp
	pop	cx
	pop	bx
	ret
.ret:
	add	ax, 31
	pop	bp
	pop	cx
	pop	bx
	ret
	
;------------------------------------------------------------------------------------------------------

;---------------------------------------------------------------------------------------------
StrEqu:    ;to compare if two strings(push seg1, off1, seg2, off2, len16) are equal, return ax.
    push    di
    push    bx
    push    cx
    push    es
    push    bp
    mov	di, 0
.loop:
    mov	bp, sp
    add	bp, 12
    cmp	di, [bp]
    jz	.equal
    add	bp, 8
    mov	ax, [bp]
    mov	es, ax
    sub	bp, 2
    mov	bx, [bp]
    add	bx, di
    mov	cl, [es:bx]
    sub	bp, 2
    mov	ax, [bp]
    mov	es, ax
    sub	bp, 2
    mov	bx, [bp]
    add	bx, di
    mov	ch, [es:bx]
    cmp	cl, ch
    jnz	.notEqual
    inc	di
    jmp	.loop
.equal:
    mov	ax, 1
    jmp	.end
.notEqual:
    mov	ax, 0
    jmp	.end
.end:
    pop	bp
    pop	es
    pop	cx
    pop	bx
    pop	di
    ret	10
;---------------------------------------------------------------------------------------------

