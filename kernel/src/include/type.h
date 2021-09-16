#define KB_IN_BYTES	32
#define TTY_IN_BYTES	256

typedef unsigned int    t_4;
typedef unsigned short  t_2;
typedef unsigned char   t_1;

typedef struct s_descriptor{
        t_2     limit_low;
        t_2     base_low;
        t_1     base_mid;
        t_1     attr1;
        t_1     limit_high_attr2;
        t_1     base_high;
}Descriptor;

typedef struct s_gate{
        t_2     offset_low;
        t_2     selector;
        t_1     dcount;
        t_1     attr;
        t_2     offset_high;
}Gate;

typedef struct s_stackframe{
	t_4	gs;
	t_4	fs;
	t_4	es;
	t_4	ds;
	t_4	edi;
	t_4	esi;
	t_4	ebp;
	t_4	kernel_esp;
	t_4	ebx;
	t_4	edx;
	t_4	ecx;
	t_4	eax;
	t_4	retAddr;
	t_4	eip;
	t_4	cs;
	t_4	eflags;
	t_4	esp;
	t_4	ss;
}StackFrame;

typedef struct  s_proc{
	StackFrame	regs;
	t_2		ldtSel;
	Descriptor	ldts[LDT_SIZE];
	t_4		ticks;
	t_4		priority;
	t_4		pid;
	char		p_name[16];
	int		nrTty;
}Process;

typedef struct s_tss{
	t_4	backlink;
	t_4	esp0;
	t_4	ss0;
	t_4	esp1;
	t_4	bss1;
	t_4	eesp2;
	t_4	sss2;
	t_4	ecr3;
	t_4	eip;
	t_4	flags;
	t_4	eax;
	t_4	ecx;
	t_4	edx;
	t_4	ebx;
	t_4	esp;
	t_4	ebp;
	t_4	esi;
	t_4	edi;
	t_4	es;
	t_4	cs;
	t_4	ss;
	t_4	ds;
	t_4	fs;
	t_4	gs;
	t_4	ldt;
	t_2	trap;
	t_2	iobase;
}Tss;

typedef struct s_task{
	t_4	initialEip;
	t_4	stackSize;
	char	name[32];
}Task;

typedef struct s_kb{
	char*	p_head;
	char*	p_tail;
	int	count;
	char 	buf[KB_IN_BYTES];
}KbInput;

typedef struct s_console{
	t_4	current_start_addr;
	t_4	original_addr;
	t_4	v_mem_limit;
	t_4	cursor;	
}Console;

typedef struct s_tty{
	t_4	in_buf[TTY_IN_BYTES];
	t_4*	p_inbuf_head;
	t_4*	p_inbuf_tail;
	int	inbuf_count;
	Console* p_console;
}Tty;

//klib.asm
void restart();
//util.asm
void dispNum4(t_4 num);
void memCopy(void* from_4, void* to_4, t_4 num_4);
void dispStr(void* from_4);
t_4 inByte(t_4 port);
void outByte(t_4 port, t_1 byte);
void memset(void* addr, t_4 byte, t_4 size);
void disableInt();
void enableInt();
// syscall.asm
t_4 getTicks();
void write(char* buf, int len);
// interrupt.asm
void syscall();
// protect.c
void exceptionHandler(t_4 vecNo, t_4 errCode, t_4 eip, t_4 cs, t_4 eflags);
void outIntHandler(t_4 irq);
void init8259A();
void initIdtDesc(t_1 vector, t_1 type, void* handlerAddr, t_1 privilege);
void initPort();
void initDescriptor(Descriptor* p, t_4 base, t_4 limit, t_2 attr);
t_4 seg2Phys(t_2 reg);
void setInterrupt();
void setIrq(t_4 irq, void* handler, t_4 ifEnable);
// start.c
void cinit();
void initGdt();
void initIdt();
// clib.c
void strCopy(char* to, char* from);
t_4 strLength(char* str);
void numToHex(char buf[], int num);
void delay(int time);
int sysWrite(char* buf, int len, Process* p_proc);
t_4 sysGetTicks();
void milliDelay(t_4 milliSec);
void schedule();
// main.c
int minuxMain();
void testA();
void testB();
void testC();
// clock.c
void clockHandler(int irq);
// keyboard.c
void keyboardHandler(int irq);
void keyboardRead(Tty* p_tty);
void inProcess(Tty* p_tty, t_4 key);
t_1 getByteFromKbBuf();
void initKeyboard();
// tty.c
void taskTty();
void ttyDoRead(Tty* p);
void ttyDoWrite(Tty* p);
// console.c
void outChar(Console* p_con, char ch);
void selectConsole(int nrConsole);
void scrollScreen(Console* p_con, int direction);
void setCursor(t_4 posi);
int isCurrentConsole(Console* p);
void setVideoStartAddr(t_4 addr);
void flush(Console* p_con);
// print.c
int printf(const char*fmt, ...);
int vsprintf(char* buf, const char* fmt, char* args);
