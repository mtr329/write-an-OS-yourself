org	0100h
;------------------------
BaseOfFat	    equ	08ee0h
OffsetOfFat	    equ	0

BaseOfStack	    equ	0100h

BaseOfKernel	    equ	08000h
OffsetOfKernel	    equ	0
BaseOfKernelPhyAddr equ	BaseOfKernel * 10h

BaseOfLoader	    equ	09000h
OffsetOfLoader	    equ	0100h
BaseOfLoaderPhyAddr equ	BaseOfLoader * 10h

PageDirBase	    equ	100000h
PageTblBase	    equ	101000h

KernelEntryPhyAddr  equ	030400h
;------------------------

;-----------------------
KernelFileName	db	"KERNEL  BIN"
;-----------------------

;-----------------------------------	
	jmp	short LABEL_START
	nop
;-----------------------------------	

%include    "inc/fat12hdr.inc"
%include    "inc/pm.inc"

LABEL_GDT:	    Descriptor	0,	    0,		0
LABEL_DESC_FLAT_C:  Descriptor	0,	    0fffffh,	DA_CR|DA_32|DA_LIMIT_4K
LABEL_DESC_FLAT_RW: Descriptor	0,	    0fffffh,	DA_DRW|DA_32|DA_LIMIT_4K
LABEL_DESC_VIDEO:   Descriptor	0b8000h,    0fffffh,	DA_DRW|DA_DPL3	

GdtLen	equ $ - LABEL_GDT
GdtPtr	dw  GdtLen
	dd  BaseOfLoaderPhyAddr + LABEL_GDT

SelectorFlatC	equ LABEL_DESC_FLAT_C	-   LABEL_GDT
SelectorFlatRW	equ LABEL_DESC_FLAT_RW	-   LABEL_GDT
SelectorVideo	equ LABEL_DESC_VIDEO	-   LABEL_GDT	+   SA_RPL3
;--------------------------------

;-------------------------------
LABEL_START:
	mov	ax, cs
	mov	ds, ax
	mov	es, ax
	mov	ss, ax
	mov	sp, BaseOfStack
	mov	ah, 0
	mov	dl, 0
	int	13h

LABEL_GET_MEM_SIZE:
	mov	ebx, 0
	mov	di, _MemChkBuf
.MemChkLoop:
	mov	eax, 0e820h
	mov	ecx, 20
	mov	edx, 0534d4150h
	int	15h
	jc	.MemChkFail
	cmp	dword [di + 16], 1
	jnz	.afterCount
	mov	eax, dword [di]
	add	eax, dword [di + 8]
	cmp	eax, dword [_dwMemSize]
	jna	.afterCount
	mov	dword [_dwMemSize], eax
.afterCount:	
	add	di, 20
	inc	dword [_dwMCRNumber]
	cmp	ebx, 0
	jnz	.MemChkLoop
	jmp	.MemChkOK
.MemChkFail:
	mov	dword [_dwMCRNumber], 0
	mov	dword [_dwMemSize], 0
.MemChkOK:
	
LABEL_FIND_KERNEL_IN_ROOT:
	mov	si, BeginSecOfRootDir
.loop:
	cmp	si, EndSecOfRootDir
	jz	LABEL_KERNEL_NOT_FOUND
	mov	ax, si
	mov	cl, 1
	mov	dx,	BaseOfKernel
	mov	es, dx
	mov	bx, OffsetOfKernel
	call	ReadSector
	mov	di, 0	
.searchInSector:
	cmp	di, 512
	jz	.endOfSearchInSector
	mov	dx, 0
.strCmp:
	cmp	dx, 11
	jz	.equal
	mov	bx, BaseOfKernel
	mov	es, bx
	mov	bx, OffsetOfKernel
	add	bx, dx
	add	bx, di
	mov	cl, [es:bx]
	mov	bx, ds
	mov	es, bx
	mov	bx, KernelFileName
	add	bx, dx
	mov	ch, [es:bx]
	cmp	cl, ch
	jnz	.notEqual
	inc	dx
	jmp	.strCmp
.equal:
	jmp	LABEL_KERNEL_FOUND
.notEqual:
	add	di, 32
	jmp	.searchInSector
.endOfSearchInSector:
	inc	si
	jmp	.loop
	
LABEL_KERNEL_NOT_FOUND:
	mov	ax, 0b800h
	mov	gs, ax
	mov	al, 'N'
	mov	ah, 0fh
	mov	[gs:(80 * 5 + 0) * 2], ax
	jmp $
	
LABEL_KERNEL_FOUND:	
	mov bx, BaseOfKernel
	mov es, bx
	mov bx, OffsetOfKernel
	add bx, di
	add bx, 26
	mov dx, word [es:bx]
	mov ax, dx
	add ax, CluToSec

	mov di, 0
	mov bp, 0
LABEL_KERNEL_LOADING:
	mov bx, BaseOfKernel
	mov es, bx
	mov cl, 1
	mov bx, OffsetOfKernel
	add bx, di
	call	ReadSector
	mov bx, BaseOfFat
	mov es, bx
	mov bx,	OffsetOfFat
	call	FatTrans
	cmp ax, 0
	jz  LABEL_KERNEL_LOADED
	add di, 512
	inc bp
	jmp LABEL_KERNEL_LOADING

LABEL_KERNEL_LOADED:
	lgdt	[GdtPtr]
	cli
	in  al, 92h
	or  al, 00000010b
	out 92h, al
	mov eax, cr0
	or  eax, 1
	mov cr0, eax
	jmp dword SelectorFlatC:(BaseOfLoaderPhyAddr + LABEL_PM_START)
	
%include "inc/lib.inc"
;-------------end of s16-----------------

[SECTION .s32]
align	32
[Bits	32]

LABEL_PM_START:
	mov ax, SelectorVideo
	mov gs, ax
	mov ax, SelectorFlatRW
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
	mov esp, TopOfStack

LABEL_SETUP_PAGING:
	mov edx, 0
	mov eax, [dwMemSize]
	mov ebx, 400000h
	div ebx
	mov [PageNum], eax
	cmp edx, 0
	jz  .gotPageNum
	inc dword [PageNum]
.gotPageNum:
	mov ax, SelectorFlatRW
	mov es, ax
	mov edi, PageDirBase
	mov eax, PageTblBase | PG_P | PG_USU | PG_RWW
	mov ecx, [PageNum]
.loop1:
	cmp ecx, 0
	jz  .endLoop1
	mov [edi], eax
	add edi, 4
	add eax, 4096
	dec ecx
	jmp .loop1
.endLoop1:
	mov eax, [PageNum]
	mov ebx, 1024
	mul ebx
	mov ecx, eax
	mov edi, PageTblBase
	xor eax, eax
	mov eax, PG_P | PG_USU | PG_RWW
.loop2:
	cmp ecx, 0
	jz  .endLoop2
	mov [edi], eax
	add edi, 4
	add eax, 4096
	dec ecx
	jmp .loop2
.endLoop2:
	mov eax, PageDirBase
	mov cr3, eax
	mov eax, cr0
	or  eax, 80000000h
	mov cr0, eax
	nop
	nop

LABEL_INIT_KERNEL:  
	mov cx, [BaseOfKernelPhyAddr + 2ch]
	mov ebx, [BaseOfKernelPhyAddr + 1ch]
	add ebx, BaseOfKernelPhyAddr
.loop:
	cmp cx, 0
	jz  .loopEnd
	mov edx, 0
.copy:
	mov esi, ebx
	add esi, 14h
	cmp edx, [esi]
	jz  .copyEnd
	mov esi, ebx
	add esi, 4h
	mov ebp, [esi]
	add ebp, edx
	add ebp, BaseOfKernelPhyAddr
	mov al, [ebp]
	mov esi, ebx
	add esi, 8h
	mov ebp, [esi]
	add ebp, edx
	mov [ebp], al
	inc edx
	jmp .copy
.copyEnd:
	dec cx
	mov eax, 0
	mov ax, [BaseOfKernelPhyAddr + 2ah]
	add ebx, eax
	jmp .loop
.loopEnd:	
	
	jmp SelectorFlatC:KernelEntryPhyAddr

[SECTION .data1]
align	32
LABEL_DATA:

_dwMCRNumber:	dd  0
_dwMemSize:	dd  0
_dwDispPos:	dd  0
_ARDStruct:	
    _dwBaseAddrLow:	dd  0
    _dwBaseAddrHigh:	dd  0
    _dwLengthLow:	dd  0
    _dwLengthHigh:	dd  0
    _dwType:		dd  0
_MemChkBuf: times   256	dd  0   
_PageNum:		dd  0


dwDispPos	equ BaseOfLoaderPhyAddr + _dwDispPos
dwMemSize	equ BaseOfLoaderPhyAddr + _dwMemSize
ARDStruct	equ BaseOfLoaderPhyAddr + _ARDStruct
dwBaseAddrLow   equ BaseOfLoaderPhyAddr + _dwBaseAddrLow
dwBaseAddrHigh  equ BaseOfLoaderPhyAddr + _dwBaseAddrHigh
dwLengthLow     equ BaseOfLoaderPhyAddr + _dwLengthLow
dwLengthHigh    equ BaseOfLoaderPhyAddr + _dwLengthHigh
dwType		equ BaseOfLoaderPhyAddr + _dwType
MemChkBuf	equ BaseOfLoaderPhyAddr + _MemChkBuf
PageNum		equ BaseOfLoaderPhyAddr + _PageNum

StackSpace: times 1024 db 0
TopOfStack  equ	BaseOfLoaderPhyAddr + $
;----------end of .data1--------------
