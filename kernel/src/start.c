#include "global.h"
/*
t_1 dispPos[4] = {0};

t_1 gdtPtr[6];
Descriptor gdt[GDT_SIZE];

t_1 idtPtr[6];
Gate idt[IDT_SIZE];
*/
void cinit(){
	initGdt();
	initIdt();
	initPort();	
}

void initGdt(){
	t_4 num = (t_4)(*(t_4*)(&gdtPtr[0]));
	num &= 0x0ffff;
	memCopy((void*)(&gdt), (void*)(*((t_4*)(&gdtPtr[2]))), num);
	t_2* limit = (t_2*)(&gdtPtr[0]);
	t_4* base = (t_4*)(&gdtPtr[2]);
	*limit = GDT_SIZE * sizeof(Descriptor);
	*base = (t_4)(&gdt);
}

void initIdt(){
	t_2* limit = (t_2*)(&idtPtr[0]);
	t_4* base = (t_4*)(&idtPtr[2]);
	*limit = IDT_SIZE * sizeof(Gate);
	*base = (t_4)(&idt);
}

