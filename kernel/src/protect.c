#include "global.h"

extern Gate idt[];

void	divideError();
void	debugException();
void	nmi();
void	breakPointException();
void	overflow();
void	boundsCheck();
void	invalidOpcode();
void	coprUnavailable();
void	doubleFault();
void	coprSegOver();
void	invalidTss();
void	segNotPresent();
void	stackException();
void	generalProtection();
void	pageFault();
void	coprError();
void	hwint00();
void	hwint01();
void	hwint02();
void	hwint03();
void	hwint04();
void	hwint05();
void	hwint06();
void	hwint07();
void	hwint08();
void	hwint09();
void	hwint10();
void	hwint11();
void	hwint12();
void	hwint13();
void	hwint14();
void	hwint15();

void initPort(){
	init8259A();
	initIdtDesc(INT_VECTOR_DIVIDE, DA_386IGate, divideError, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_DEBUG, DA_386IGate, debugException, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_NMI, DA_386IGate, nmi, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_BREAKPOINT, DA_386IGate, breakPointException, PRIVILEGE_USER);
	initIdtDesc(INT_VECTOR_OVERFLOW, DA_386IGate, overflow, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_BOUNDS, DA_386IGate, boundsCheck, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_INVAL_OP, DA_386IGate, invalidOpcode, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_COPROC_NOT, DA_386IGate, coprUnavailable, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_DOUBLE_FAULT, DA_386IGate, doubleFault, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_COPROC_SEG, DA_386IGate, coprSegOver, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_INVAL_TSS, DA_386IGate, invalidTss, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_SEG_NOT, DA_386IGate, segNotPresent, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_STACK_FAULT, DA_386IGate, stackException, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_PROTECTION, DA_386IGate, generalProtection, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_PAGE_FAULT, DA_386IGate, pageFault, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_COPROC_ERR, DA_386IGate, coprError, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_IRQ0 + 0, DA_386IGate, hwint00, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_IRQ0 + 1, DA_386IGate, hwint01, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_IRQ0 + 2, DA_386IGate, hwint02, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ0 + 3, DA_386IGate, hwint03, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_IRQ0 + 4, DA_386IGate, hwint04, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ0 + 5, DA_386IGate, hwint05, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ0 + 6, DA_386IGate, hwint06, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ0 + 7, DA_386IGate, hwint07, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_IRQ8 + 0, DA_386IGate, hwint08, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ8 + 1, DA_386IGate, hwint09, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ8 + 2, DA_386IGate, hwint10, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ8 + 3, DA_386IGate, hwint11, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ8 + 4, DA_386IGate, hwint12, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ8 + 5, DA_386IGate, hwint13, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ8 + 6, DA_386IGate, hwint14, PRIVILEGE_KRNL);
        initIdtDesc(INT_VECTOR_IRQ8 + 7, DA_386IGate, hwint15, PRIVILEGE_KRNL);
	initIdtDesc(INT_VECTOR_SYSCALL, DA_386IGate, syscall, PRIVILEGE_USER);
	memset(&tss, 0, sizeof(Tss));
	tss.ss0 = SELECTOR_KERNEL_DS;

	initDescriptor(&gdt[INDEX_TSS],
			vir2phys(seg2Phys(SELECTOR_KERNEL_DS), &tss),
			sizeof(Tss), DA_386TSS);
	
	int i;
	Process* p = procTable;
	t_2 selectorLdt = SELECTOR_LDT_FIRST;
	for(i = 0; i < NR_TASKS + NR_PROCS; i++){
		initDescriptor(&gdt[selectorLdt >> 3], 
				vir2phys(seg2Phys(SELECTOR_KERNEL_DS),procTable[i].ldts),
				LDT_SIZE*sizeof(Descriptor), DA_LDT);
		p++;
		selectorLdt += 1 << 3;
	}
	setInterrupt();
}

void setInterrupt(){
	// keyboard
        outByte(INT_M_CTLMASK, 0xfd);
        outByte(INT_S_CTLMASK, 0xff);
	// clock
        outByte(INT_M_CTLMASK, 0xfe);
}

void initDescriptor(Descriptor* p, t_4 base, t_4 limit, t_2 attr){
	p->limit_low = limit & 0x0ffff;
	p->base_low = base & 0x0ffff;
	p->base_mid = (base>>16) & 0x0ff;
	p->attr1 = attr & 0xff;
	p->limit_high_attr2 = ((limit>>16)&0x0f) | ((attr>>8) & 0xf0);
	p->base_high = (base>>24) & 0x0ff;  
}

//get phyAddr by seg
t_4 seg2Phys(t_2 seg){
	Descriptor* p = &gdt[seg >> 3];
	return (p->base_high<<24) | (p->base_mid<<16) | (p->base_low);
}

void initIdtDesc(t_1 vector, t_1 type, void* handlerAddr, t_1 privilege){
	Gate* g = &idt[vector];
	t_4 base = (t_4)handlerAddr;
	g->offset_low = base & 0xffff;
	g->selector = SELECTOR_KERNEL_CS;
	g->dcount = 0;
	g->attr = type | (privilege << 5);
	g->offset_high = (base >> 16) & 0xffff;
}

void init8259A(){
        outByte(INT_M_CTL, 0x11);
        outByte(INT_S_CTL, 0x11);
        outByte(INT_M_CTLMASK, INT_VECTOR_IRQ0);
        outByte(INT_S_CTLMASK, INT_VECTOR_IRQ8);
        outByte(INT_M_CTLMASK, 0x4);
        outByte(INT_S_CTLMASK, 0x2);
        outByte(INT_M_CTLMASK, 0x1);
        outByte(INT_S_CTLMASK, 0x1);
        //shut down interruption
        outByte(INT_M_CTLMASK, 0xff);
        outByte(INT_S_CTLMASK, 0xff);

	int i;
	for(i = 0; i < NR_IRQ; i++){
		setIrq(i, outIntHandler, 0);
	}

}

void outIntHandler(t_4 irq){
	dispStr("spurious_irq: ");
	dispNum4(irq);
	dispStr("\n");
}

void setIrq(t_4 irq, void* handler, t_4 ifEnable){
	if(irq < 8){
		outByte(INT_M_CTLMASK, inByte(INT_M_CTLMASK) | (1 << irq));
	}
	else{
		outByte(INT_S_CTLMASK, inByte(INT_S_CTLMASK) | (1 << irq));
	}
	irqTable[irq] = handler;
	if(ifEnable != 1) return;
	if(irq < 8){
                outByte(INT_M_CTLMASK, inByte(INT_M_CTLMASK) & ~(1 << irq));
        }
        else{
                outByte(INT_S_CTLMASK, inByte(INT_S_CTLMASK) & ~(1 << irq));
        }
}

void exceptionHandler(t_4 vecNo, t_4 errCode, t_4 eip, t_4 cs, t_4 eflags){
	char errDescription[][64] = {
		"#DE Divide Error",
		"#DB Reserved",
		"#-- NMI Interrupt",
		"#BP Breakpoint",
		"#OF Overflow",
		"#BR Bound Range Exceeded",
		"#UD Invalid Opcode",
		"#NM Device unavailable",
		"#DF Double Fault",
		"#-- Copr Seg Over (reserved)",
		"#TS Invalid Tss",
		"#NP Seg Not Present",
		"#SS Stack Seg Fault",
		"#GP General Protection",
		"#PF Page Fault",
		"#-- (Intel Reserved)",
		"#MF x87 FPU Error",
		"#AC Alignment Check",
		"#MC Machine Check",
		"#XF SIMD FloatingPoint Exception"
	};
	dispStr(errDescription[vecNo]);
	dispStr("\n");
	dispStr("Code: ");
	dispNum4(vecNo);
	dispStr("\n");
}
