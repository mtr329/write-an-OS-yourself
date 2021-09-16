#define GDT_SIZE 128
#define IDT_SIZE 256
#define LDT_SIZE 2
/* GDT */
/* 描述符索引 */
#define INDEX_DUMMY             0       // ┓
#define INDEX_FLAT_C            1       // ┣ LOADER 里面已经确定了的.
#define INDEX_FLAT_RW           2       // ┃
#define INDEX_VIDEO             3       // ┛
#define	INDEX_TSS		4
#define	INDEX_LDT_FIRST		5
/* 选择子 */
#define SELECTOR_DUMMY             0            // ┓
#define SELECTOR_FLAT_C         0x08            // ┣ LOADER 里面已经确定了的.
#define SELECTOR_FLAT_RW        0x10            // ┃
#define SELECTOR_VIDEO          (0x18+3)        // ┛<-- RPL=3
#define	SELECTOR_TSS		0x20		// TSS. 从外层跳到内存时SS和ESP的值从里面获得.
#define SELECTOR_LDT_FIRST	0x28

#define SELECTOR_KERNEL_CS      SELECTOR_FLAT_C
#define SELECTOR_KERNEL_DS      SELECTOR_FLAT_RW
#define	SELECTOR_KERNEL_GS	SELECTOR_VIDEO

#define NR_TASKS		1
#define NR_PROCS		3
#define STACK_SIZE_TESTA	0x8000
#define STACK_SIZE_TESTB	0x8000
#define STACK_SIZE_TESTC	0x8000
#define STACK_SIZE_TTY		0x8000
#define STACK_SIZE_TOTAL	(STACK_SIZE_TESTA+STACK_SIZE_TESTB+STACK_SIZE_TESTC+STACK_SIZE_TTY)

#define NR_CONSOLES		3
#define DEFAULT_CHAR_COLOR	0x07
#define SCROLL_SCREEN_UP	1	/* scroll forward */
#define SCROLL_SCREEN_DOWN	-1	/* scroll backward */
#define SCREEN_SIZE		(80 * 25)
#define SCREEN_WIDTH		80
