#include <io.h>

char __early_buf[BUF_SIZE];
char __early_print_buf[BUF_SIZE];

static const int PAGE_SIZE = (VIDEO_COLUMNS << 1) * VIDEO_ROWS;
int screen_x = 0;
int screen_y = 0;

// this is used only before VGA IO DRIVER FINISHED
#define CURSOR_SHAPE ('_')
//#define CURSOR_ATTR  (RED & 0b10000000)
#define CURSOR_ATTR  (RED)
#define TAB_WIDTH    8

// should not overlap
void* memcpy(void* dest, const void* src, uint32 size)
{
	int dist = (int)dest - (int)src;
	if ( dist < 0 ) dist = -dist;
	if ( dist < size )
		// overlap
		return dest;
	
	__asm__ __volatile__ (
		"pushl %%es       \n\t"
		"pushl %%eax      \n\t"		
		"cld              \n\t"
		"movl %%ds, %%eax \n\t"
		"movl %%eax, %%es \n\t"
		"1:               \n\t"
		"cmp $0, %%ecx    \n\t"
		"je 1f            \n\t"
		"movsb            \n\t"
		"dec %%ecx        \n\t"
		"jmp 1b           \n\t"
		"1:		          \n\t"
		"popl %%eax       \n\t"		
		"popl %%es        \n\t"
		:
		: "S"(src), "D"(dest), "c"(size)
		: "eax"
		);
	return dest;
}

// may overlap
void* memmove(void* dest, const void* src, uint32 size)
{
	__asm__ __volatile__ (
		"pushl %%es       \n\t"
		"pushl %%eax      \n\t"		
		"cld              \n\t"
		"movl %%ds, %%eax \n\t"
		"movl %%eax, %%es \n\t"
		"1:               \n\t"
		"cmp $0, %%ecx    \n\t"
		"je 1f            \n\t"
		"movsb            \n\t"
		"dec %%ecx        \n\t"
		"jmp 1b           \n\t"
		"1:		          \n\t"
		"popl %%eax       \n\t"		
		"popl %%es        \n\t"
		:
		: "S"(src), "D"(dest), "c"(size)
		);
	return dest;
}

void memset(void* dest, int i, uint32 len)
{
	if ( __unlikely(len <= 0) )
		return;
	
	byte b = (byte)( (unsigned)i & 0x00ff );

	__asm__ __volatile__ (
		"cld \n\t"
		"1: cmp $0, %%ecx \n\t"
		"jl 1f \n\t"
		"movb %0, (%%esi, %%ecx) \n\t"
		"dec %%ecx \n\t"
		"jmp 1b \n\t"
		"1: nop \n\t"
		:
		: "r"(b), "S"(dest), "c"(len-1)
		);

/*	
	uint32 j = 0;
	for ( j = 0; j < len; j++ )
	((char*)dest)[j] = b;
*/
}

char* early_itos(int i, int base)
{
	int goon =  ( base == 2 ) || (base == 16 ) || (base == 10);
	if ( !goon )
		return NULL;

	int minus = (i >= 0) ? 0: 1;
	int absolute = abs(i);
	
	char buf[BUF_SIZE] = {0};
	memset(__early_buf, 0, BUF_SIZE);
	
	int divider = absolute, remainder = 0;
	char *p = buf;
	*p = '0';
	
	/* while ( divider ) { */
	/* 	remainder = divider%base; */
	/* 	divider = divider/base; */
	/* 	*p++ = "0123456789ABCDEF"[remainder]; */
	/* } */

	do {
		remainder = divider%base;
		divider = divider/base;
		*p++ = "0123456789ABCDEF"[remainder];
	} while ( divider );
	
	// check overflow
	if ( p - buf > BUF_SIZE )
		return NULL;

	if ( p == buf ) {
		return NULL;
	} else {
		if ( minus ) {
			*p++ = '-';
			*p = '\0';
		}

		char *q = __early_buf;
		while ( p > buf ) {
			*q++ = *--p;
		}
		*q = 0;
		return __early_buf;
	}
}

void early_strncpy(char* dest, const char *src, uint16 length)
{
	if ( length == 0 )
		return;
	
	uint16 size = min(length, early_strlen(src));

	if ( size == 0 )
		return;
	
	/* uint16 i = 0; */
	/* while ( i++ < size && *src != 0 ) */
	/* 	*dest++ = *src++; */
	
	__asm__ __volatile__ ( "xorl %%eax, %%eax  \n\t"
						   "push %%es          \n\t"
						   "mov %%ds, %%eax    \n\t"
						   "mov %%eax, %%es    \n\t"
						   "cld                 \n\t"
						   "rep movsb          \n\t"
						   "pop %%es           \n\t"
						   :
						   : "D"(dest), "S"(src), "c"(size)
						   : "eax" );

}

int early_strlen(const char* s)
{
	if ( s == NULL )
		return 0;
	
	int len = 0;
	while ( *s++ ) len++;
	return len;
}

print_level_color_t __default_color_scheme[PL_MAX] = {
	{ WHITE, BLACK },
	{ GREEN, BLACK },
	{ RED, BLACK },
	{ CYAN, BLACK },	
};
	
int early_kprint(PRINT_LEVEL lvl, const char* fmt, ...)
{
	args_list al;
	int printed_chars = 0;
	int i;
	uint16 len;
	char c;
	char *s;

	var_init(al, fmt);

	memset(__early_print_buf, 0, BUF_SIZE);
	char *p = __early_print_buf;
	while( *fmt ) {
		if ( *fmt == '%' ) {
			++fmt;
			switch(*fmt) {
			case 'd':
				i = var_next_arg(al, int);
				s = early_itos(i, 10);
				len = early_strlen(s);
				early_strncpy(p, s, len);
				p += len;				
				break;

			case 'x':
				i = var_next_arg(al, int);
				s = early_itos(i, 16);
				len = early_strlen(s);
				early_strncpy(p, s, len);
				p += len;
				break;
								
			case 's':
				s = var_next_arg(al, char*);
				len = early_strlen(s);
				early_strncpy(p, s, len);
				p += len;
				break;

			case 'c':
				c = var_next_arg(al, char);
				early_strncpy(p, (char*)&c, 1);
				p += 1;
				break;
				
			case '%':
				*p++ = '%';
				break;
				
			default: break;
			} //~ switch

		} else {
			*p++ = *fmt;
		}

		++fmt;		
	} //~ while
	*p = 0;
	
	var_end(al);
	printed_chars = p - __early_print_buf;

#ifndef _YOS_LIBTEST
	for ( i = 0; i < printed_chars; i++ ) {
		char c = *((char*)__early_print_buf + i);
		if ( c ) 
			early_video_putc( c, __default_color_scheme[lvl] );
	}
#endif
	
	return printed_chars;	
}

void get_cursor(int *x, int *y)
{
	if ( x != NULL && y != NULL ) {
		*x = screen_x;
		*y = screen_y;
	}
}

static void update_cursor()
{
	unsigned short position=(screen_y*80) + screen_x;

	// cursor LOW port to vga INDEX register
	outb(0x0F, 0x3d4);
	outb((unsigned char)(position&0xFF), 0x3d5);
	// cursor HIGH port to vga INDEX register
	outb(0x0E, 0x3d4);
	outb((unsigned char )((position>>8)&0xFF), 0x3d5);
}

void set_cursor(int x, int y)
{
	if ( INSET(x, 0, VIDEO_COLUMNS-1) && INSET(y, 0, VIDEO_ROWS-1) ) {
		screen_x = x;
		screen_y = y;
		update_cursor();
	}
}

void scroll_up(int n)
{
	if ( n >= VIDEO_ROWS )
		n = VIDEO_ROWS;
	else if ( n <= 0 ) {
		n = 0;
		return;
	}
	
	const int LINE_SIZE = (VIDEO_COLUMNS<<1);
	byte *dest = (byte*)0;
	byte *src = dest + LINE_SIZE*n;
	uint32 size = (PAGE_SIZE - LINE_SIZE*n)>>2;
		
	__asm__ __volatile__ (
		"pushl %%ds       \n\t"
		"cld              \n\t"
		"movl %%es, %%eax \n\t"
		"movl %%eax, %%ds \n\t"
		"rep movsl        \n\t"
		"popl %%ds        \n\t"
		:
		: "S"(src), "D"(dest), "c"(size)
		: "eax"
		);

	// clear remaining lines
	// as LINE_SIZE here is 4-aligned, use stosq instead of stosb for speed
	dest = (byte*)0 + PAGE_SIZE -LINE_SIZE*n;
	size = (LINE_SIZE*n)>>2;
	__asm__ __volatile__ (
		"cld              \n\t"
		"xor %%eax, %%eax \n\t"
		"mov $0, %%eax    \n\t"
		"rep stosl        \n\t"
		:
		: "D"(dest), "c"(size)
		: "eax"
		);
}


// true if (screen_x, screen_y) is the last position in a page
int need_scroll() 
{
	return (screen_x == VIDEO_COLUMNS-1) && (screen_y == VIDEO_ROWS-1);
}

void backward_cursor()
{
}

void forward_cursor() 
{
	if ( need_scroll() ) {
		scroll_up(1);
		screen_x = 0;
		screen_y = VIDEO_COLUMNS - 1;
		
	} else {
		if ( screen_x == VIDEO_COLUMNS-1 ) {
			screen_x = 0;
			screen_y++;
		} else {
			screen_x++;
		}
	}
	update_cursor();
}

// this routine assumes that %es is the video segment selector
void early_video_putc(char c, print_level_color_t color)
{
	char attr = (color.fg & 0x0f) | ((color.bg<<4) & 0xf0);
	
	switch( c ) {
	case '\n':
		screen_x = 0;
		switch ( screen_y ) {
		case 0 ... VIDEO_ROWS-2:
			screen_y++;
			break;

		case VIDEO_ROWS-1:
			scroll_up(1);
			break;

		default:
			break;
		}
		break;
		
	case '\t':
		if ( screen_x + TAB_WIDTH < VIDEO_COLUMNS )
			screen_x = screen_x + TAB_WIDTH - (screen_x % TAB_WIDTH);
		break;
		
	case '\r':
		screen_x = 0;
		break;

	default:
		__asm__ __volatile__ (
			"pusha                    \n\t"
			"xorl %%esi, %%esi        \n\t"
			"imull $80, %%edx         \n\t"
			"add %%eax, %%edx         \n\t"
			"mov %%edx, %%esi         \n\t"
			"shll $1, %%esi           \n\t"
			"movb %%bl, %%es: (%%esi) \n\t"
			"inc %%esi                \n\t"
			"movb %%cl, %%es: (%%esi) \n\t"
			"popa                     \n\t"
			:
			: "b"(c), "c"(attr), "a"(screen_x), "d"(screen_y)
			: "esi"
			);
		forward_cursor();
		break;
	}
}
