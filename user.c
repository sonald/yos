/**
 * simple user-space support routines ( temporary )
 */

#include <io.h>
#include <timer.h>

int goto_userlevel()
{

	return 0;
	
}

void do_task1()
{
	for ( ;; ) {
		early_kprint( PL_INFO, "A" );
		delay(10);
	}
}

void do_task2()
{
	for ( ;; ) {
		early_kprint( PL_INFO, "B" );
		delay(10);
	}
}


