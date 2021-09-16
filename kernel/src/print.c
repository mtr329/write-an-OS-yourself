#include "global.h"

int printf(const char*fmt, ...){
	int i;
	char buf[256] = {'\0'};
	char* arg = (char*)((char*)(&fmt) + sizeof(fmt));
	i = vsprintf(buf, fmt, arg);
	write(buf, i);
	return i;
}

int vsprintf(char* buf, const char* fmt, char* args){
	char* p;
	char tmp[256] = {'\0'};
	char* pNextArg = args;
	for(p = buf; *fmt; fmt++){
		if(*fmt != '%'){
			*p++ = *fmt;
			continue;
		}
		fmt++;
		switch(*fmt){
			case 'x':
				numToHex(tmp, *((int*)pNextArg));
				strCopy(p, tmp);
				pNextArg += 4;
				p += strLength(tmp);
				break;
			default:
				break;
		}
	}
	return (p - buf);
}
