org	    07c00h
BaseOfStack equ	07c00h

;-----------------------------
    jmp short   LABEL_START
    nop
    %include    "inc/fat12hdr.inc"
;-----------------------------

;-----------------------------
BaseOfLoader		equ	09000h
OffsetOfLoader		equ	0100h
BaseOfFat			equ	08ee0h
OffsetOfFat			equ	0
;-----------------------------

;-----------------------------------
LoaderFileName	db	"LOADER  BIN", 0
;-----------------------------------

LABEL_START:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack
	xor	ah, ah
	xor	dl, dl
	int	13h

LABEL_FIND_LOADER_IN_ROOT:

	mov	si, BeginSecOfRootDir
.loop:
	cmp	si, EndSecOfRootDir
	jz	LABEL_LOADER_NOT_FOUND
	mov	ax, si
	mov	cl, 1
	mov	dx,	BaseOfLoader
	mov	es, dx
	mov	bx, OffsetOfLoader
	call	ReadSector
	mov	di, 0
.searchInSector:
	cmp	di, 512
	jz	.endOfSearchInSector
	mov	ax, BaseOfLoader
	push	ax
	mov	ax,	OffsetOfLoader
	add	ax, di
	push	ax
	mov	ax, ds
	push	ax
	mov	ax, LoaderFileName
	push	ax
	push	11
	call	StrEqu
	cmp	ax, 1
	jz	LABEL_LOADER_FOUND
	add	di, 32
	jmp	.searchInSector
.endOfSearchInSector:
	inc	si
	jmp	.loop
	
LABEL_LOADER_NOT_FOUND:
	mov	ax, 0b800h
	mov	gs, ax
	mov	al, 'n'
	mov	ah, 0fh
	mov	[gs:(80 * 5 + 0) * 2], ax
	jmp $
	
LABEL_LOADER_FOUND:
	mov	bx,	BaseOfLoader
	mov	es, bx
	mov	bx,	OffsetOfLoader
	add	bx,	di
	add	bx,	26
	mov	dx, word [es:bx]
	
	mov	ax, BeginSecOfFat1
	mov	cl,	NumsSecOfFat1
	mov	bx,	BaseOfFat
	mov	es, bx
	mov	bx,	OffsetOfFat
	call	ReadSector
	
	mov	ax, dx
	add	ax, CluToSec
	mov	di, 0
	
LABEL_LOADER_LOADING:
	mov	bx, BaseOfLoader
	mov	es, bx
	mov	cl, 1
	mov	bx,	OffsetOfLoader
	add	bx, di
	call	ReadSector
	mov	bx, BaseOfFat
	mov	es, bx
	mov	bx,	OffsetOfFat
	call	FatTrans
	cmp	ax,	0
	jz	LABEL_LOADER_LOADED
	add	di, 512
	jmp	LABEL_LOADER_LOADING

LABEL_LOADER_LOADED:
	jmp	BaseOfLoader:OffsetOfLoader
	%include	"inc/lib.inc"
;----------------------
times 510-( $-$$ ) db 0
db 0x55, 0xaa
