#include <task.h>
#include <io.h>
#include <timer.h>
#include <mm.h>

struct task_struct *current = NULL;
struct task_struct task_init = {
	.tss = {
		.link = 0,
		.esp0 = (uint32)0x6800,
		.ss0 = SEL_DATA,
		.esp1 = 0,
		.ss1 = 0,
		.esp2 = 0x6000,
		.ss2 = SEL_USER_DATA,
		.cr3 = PAGE_DIR_BASE,  // all tasks will copy this which is very important
//		.eip = (uint32)(do_init_task+18),
		.eip = (uint32)(do_init_task),		
//		.eip = 0,		
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
	
	.next = NULL,
	.sched_prev = &task_init,
	.sched_next = &task_init	
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
		"mov %%eax, %%ds   \n\t"
		"mov %%eax, %%fs   \n\t"
		"mov %%eax, %%gs   \n\t"
		"mov $0x20, %%eax  \n\t"
		"mov %%eax, %%es   \n\t"
		:::"eax"
		);

	char wheel[] = {'\\', '|', '/', '-'};
	int i = 0;

	// testing access an not-present page here
	int step = -10;
	for ( ; step < 10; step++ ) {
		uint32 *mem_addr = (uint32*)(0x2000000+step);
		early_kprint( PL_WARN, "%d: read mem_addr: %d\n", step, *mem_addr );
	}
	// end of test
	
	for (;;) {
		__asm__ ("movb  %%al,   0xb8000+160*24"::"a"(wheel[i]));

		delay(1000);
		early_kprint ( PL_DEBUG, "C(%d) ", i );
		
		if (i == sizeof wheel - 1)
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

	task->sched_next = current->sched_next;
	current->sched_next->sched_prev = task;
	current->sched_next = task;

	task->sched_prev = current;
	
	task->state = TASK_READY;
}

#ifdef _YOS_TASK_DEBUG
static void dump_tss( struct tss_struct *tss )
{
	early_kprint( PL_DEBUG, "es %x, cs %x, ds %x, fs %x, gs %x, "
				  "ss %x, esp %x, link %x\n",
				  tss->es, tss->cs, tss->ds, tss->fs, tss->gs,
				  tss->ss, tss->esp, tss->link );
	
	early_kprint( PL_DEBUG, "esp0 %x, ss0 %x, eip %x, eflags %x\n",
				  tss->esp0, tss->ss0, tss->eip, tss->eflags );
}
#endif

void scheduler()
{
	struct task_struct *l = &task_init;
	struct task_struct *next_run = NULL;
	int max_prio = 0;

	int x =0, y = 0;
	get_cursor( &x, &y );

	do {
		if ( l->state == TASK_DEAD ) {
			l = l->sched_next;
			continue;
		}
		
		if ( l->state == TASK_READY ) {
			if ( l->priority > max_prio ) {
				max_prio = l->priority;
				next_run = l;
			} 
		}

		l = l->sched_next;
	} while ( l != &task_init );

	set_cursor( 7, VIDEO_ROWS-1 );	
	if ( max_prio == 0 ) {
		early_kprint ( PL_DEBUG, "L" );
		
		for ( l = &task_init; l != NULL; l = l->next ) {
			l->priority = DEFAULT_PRIO;
		}
	} else 
		early_kprint ( PL_DEBUG, " " );
	
	if ( next_run == NULL )
		next_run = &task_init;
	
	next_run->priority--;

	setup_gdt_entry((SEL_CUR_TSS>>3), (uint32)&next_run->tss, 0x67UL, (F_USER32_TSS));
	setup_gdt_entry((SEL_CUR_LDT>>3), (uint32)&next_run->ldt, 0xfUL, (F_USER32_LDT));

	current->state = TASK_READY;	
	next_run->state = TASK_RUNNING;
#ifdef _YOS_TASK_DEBUG
	struct tss_struct old_tss1, old_tss2;
	memcpy((char*)&old_tss1, (char*)&current->tss, sizeof(struct tss_struct));
	memcpy((char*)&old_tss2, (char*)&next_run->tss, sizeof(struct tss_struct));
#endif
	current = next_run;

	set_cursor( 10, VIDEO_ROWS-1 );
	if ( current == &task_init ) {
		early_kprint ( PL_DEBUG, "INIT(%d)", jiffies );
	} else {
		early_kprint ( PL_DEBUG, "USER(%d)", jiffies );		
	}
	set_cursor( x, y );

#ifdef _YOS_TASK_DEBUG
	early_kprint( PL_DEBUG, "before switch, current tss & next task tss (%d)\n",
		jiffies );
	dump_tss(&old_tss1);	
	dump_tss(&old_tss2);
#endif
	
	__asm__ __volatile__ (
		"ljmp %0, $0 \n\t"
		::"i"(SEL_CUR_TSS)
		);

	/**
	 * Situation 1: when only task_init created
	 * when ljmp executed, ip after ljmp is saved to old TSS.
	 * why? because I use interrupt-gate towards exception handling,
	 * so no task switch occurs, exception handler represent current
	 * task to execute. so current task's next instruction address
	 * ( here is right after ljmp ) is saved as eip into TSS.
	 * it seems that qemu has a bug of this, bochs and vmware is right.
	 * Situation 2: when there are more than one task
	 */
	// do some test to confirm that task switch actually occurs
	// print old/current TSS, EIP etc
#ifdef _YOS_TASK_DEBUG	
	early_kprint( PL_DEBUG, "after switch, new tss (%d) \n", jiffies );
	dump_tss(&current->tss);
//	while(1);
#endif
	
	get_cursor( &x, &y );
	set_cursor( 25, VIDEO_ROWS-1 );
	early_kprint ( PL_DEBUG, "RET_SCHED(%d)", jiffies );
	set_cursor( x, y );
}
