#ifndef _user_h
#define _user_h

#include <common.h>

int goto_userlevel();

// two demo kernel tasks, do classic A/B switch
extern void do_task1();
extern void do_task2();

#endif
