#include "global.h"

void clockHandler(int irq){
	ticks++;
	procReadyPtr->ticks -= (t_4)1;
//	dispNum4(ticks);
//	dispStr("\n");
	
	if(kReenter != 0) return;
	
	if(procReadyPtr->ticks != 0) return;

	schedule();
}
