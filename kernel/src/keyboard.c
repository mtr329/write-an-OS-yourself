#include "global.h"

int codeWithE0 = 0;
int shiftL;
int shiftR;
int altL;
int altR;
int ctrlL;
int ctrlR;
int column = 0;
int capsLock = 0;
int numLock = 0;
int scrollLock = 0;

void kbWait();
void kbAck();
void setLeds();

void initKeyboard(){
	kbInput.count = 0;
        kbInput.p_head = kbInput.p_tail = kbInput.buf;
	capsLock = 0;
	numLock = 1;
	scrollLock = 0;
	setLeds();
        setIrq(KEYBOARD_IRQ, keyboardHandler, 1);

}

void kbWait(){
	t_1 kbStat;
	do{
		kbStat = inByte(KB_CMD);
	}while(kbStat & 0x02);
}

void kbAck(){
	t_1 kbRead;
	do{
		kbRead = inByte(KB_DATA);
	}while(kbRead != KB_ACK);
}

void setLeds(){
	t_1 leds = (capsLock << 2) | (numLock << 1) | (scrollLock);
	kbWait();
	outByte(KB_DATA, LED_CODE);
	kbAck();
	kbWait();
	outByte(KB_DATA, leds);
	kbAck();
}

void keyboardRead(Tty* p_tty){
	t_1 scanCode;
	int make;
	t_4 key = 0;
	t_4* keyrow;

	if(kbInput.count <= 0) return;

	codeWithE0 = 0;
	scanCode = getByteFromKbBuf();
	if(scanCode == 0xe1){
		int i;
		t_1 pausebreak_scan_code[] = {0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5};
		int is_pausebreak = 1;
		for(i=1;i<6;i++){
			if (getByteFromKbBuf() != pausebreak_scan_code[i]) {
				is_pausebreak = 0;
				break;
			}
		}
		if (is_pausebreak) {
			key = PAUSEBREAK;
		}
	}
	else if(scanCode == 0xe0){
		scanCode = getByteFromKbBuf();
		/* PrintScreen 被按下 */
		if (scanCode == 0x2A) {
			if (getByteFromKbBuf() == 0xE0) {
				if (getByteFromKbBuf() == 0x37) {
					key = PRINTSCREEN;
					make = 1;
				}
			}
		}

		/* PrintScreen 被释放 */
		if (scanCode == 0xB7) {
			if (getByteFromKbBuf() == 0xE0) {
				if (getByteFromKbBuf() == 0xAA) {
					key = PRINTSCREEN;
					make = 0;
				}
			}
		}

		/* 不是 PrintScreen。此时 scan_code 为 0xE0 紧跟的那个值。 */
		if (key == 0) {
			codeWithE0 = 1;
		}
	}
	if ((key != PAUSEBREAK) && (key != PRINTSCREEN)) {
		/* 首先判断Make Code 还是 Break Code */
		make = (scanCode & FLAG_BREAK ? 0 : 1);

		/* 先定位到 keymap 中的行 */
		keyrow = &keymap[(scanCode & 0x7F) * MAP_COLS];

		column = 0;

		if (shiftL || shiftR) {
			column = 1;
		}

		if (codeWithE0) {
			column = 2;
		}

		key = keyrow[column];

		switch(key) {
		case CAPS_LOCK:
			if(make){
				capsLock = !capsLock;
				setLeds();
			}
			break;
		case NUM_LOCK:
			if(make){
				numLock = !numLock;
				setLeds();	
			}
			break;

		case SCROLL_LOCK:
			if(make){
				scrollLock = !scrollLock;
				setLeds();
			}
			break;
		case SHIFT_L:
			shiftL	= make;
			break;
		case SHIFT_R:
			shiftR	= make;
			break;
		case CTRL_L:
			ctrlL	= make;
			break;
		case CTRL_R:
			ctrlR	= make;
			break;
		case ALT_L:
			altL	= make;
			break;
		case ALT_R:
			altR	= make;
			break;
		default:
			break;
		}
	}

	if(make){ /* 忽略 Break Code */
		int pad = 0;
		if((key >= PAD_SLASH) && key <= PAD_9){
			pad = 1;
			switch(key){
			case PAD_SLASH:
				key = '/';
				break;
			case PAD_STAR:
				key = '*';
				break;
			case PAD_MINUS:
				key = '-';
				break;
			case PAD_PLUS:
				key = '+';
				break;
			case PAD_ENTER:
				key = ENTER;
				break;
			default:
				if(numLock){
					if((key >= PAD_0) && (key <= PAD_9)){
						key = key - PAD_0 + '0';
					}
					else if(key == PAD_DOT){
						key = '.';
					}
				}
				else{
					switch(key){
						case PAD_HOME:
							key = HOME;
							break;
						case PAD_END:
							key = END;
							break;
						case PAD_PAGEUP:
							key = PAGEUP;
							break;
						case PAD_PAGEDOWN:
							key = PAGEDOWN;
							break;
						case PAD_INS:
							key = INSERT;
							break;
						case PAD_UP:
							key = UP;
							break;
						case PAD_DOWN:
							key = DOWN;
							break;
						case PAD_LEFT:
							key = LEFT;
							break;
						case PAD_RIGHT:
							key = RIGHT;
							break;
						case PAD_DOT:
							key = DELETE;
							break;
						default:
							break;
					}
				}
				break;
			}
		}
		key |= shiftL	? FLAG_SHIFT_L	: 0;
		key |= shiftR	? FLAG_SHIFT_R	: 0;
		key |= ctrlL	? FLAG_CTRL_L	: 0;
		key |= ctrlR	? FLAG_CTRL_R	: 0;
		key |= altL	? FLAG_ALT_L	: 0;
		key |= altR	? FLAG_ALT_R	: 0;
		key |= pad	? FLAG_PAD	: 0;
		inProcess(p_tty, key);
	}

}

void keyboardHandler(int irq){
	t_1 scanCode = inByte(KB_DATA);
	if(kbInput.count < KB_IN_BYTES){
		*(kbInput.p_head) = scanCode;
		kbInput.p_head++;
		if(kbInput.p_head == kbInput.buf + KB_IN_BYTES){
			kbInput.p_head = kbInput.buf;
		}
		kbInput.count++;
	}
}

t_1 getByteFromKbBuf(){
	t_1 scanCode;
	while(kbInput.count <= 0){}
	disableInt();
	scanCode = *(kbInput.p_tail);
	kbInput.p_tail++;
	if(kbInput.p_tail == kbInput.buf + KB_IN_BYTES){
		kbInput.p_tail = kbInput.buf;
	}
	kbInput.count--;
	enableInt();
	return scanCode;
}
