/**
 * simplest io lib
 */

#ifndef _yos_io_h
#define _yos_io_h

#include <common.h>

/*
Bit 7  	 Blinking
Bits 6-4 	Background color
Bit 3 	Bright
Bit3 2-0 	Foreground color
*/
#define BLANK_CHAR (' ')
#define BLANK_ATTR (0x07)

typedef enum COLOR_T {
    BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, BROWN, WHITE,
    GRAY, LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN,
    LIGHT_RED, LIGHT_MAGENTA, YELLOW, BRIGHT_WHITE
} COLOR;

typedef enum PRINT_LEVEL_T {
	PL_INFO, PL_WARN, PL_ERROR, PL_DEBUG, PL_MAX
} PRINT_LEVEL;

typedef struct print_level_color_
{
	COLOR fg;
	COLOR bg;
} print_level_color_t;

#define VIDEO_COLUMNS 80
#define VIDEO_ROWS    25

#define BUF_SIZE 1024

__attribute__((unused)) static int __last_request_type_size = 0;

// buf for print
extern char __early_print_buf[BUF_SIZE];

typedef char* args_list;

#define __arg_type_size(type) (((sizeof(type)-1)/sizeof(type)+1)*sizeof(int))

#define var_init(al, fmt) do {								\
		al = (char*)&(fmt);									\
		__last_request_type_size = __arg_type_size(&(fmt));	\
	} while( 0 );

#define var_next_arg(al, type) (al += __last_request_type_size, __last_request_type_size = __arg_type_size(type) , *(type *)al)

#define var_end(al) ( al = NULL, __last_request_type_size = 0 )

/**
 * weak implemented printf replacement, no bound check right now
 * supported meta type: %s, %d, %c, %x, %%
 * this print use internal buffer less than 1kb, so be careful
 */
int early_kprint(PRINT_LEVEL lvl, const char* fmt, ...);

// scroll up n lines
void scroll_up(int n);

// change cursor position, temperary implementation
void set_cursor(int x, int y);

/**
 * output a char to video buffer
 */
void early_video_putc(char c, print_level_color_t color);

extern char __early_buf[BUF_SIZE];
char* early_itos(int i, int base);
void early_strncpy(char* dest, const char *src, uint16 length);
/**
 * return > 0, size of s, -1, error
 */
int early_strlen(const char* s);

void memset(void* dest, int i, uint32 len);
void* memcpy(void* dest, const void* src, uint32 size);
void* memmove(void* dest, const void* src, uint32 size);

/**
 * I/O operator
 */
#define outb(data, port) __asm__ __volatile__ ( "outb %%al, %%dx\n\t"	\
												::"dx"(port), "al"(data) )


#define inb(port) ({													\
			unsigned char __ret = 0;									\
			__asm__ __volatile__ ( "inb %%dx, %%al \n\t"				\
								   : "=a"(__ret) :"dx"(port)  );		\
			__ret;														\
		})

#endif
