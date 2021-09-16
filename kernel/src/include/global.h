#include "const.h"
#include "sys.h"
#include "type.h"
#include "kb.h"

extern t_1 		dispPos[];

extern t_1 		gdtPtr[];
extern Descriptor	gdt[];

extern t_1		idtPtr[];
extern Gate		idt[];
extern Process		procTable[];
extern Process*		procReadyPtr;
extern Tss    		tss;
extern char		taskStack[];
extern int 		kReenter;
extern Task		taskTable[];
extern Task		userProcTable[];
extern void*		irqTable[];
extern void*		syscallTable[];
extern t_4		ticks;
extern t_4		keymap[];
extern KbInput		kbInput;
extern Tty		ttyTable[];
extern Console		consoleTable[];
extern int		nrCurrentConsole;
