#ifndef _timer_h
#define _timer_h

#include <kernel.h>
#define TIMER_FREQ 1193182

#define TIMER_CHANNEL_0   0x00
#define TIMER_CHANNEL_1   0x40
#define TIMER_CHANNEL_2   0x80
#define TIMER_CHANNEL_3   0xc0  /* read back (8254 only) */

#define ACCESS_LATCH_COUNT       0x00
#define ACCESS_LO_ONLY           0x10
#define ACCESS_HI_ONLY           0x20
#define ACCESS_LO_HI             0x30

#define TIMER_MODE_0             0x00
#define TIMER_MODE_1             0x02
#define TIMER_MODE_2             0x04
#define TIMER_MODE_3             0x06
#define TIMER_MODE_4             0x08
#define TIMER_MODE_5             0x0a

#define TIMER_BCD_MODE           0x01

#define TIMER_DATA_PORT0         0x40
#define TIMER_DATA_PORT1         0x41
#define TIMER_DATA_PORT2         0x42

#define TIMER_OPER_PORT          0x43

extern void init_8254_timer();

// timer isr
extern void do_timer();

extern u64 jiffies;


#endif
