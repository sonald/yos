/**
 * Timer interrupt
 */
#include <kernel.h>
#include <isr.h>
#include <timer.h>
#include <io.h>
#include <task.h>

void init_8254_timer()
{
	unsigned int divisor = TIMER_FREQ/HZ;
	outb( TIMER_CHANNEL_0+ACCESS_LO_HI+TIMER_MODE_3, TIMER_OPER_PORT );
	outb( divisor & 0xff, 0x40 );
	outb( divisor >> 8, 0x40 );

	// clear IMF for IRQ0 ( timer )
	unsigned char imf = inb(0x21);
	outb( imf & 0xfe, 0x21 );
}

uint32 jiffies = 0;

void do_timer()
{
	cli();
	jiffies++;
	int x, y;
	get_cursor(&x, &y);
	set_cursor(74, 24);
	early_kprint( PL_WARN, "%d", jiffies/HZ );
	set_cursor(x, y);

	// send EOI to indicate cpu that interrupt process
	// this is necessary because we programmed pic to manual mode
	outb( 0x20, 0x20 );

	// schedule 5 times a second
	byte schedule = (jiffies % (HZ/5) == 0);
	if ( schedule ) {
		scheduler();
	}
	sti();
	
}

void delay(int msecs)
{
	uint32 ticks = (msecs * HZ) / 1000;
	uint32 start = jiffies;
	while ( jiffies - start < ticks )
		;
}
