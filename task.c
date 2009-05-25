#include <task.h>
#include <io.h>
#include <timer.h>

struct task_struct *current = NULL;
struct task_struct task_init = {
	.tss = {
		.link = 0,
		.esp0 = (uint32)0x8000,
		.ss0 = SEL_DATA,
		.esp1 = 0,
		.ss1 = 0,
		.esp2 = 0x6000,
		.ss2 = SEL_USER_DATA,
		.cr3 = 0,
		.eip = 0,
		.eflags = 0,
		.eax = 0,
		.ecx = 0,
		.edx = 0,
		.ebx = 0,
		.esp = 0,
		.ebp = 0,
		.esi = 0,
		.edi = 0,
		.es = SEL_VIDEO,
		.cs = SEL_USER_CODE,
		.ss = SEL_USER_DATA,
		.ds = SEL_USER_DATA,
		.fs = SEL_USER_DATA,
		.gs = SEL_USER_DATA,
		.ldt_selector = SEL_CUR_LDT,
		.t = 0,
		.io_map = 0
	},

	.ldt = 	{
		0x00c0fa0000008fffULL, 0x00c0f20000008fffULL 
	},

	.state = TASK_RUNNING,
	.priority = DEFAULT_PRIO,
	
	.next = NULL
};

struct task_struct task1 = {
	.ldt = 	{
		0x00c0fa0000008fffULL, 0x00c0f20000008fffULL 
	},

	.state = TASK_DEAD,
	.priority = DEFAULT_PRIO,
	
	.next = NULL
};
	
struct task_struct task2 = {
	.ldt = 	{
		0x00c0fa0000008fffULL, 0x00c0f20000008fffULL 
	},

	.state = TASK_DEAD,
	.priority = DEFAULT_PRIO,
	
	.next = NULL
};

void do_init_task()
{
	__asm__ __volatile__ (
		"mov $0xf, %%eax   \n\t"
		"mov %%eax, %%ds \n\t"
		"mov %%eax, %%fs \n\t"
		"mov %%eax, %%gs \n\t"
		"mov $0x20, %%eax \n\t"
		"mov %%eax, %%es \n\t"
		:::"eax"
		);

	char wheel[] = {'\\', '|', '/', '-'};
	int i = 0;
 
	for (;;) {
		__asm__ ("movb  %%al,   0xb8000+160*24"::"a"(wheel[i]));
		if (i == sizeof wheel)
			i = 0;
		else
			++i;
	}
}

void new_task(struct task_struct *task, void (*entry)(), uint32 esp0, uint32 esp3 )
{
	if ( task == NULL )
		return;
	
	memcpy( task, &task_init, sizeof task_init );
	task->state = TASK_DEAD;	
	task->tss.eip = (uint32)entry;
	task->tss.esp0 = esp0;
	task->tss.esp = esp3;
	task->tss.eflags = 0x3202;
	task->priority = DEFAULT_PRIO;


	task->next = current->next;
	current->next = task;
	task->state = TASK_READY;
}

void scheduler()
{
//	early_kprint( PL_DEBUG, "scheduler\n" );
	
	struct task_struct *l = &task_init;
	struct task_struct *next_run = NULL;
	int max_prio = 0;

	do {
		if ( l->state == TASK_DEAD )
			continue;

		if ( l->state == TASK_READY ) {
			if ( l->priority > max_prio ) {
				max_prio = l->priority;
				next_run = l;
			} 
		}

		l = l->next;
	} while ( l != NULL );

	if ( max_prio == 0 ) {
		int x =0, y = 0;
		get_cursor( &x, &y );
		set_cursor( 5, VIDEO_ROWS-1 );
		early_kprint ( PL_DEBUG, "scheduler: recount priority" );
		// busy delay here
//		set_cursor( 5, VIDEO_ROWS-1 );		
//		early_kprint ( PL_DEBUG, "                           " );
		
		for ( l = &task_init; l != NULL; l = l->next ) {
			l->priority = DEFAULT_PRIO;
		}

		set_cursor( x, y );
	}

	if ( next_run == NULL )
		next_run = &task_init;
	
/*
	int n = 0;
	while ( l != NULL ) {
		n++;
		l = l->next;
	}
	early_kprint( PL_DEBUG, "total %d tasks\n", n );

	if ( task_init.next != NULL ) {
		early_kprint( PL_DEBUG, "next task\n" );
		next_run = task_init.next;
	}
*/
	
	next_run->priority--;

	setup_gdt_entry((SEL_CUR_TSS>>3), (uint32)&next_run->tss, 0x67UL, (F_USER32_TSS));
	setup_gdt_entry((SEL_CUR_LDT>>3), (uint32)&next_run->ldt, 0xfUL, (F_USER32_LDT));

	current->state = TASK_READY;	
	next_run->state = TASK_RUNNING;	
	current = next_run;

	/* __asm__ ( "ltrw %%ax \n\t" ::"a"(SEL_CUR_TSS) ); */
	/* __asm__ ( "lldt %%ax \n\t" ::"a"(SEL_CUR_LDT) ); */

	__asm__ __volatile__ (
		"ljmp %0, $0 \n\t"
		::"i"(SEL_CUR_TSS)
		);
}
