#include "global.h"

void outChar(Console* p_con, char ch){
	t_1* p_vmem = (t_1*)(V_MEM_BASE + p_con->cursor * 2);
	switch(ch){
	case '\n':{
		if(p_con->cursor < p_con->original_addr+p_con->v_mem_limit-SCREEN_WIDTH){
			p_con->cursor = p_con->original_addr + SCREEN_WIDTH * ((p_con->cursor - p_con->original_addr)/SCREEN_WIDTH + 1);
		}	
		break;
	}
	case '\b':{
		if(p_con->cursor > p_con->original_addr){
			p_con->cursor--;
			*(p_vmem-2) = ' ';
			*(p_vmem-1) = DEFAULT_CHAR_COLOR;	
		}
		break;
	}
	default:{
		if(p_con->cursor < p_con->original_addr + p_con->v_mem_limit - 1){
			*p_vmem++ = ch;
			*p_vmem++ = DEFAULT_CHAR_COLOR;
			p_con->cursor++;
		}
		break;	
	}
	}

	while(p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE){
		scrollScreen(p_con, SCROLL_SCREEN_DOWN);
	}

	setCursor(p_con->cursor);
}

void flush(Console* p_con){
	setCursor(p_con->cursor);
	setVideoStartAddr(p_con->current_start_addr);
}

void selectConsole(int nrConsole){
	if(nrConsole < 0 || nrConsole >= NR_CONSOLES){
		
		return;
	}
	nrCurrentConsole = nrConsole;
	flush(&consoleTable[nrConsole]);
}

void scrollScreen(Console* p_con, int direction){
	if(direction == SCROLL_SCREEN_UP){
		if(p_con->current_start_addr > p_con->original_addr){
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if(direction == SCROLL_SCREEN_DOWN){
		if(p_con->current_start_addr + SCREEN_SIZE < 
				p_con->original_addr + p_con->v_mem_limit){
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	else{}
	setVideoStartAddr(p_con->current_start_addr);
	setCursor(p_con->cursor);
}

void setCursor(t_4 posi){
	disableInt();
        outByte(CRTC_ADDR_REG, CRTC_DATA_IDX_CURSOR_H);
        outByte(CRTC_DATA_REG, (posi >> 8) & 0xff);
        outByte(CRTC_ADDR_REG, CRTC_DATA_IDX_CURSOR_L);
        outByte(CRTC_DATA_REG, posi & 0xff);
        enableInt();
}

void setVideoStartAddr(t_4 addr){
	disableInt();
	outByte(CRTC_ADDR_REG, CRTC_DATA_IDX_START_ADDR_H);
	outByte(CRTC_DATA_REG, (addr>>8) & 0xff);
	outByte(CRTC_ADDR_REG, CRTC_DATA_IDX_START_ADDR_L);
	outByte(CRTC_DATA_REG, addr & 0xff);
	enableInt();
}

int isCurrentConsole(Console* p){
	return (p == &consoleTable[nrCurrentConsole]);
}
