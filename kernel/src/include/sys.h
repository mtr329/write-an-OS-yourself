//8259A
#define INT_M_CTL	0x20
#define INT_M_CTLMASK	0x21
#define INT_S_CTL	0xa0
#define INT_S_CTLMASK	0xa1

/* 描述符类型值说明 */
#define DA_32                   0x4000  /* 32 位段 */
#define DA_LIMIT_4K             0x8000  /* 段界限粒度为 4K 字节 */
#define DA_DPL0                 0x00    /* DPL = 0 */
#define DA_DPL1                 0x20    /* DPL = 1 */
#define DA_DPL2                 0x40    /* DPL = 2 */
#define DA_DPL3                 0x60    /* DPL = 3 */

/* 存储段描述符类型值说明 */
#define DA_DR                   0x90    /* 存在的只读数据段类型值 */
#define DA_DRW                  0x92    /* 存在的可读写数据段属性值 */
#define DA_DRWA                 0x93    /* 存在的已访问可读写数据段类型值 */
#define DA_C                    0x98    /* 存在的只执行代码段属性值 */
#define DA_CR                   0x9A    /* 存在的可执行可读代码段属性值 */
#define DA_CCO                  0x9C    /* 存在的只执行一致代码段属性值 */
#define DA_CCOR                 0x9E    /* 存在的可执行可读一致代码段属性值 */

/* 系统段描述符类型值说明 */
#define DA_LDT                  0x82    /* 局部描述符表段类型值 */
#define DA_TaskGate             0x85    /* 任务门类型值 */
#define DA_386TSS               0x89    /* 可用 386 任务状态段类型值 */
#define DA_386CGate             0x8C    /* 386 调用门类型值 */
#define DA_386IGate             0x8E    /* 386 中断门类型值 */
#define DA_386TGate             0x8F    /* 386 陷阱门类型值 */

/* 选择子类型值说明 */
/* 其中, SA_ : Selector Attribute */
#define	SA_RPL_MASK	0xFFFC
#define	SA_RPL0		0
#define	SA_RPL1		1
#define	SA_RPL2		2
#define	SA_RPL3		3

#define	SA_TI_MASK	0xFFFB
#define	SA_TIG		0
#define	SA_TIL		4

// interrupt vector
#define INT_VECTOR_DIVIDE       0x0
#define INT_VECTOR_DEBUG        0x1
#define INT_VECTOR_NMI          0x2
#define INT_VECTOR_BREAKPOINT   0x3
#define INT_VECTOR_OVERFLOW     0x4
#define INT_VECTOR_BOUNDS       0x5
#define INT_VECTOR_INVAL_OP     0x6
#define INT_VECTOR_COPROC_NOT   0x7
#define INT_VECTOR_DOUBLE_FAULT 0x8
#define INT_VECTOR_COPROC_SEG   0x9
#define INT_VECTOR_INVAL_TSS    0xA
#define INT_VECTOR_SEG_NOT      0xB
#define INT_VECTOR_STACK_FAULT  0xC
#define INT_VECTOR_PROTECTION   0xD
#define INT_VECTOR_PAGE_FAULT   0xE
#define INT_VECTOR_COPROC_ERR   0x10
#define INT_VECTOR_IRQ0         0x20
#define INT_VECTOR_IRQ8         0x28
#define INT_VECTOR_SYSCALL	0x90

// privilege
#define PRIVILEGE_KRNL  0
#define PRIVILEGE_TASK  1
#define PRIVILEGE_USER  3

// rpl
#define	RPL_KRNL	SA_RPL0
#define	RPL_TASK	SA_RPL1
#define	RPL_USER	SA_RPL3

// i8259a
#define	NR_IRQ		16	/* Number of IRQs */
#define	CLOCK_IRQ	0
#define	KEYBOARD_IRQ	1
#define	CASCADE_IRQ	2	/* cascade enable for 2nd AT controller */
#define	ETHER_IRQ	3	/* default ethernet interrupt vector */
#define	SECONDARY_IRQ	3	/* RS232 interrupt vector for port 2 */
#define	RS232_IRQ	4	/* RS232 interrupt vector for port 1 */
#define	XT_WINI_IRQ	5	/* xt winchester */
#define	FLOPPY_IRQ	6	/* floppy disk */
#define	PRINTER_IRQ	7
#define	AT_WINI_IRQ	14

// syscalls
#define NR_SYSCALL	2

// i8253
#define TIMER0		0x40
#define TIMER_MODE	0x43
#define RATE_GENERATOR	0x34
#define TIMER_FREQ	1193182L
#define HZ		100

// keyboard
#define KB_DATA		0x60
#define	KB_CMD		0x64
#define LED_CODE	0xED
#define KB_ACK		0xFA

// vga
#define CRTC_ADDR_REG			0x3D4	/* CRT Controller Registers - Address Register */
#define CRTC_DATA_REG			0x3D5	/* CRT Controller Registers - Data Registers */
#define CRTC_DATA_IDX_START_ADDR_H	0xC	/* register index of video mem start address (MSB) */
#define CRTC_DATA_IDX_START_ADDR_L	0xD	/* register index of video mem start address (LSB) */
#define CRTC_DATA_IDX_CURSOR_H		0xE	/* register index of cursor position (MSB) */
#define CRTC_DATA_IDX_CURSOR_L		0xF	/* register index of cursor position (LSB) */
#define V_MEM_BASE			0xB8000	/* base of color video memory */
#define V_MEM_SIZE			0x8000	/* 32K: B8000H -> BFFFFH */

// others
#define vir2phys(seg_base,vir) (t_4)(((t_4)(seg_base)) + (t_4)(vir))
