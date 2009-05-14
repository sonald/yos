#include <keyboard.h>
#include <io.h>

void init_kbd()
{
	// clear IMF for IRQ1 ( keyboard )
	unsigned char imf = inb(0x21);
	outb( imf & 0xfd, 0x21 );
}

/*
Escaped scancodes:
e0 1c (Keypad Enter)	| 	1c (Enter)
e0 1d (RCtrl) 			| 	1d (LCtrl)
e0 2a (fake LShift) 	| 	2a (LShift)
e0 35 (Keypad-/) 		| 	35 (/?)
e0 36 (fake RShift) 	| 	36 (RShift)
e0 37 (Ctrl-PrtScn) 	| 	37 (PrtScn)
e0 38 (RAlt) 			| 	38 (LAlt)
e0 46 (Ctrl-Break) 		| 	46 (ScrollLock)
e0 47 (Grey Home) 		| 	47 (Keypad-7/Home)
e0 48 (Grey Up) 		| 	48 (Keypad-8/UpArrow)
e0 49 (Grey PgUp) 		| 	49 (Keypad-9/PgUp)
e0 4b (Grey Left) 		| 	4b (Keypad-4/Left)
e0 4d (Grey Right) 		| 	4d (Keypad-6/Right)
e0 4f (Grey End) 		| 	4f (Keypad-1/End)
e0 50 (Grey Down) 		| 	50 (Keypad-2/DownArrow)
e0 51 (Grey PgDn) 		| 	51 (Keypad-3/PgDn)
e0 52 (Grey Insert) 	| 	52 (Keypad-0/Ins)
e0 53 (Grey Delete) 	| 	53 (Keypad-./Del) 
*/

static struct kbd_state_struct kbd_state = {
	.escaped = 0,
	.shift_down = 0,
	.ctrl_down = 0,
	.alt_down = 0,
	.left = 1
};

typedef void (*scancode_handler)(struct kbd_state_struct kbd, byte code);

void do_process_printable(struct kbd_state_struct kbd, byte code)
{
	if ( code & 0x80 )
		// key release, ignore now
		return;
	
	print_level_color_t clr = {
		.fg = WHITE,
		.bg = BLACK
	};
	char c = 'a';
	static byte char_map[][2] = {
		{00, 00}, {00, 00}, {'1', '!'}, {'2', '@'}, {'3', '#'}, {'4', '$'}, {'5', '%'}, {'6', '^'},
		{'7', '&'}, {'8', '*'}, {'9', '('}, {'0', ')'}, {'-', '_'}, {'=', '+'}, {'\b', '\b'}, {'\t', '\t'},
		{'q', 'Q'}, {'w', 'W'}, {'e', 'E'}, {'r', 'R'}, {'t', 'T'}, {'y', 'Y'}, {'u', 'U'}, {'i', 'I'},
		{'o', 'O'}, {'p', 'P'}, {'[', '{'}, {']', '}'}, {'\n', '\n'}, {00, 00}, {'a', 'A'}, {'s', 's'},
		{'d', 'D'}, {'f', 'F'}, {'g', 'G'}, {'h', 'H'}, {'j', 'J'}, {'k', 'K'}, {'l', 'L'}, {';', ':'},
		{'\'', '\"'}, {'`', '~'}, {0, 0}, {'\\', '|'}, {'z', 'Z'}, {'x', 'X'}, {'c', 'C'}, {'v', 'V'},
		{'b', 'B'}, {'n', 'N'}, {'m', 'M'}, {',', '<'}, {'.', '>'}, {'/', '?'}, {00, 00}, {'*', '*'},
		{0, 0}, {' ', ' '}
	};

	static int num_chars = sizeof(char_map)/2;
	if ( code > num_chars ) {
		early_kprint( PL_ERROR, "do_process_printable: invalid code for char: %d\n", code );
		return;
	}

	if ( kbd.shift_down ) {
		c = char_map[code][1];
		if ( c )
			early_video_putc( c, clr );
	} else {
		c = char_map[code][0];
		if ( c )
			early_video_putc( c, clr );
	}
}

void do_process_unprintable(struct kbd_state_struct kbd, byte code)
{
}

void do_process_function(struct kbd_state_struct kbd, byte code)
{
}

void do_keyboard()
{
/*
make codes(break codes: +0x80 ) :  
01 	ESC	 	02 	1! 		03 	2@      04  3#		05 	4$  	06 	5%
07 	6^ 		08 	7& 		09 	8* 		0A 	9( 		0B 	0) 		0C 	-_
0D 	=+ 		0E 	BCK 	0F 	TAB 	10 	qQ 		11 	wW 		12 	eE
13 	rR 		14 	tT 		15 	yY 		16 	uU 		17 	iI 		18 	oO
19 	pP 		1A 	[{ 		1B 	]} 	1C 	Enter 	1D 	LCTL 	1E 	aA
1F 	sS 		20 	dD 		21 	fF 		22 	gG 		23 	hH 		24  jJ
25 	kK 		26 	lL 		27 	;: 		28 	'" 		29 	`~ 	2A 	LSHT
2B 	\| 		2C 	zZ 		2D 	xX 		2E 	cC 		2F 	vV 		30 	bB
31 	nN 		32 	mM 		33 	,< 		34 .> 		35 	/? 	36 	RSHT
37 	** 		38 	LALT	39 	SPACE	3a  Capslock
3B-44 		F1-F10 		57 	F11 	58 	F12
*/
	scancode_handler prt = do_process_printable;
	scancode_handler unp = do_process_unprintable;
	scancode_handler fun = do_process_function;
	
	scancode_handler dispatch_table[0x80] = {
	/*0x0 */    unp, unp, prt, prt, prt, prt, prt, prt,	 
	/*0x8 */	prt, prt, prt, prt, prt, prt, unp, prt,	 
	/*0x10*/	prt, prt, prt, prt, prt, prt, prt, prt,	 
	/*0x18*/	prt, prt, prt, prt, unp, unp, prt, prt,	 
	/*0x20*/	prt, prt, prt, prt, prt, prt, prt, prt,	 
	/*0x28*/	prt, prt, unp, prt, prt, prt, prt, prt,	 
	/*0x30*/	prt, prt, prt, prt, prt, prt, unp, prt,	 
	/*0x38*/	unp, prt, unp, fun, fun, fun, fun, fun,
	/*0x40*/	fun, fun, fun, fun, fun, unp, unp, unp,
	/*0x48*/    unp, unp, unp, unp, unp, unp, unp, unp,
	/*0x50*/    unp, unp, unp, unp, unp, unp, unp, fun,
	/*0x58*/    fun, unp, unp, unp, unp, unp, unp, unp,
	/*0x60*/    unp, unp, unp, unp, unp, unp, unp, fun,
	/*0x68*/    unp, unp, unp, unp, unp, unp, unp, fun,
	/*0x70*/    unp, unp, unp, unp, unp, unp, unp, fun,
	/*0x78*/    unp, unp, unp, unp, unp, unp, unp, fun
	};

	
	byte scancode = 0;
	scancode = inb( 0x60 );
	
	if ( kbd_state.escaped ) {
		switch ( scancode ) {
		case 0:
			early_kprint( PL_ERROR, "KEYBOARD FAILED!\n" );
			break;
			
		case 0x1d:
			kbd_state.ctrl_down = 1; kbd_state.left = 0; break;
		case 0x38:
			kbd_state.alt_down = 1; kbd_state.left = 0; break;
		case 0x36:
			// fake rshift
		case 0x2a:
			// fake lshift
			break;

		default:
			dispatch_table[scancode&0x7f](kbd_state, scancode);
			break;
		}
		
	} else {
		switch ( scancode ) {
		case 0:
			early_kprint( PL_ERROR, "KEYBOARD FAILED!\n" );
			break;
			
		case 0xe0:
			kbd_state.escaped = 1; break;
		case 0x2a: case 0x36:
			kbd_state.shift_down = 1; break;
		case 0x1d:
			kbd_state.ctrl_down = 1; kbd_state.left = 1; break;
		case 0x38:
			kbd_state.alt_down = 1; kbd_state.left = 1; break;

		default:
			dispatch_table[scancode&0x7f](kbd_state, scancode);
			break;
		}
	}

	// send EOI to indicate cpu that interrupt process
	// this is necessary because we programmed pic to manual mode
	__asm__ __volatile__ (
		"mov $0x20, %%al \n\t"
		"out %%al, $0x20 \n\t"
		::: "eax"
		);
	
}
