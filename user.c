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
		__asm__ (
			"incb 0xb8000+160*24+4        \n\t"
			"movb $0x17, 0xb8000+160*24+5 \n\t"
			);
		delay(300);
	}

	/* int i = 0; */
	/* for ( ;; ) { */
	/* 	early_kprint( PL_DEBUG, "A(%d) ", i ); */
	/* 	delay(300); */
	/* 	i++; */
	/* } */

#if 0
	int x = 0, y = 0;
	int i = 0;
	
	print_level_color_t clr = {
		.bg = YELLOW,
		.fg = RED
	};
	
	for ( ;; ) {
		get_cursor(&x, &y);
		set_cursor(2, VIDEO_ROWS-1);
		early_video_putc(i+'A', clr);
		set_cursor(x, y);
		delay(10);
		i = ( i + 1 ) % 26;
	}
#endif	
}

void do_task2()
{
	for ( ;; ) {
		__asm__ (
			"addb $2, 0xb8000+160*24+6    \n\t"
			"movb $0x17, 0xb8000+160*24+7 \n\t"
			);
		delay(400);
	}

	int i = 0;
	for ( ;; ) {
		early_kprint( PL_DEBUG, "B(%d) ", i );
		delay(400);
		i++;
	}
	
#if 0
	int x = 0, y = 0;
	int i = 1;
	
	print_level_color_t clr = {
		.bg = YELLOW,
		.fg = RED
	};
	
	for ( ;; ) {
		get_cursor(&x, &y);
		set_cursor(1, VIDEO_ROWS-1);
		early_video_putc(i+'A', clr);
		set_cursor(x, y);
		delay(10);
		i = ( i + 1 ) % 26;
	}
#endif	
}


