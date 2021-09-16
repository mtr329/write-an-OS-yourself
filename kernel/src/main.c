#include "global.h"

int minuxMain(){
	dispStr("minuxMain begins\n");
	t_1 privilege;
	t_1 rpl;
	int eflags;
	kReenter = 0;
	ticks = (t_4)0;
	Process* p = procTable;
	Task* pTask = taskTable;
	char* pTaskStack = taskStack + STACK_SIZE_TOTAL;
	t_2 selectorLdt = SELECTOR_LDT_FIRST;
	int i;
	for(i = 0; i < NR_TASKS + NR_PROCS; i++){
		if(i < NR_TASKS){
			pTask = taskTable + i;
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202;
		}
		else{
			pTask = userProcTable + i - NR_TASKS;
                        privilege = PRIVILEGE_USER;
                        rpl = RPL_USER;
                        eflags = 0x202;
		}
		strCopy((void*)(p->p_name), (void*)(pTask->name));
		p->pid = i;
		p->ldtSel = selectorLdt;
		memCopy(&p->ldts[0], &gdt[SELECTOR_KERNEL_CS>>3], (t_4)sizeof(Descriptor));
		p->ldts[0].attr1 = DA_C | (privilege << 5);
		memCopy(&p->ldts[1], &gdt[SELECTOR_KERNEL_DS>>3], (t_4)sizeof(Descriptor));
		p->ldts[1].attr1 = DA_DRW | (privilege << 5);
		p->regs.cs = ((8*0) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p->regs.ds = ((8*1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p->regs.es = ((8*1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p->regs.fs = ((8*1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p->regs.ss = ((8*1) & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;
		p->regs.eip = (t_4)(pTask->initialEip);
		p->regs.esp = (t_4)(pTaskStack);
		p->regs.eflags = eflags;
		p->nrTty = 0;
		pTaskStack -= pTask->stackSize;
		p++;
		pTask++;
		selectorLdt += 1 << 3;
	}
	procReadyPtr =  procTable;

	procTable[0].ticks = procTable[0].priority = 15;
	procTable[1].ticks = procTable[1].priority = 5;	
	procTable[2].ticks = procTable[2].priority = 5;
	procTable[3].ticks = procTable[3].priority = 5;

	procTable[1].nrTty = 0;
	procTable[2].nrTty = 1;
	procTable[3].nrTty = 1;

	// set i8253
	outByte(TIMER_MODE, RATE_GENERATOR);
	outByte(TIMER0, (t_1)(TIMER_FREQ/HZ));
	outByte(TIMER0, (t_1)((TIMER_FREQ/HZ) >> 8));

	// set clock
	setIrq(CLOCK_IRQ, clockHandler, 1);

	//set keyboard
	initKeyboard();

	restart();

	while(1){}
	return 0;
}

void testA(){
	while(1){
		printf(" [Ticks: %x] ", getTicks());
		milliDelay(1000);
//		delay(1);
	}	
}

void testB(){
        while(1){
		printf("B");
                milliDelay(1000);
//		delay(1);
        }
}

void testC(){
	while(1){
		printf("C");
		milliDelay(1000);
//		delay(1);		
	}
}
