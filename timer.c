/**
 * Timer interrupt
 */
#include <kernel.h>
#include <isr.h>
#include <timer.h>
#include <io.h>

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
	jiffies++;

	// send EOI to indicate cpu that interrupt process
	// this is necessary because we programmed pic to manual mode
	__asm__ __volatile__ (
		"mov $0x20, %%al \n\t"
		"out %%al, $0x20 \n\t"
		::: "eax"
		);
	
}
