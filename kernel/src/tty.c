#include "global.h"

#define TTY_FIRST	(ttyTable)
#define TTY_END		(ttyTable + NR_CONSOLES)

void inProcess(Tty* p_tty, t_4 key);
void initTty(Tty* p);
void initScreen(Tty* p);
void putKey(Tty* p, t_4 key);

void inProcess(Tty* p_tty, t_4 key){
        char output[2] = {'\0'};
        if(!(key & FLAG_EXT)){
		putKey(p_tty, key);
        }
	else{
		int rawCode = key & MASK_RAW;
		switch(rawCode){
		case ENTER:{
			putKey(p_tty, 0x0a);
			break;
		}
		case BACKSPACE:{
			putKey(p_tty, '\b');
			break;
		}
		case UP:{
			if((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)){
				scrollScreen(p_tty->p_console, SCROLL_SCREEN_UP);
			}	
			break;
		}
		case DOWN:{
                        if((key & FLAG_SHIFT_L) || (key & FLAG_SHIFT_R)){
                        	scrollScreen(p_tty->p_console, SCROLL_SCREEN_DOWN);
                        }
			break;
                }
		case F1:
		case F2:
		case F3:
		case F4:
		case F5:
		case F6:
		case F7:
		case F8:
		case F9:
		case F10:
		case F11:
		case F12:{
			if ((key & FLAG_ALT_L) || (key & FLAG_ALT_R)) {	/* Alt + F1~F12 */
				selectConsole(rawCode - F1);
			}
			break;	 
		}
		default:
			break;
		}
	}
}

void putKey(Tty* p, t_4 key){
	if(p->inbuf_count < TTY_IN_BYTES){
		*(p->p_inbuf_head) = key;
		p->p_inbuf_head++;
		if(p->p_inbuf_head == p->in_buf + TTY_IN_BYTES){
			p->p_inbuf_head = p->in_buf;
		}
		p->inbuf_count++;
	}
}

void taskTty(){

	Tty* p_tty;
	for(p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++){
		initTty(p_tty);
	}
	selectConsole(0);

	while(1){
		for(p_tty = TTY_FIRST; p_tty < TTY_END; p_tty++){
			ttyDoRead(p_tty);
			ttyDoWrite(p_tty);
		}
	}
}

void ttyDoRead(Tty* p){
	if(isCurrentConsole(p->p_console)){
		keyboardRead(p);
	}
}

void ttyDoWrite(Tty* p){
	if(p->inbuf_count == 0) return;
	char ch = *(p->p_inbuf_tail);
	p->p_inbuf_tail++;
	if(p->p_inbuf_tail == p->in_buf + TTY_IN_BYTES){
		p->p_inbuf_tail = p->in_buf;
	}	
	p->inbuf_count--;
	outChar(p->p_console, ch);
}

void initTty(Tty* p){
        p->inbuf_count = 0;
        p->p_inbuf_head = p->p_inbuf_tail = p->in_buf;
	initScreen(p);
}

void initScreen(Tty* p){
	int nrTty = p - ttyTable;
	p->p_console = consoleTable + nrTty;
	int vMemSize = V_MEM_SIZE >> 1;
	int conVMemSize = vMemSize / NR_CONSOLES;
	p->p_console->original_addr = nrTty * conVMemSize;
	p->p_console->v_mem_limit = conVMemSize / SCREEN_WIDTH * SCREEN_WIDTH;
	p->p_console->current_start_addr = p->p_console->original_addr;
	p->p_console->cursor = p->p_console->original_addr;
	if(nrTty == 0){
		p->p_console->cursor = (*((t_4*)dispPos)) / 2;
	}
	else{
		outChar(p->p_console, nrTty + '0');
		outChar(p->p_console, '#');
	}
	setCursor(p->p_console->cursor);
}

