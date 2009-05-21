#ifndef _task_h
#define _task_h

#include <kernel.h>

struct tss_struct {
	uint32 link;

	uint32 esp0;
	uint32 ss0;

	uint32 esp1;
	uint32 ss1;

	uint32 esp2;
	uint32 ss2;

	uint32 cr3;
	uint32 eip;
	uint32 eflags;
	uint32 eax,  ecx, edx, ebx, esp, ebp, esi, edi;
	uint32 es, cs, ss, ds, fs, gs;

	uint32 ldt_selector;

	uint16 t;  // one bit, use bitfield?
	uint16 io_map;
};

struct task_struct {
	struct tss_struct tss;
	unsigned long long tss_descriptor;

	unsigned long long ldt[2];
	unsigned long long ldt_descriptor;

	int state;
	int priority;
	
	struct task_struct *next;
};

#define TASK_READY         0
#define TASK_RUNNING       1
#define TASK_DEAD          2


#endif
