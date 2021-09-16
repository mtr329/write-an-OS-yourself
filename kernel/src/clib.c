#include "global.h"

void strCopy(char* to, char* from){
	while(1){
		*to = *from;
		if(*from == '\0') break;
		to++;
		from++;
	}
}

t_4 strLength(char* str){
	t_4 ret = 0;
	while(*str != '\0'){
		str++;
		ret++;
	}
	return ret;
}

void numToHex(char buf[], int num){
	int i;
	buf[0] = '0';
	buf[1] = 'x';
	for(i = 7; i >= 0; i--){
		char c = ((char)num) & 0x0f;
		if(c <= 9){c += '0';}
		else{c += 'A' - 10;}
		buf[i+2] = c;
		num = num >> 4;
	}
	buf[10] = '\0';
}

void delay(int time){	
	int i = 0, j = 0, k = 0;
	int t = 10000;
	for(k = 0; k < time; k++){
		for(i = 0; i < t; i++){
			for(j = 0; j < t; j++){}
		}
	}
}

void milliDelay(t_4 milliSec){
	int unit = 100000000;
	int i = unit;
	t_4 ticks1 = getTicks();
	while(i > 0){i--;}
	t_4 ticks2 = getTicks();
	t_4 millis = (ticks2 - ticks1) * 1000 / HZ;
	int loop = (int)((double)unit / millis * milliSec);
	loop -= unit;
	while(loop > 0){
		loop--;
	}
}

int sysWrite(char* buf, int len, Process* p_proc){
	char* p = buf;
	int i = len;
	Tty* p_tty = &ttyTable[p_proc->nrTty];
	while(i){
		outChar(p_tty->p_console, *p++);
		i--;
	}
	return 0;
}

t_4 sysGetTicks(){
	// dispStr("+");
	// dispNum4(ticks);
	return ticks;
}

void schedule(){
	Process* p;
	t_4 maxTicks = 0;
	while(maxTicks == 0){
		for(p = procTable; p < procTable + NR_TASKS + NR_PROCS; p++){
			if(p->ticks > maxTicks){
				maxTicks = p->ticks;
				procReadyPtr = p;
			}	
		}
		if(maxTicks == 0){
			for(p = procTable; p < procTable + NR_TASKS + NR_PROCS; p++){
				p->ticks = p->priority;
			}
		}
	}
}
