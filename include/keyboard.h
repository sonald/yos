#ifndef _keyboard_h
#define _keyboard_h

#include <common.h>

struct kbd_state_struct {
	byte escaped;
	byte shift_down;
	byte ctrl_down;
	byte alt_down;
	byte left;
	byte capslock;
};
	
extern void init_kbd();
extern void do_keyboard();

#endif
