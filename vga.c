#include <vga.h>
#include <io.h>

#define outportb(p, v)   outb(v, p)
#define inportb(p)       inb(p)
#define	pokeb(S,O,V)		*(unsigned char *)(16uL * (S) + (O)) = (V)
#define	peekb(S,O)		*(unsigned char *)(16uL * (S) + (O))

static unsigned g_wd, g_ht;

unsigned char g_640x480x16[] =
{
/* MISC */
	0xE3,
/* SEQ */
	0x03, 0x01, 0x08, 0x00, 0x06,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0x0B, 0x3E,
	0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xEA, 0x0C, 0xDF, 0x28, 0x00, 0xE7, 0x04, 0xE3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
	0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x01, 0x00, 0x0F, 0x00, 0x00
};

unsigned char g_320x200x256[] =
{
/* MISC */
	0x63,
/* SEQ */
	0x03, 0x01, 0x0F, 0x00, 0x0E,
/* CRTC */
	0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
	0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
	0xFF,
/* GC */
	0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
	0xFF,
/* AC */
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x41, 0x00, 0x0F, 0x00,	0x00
};

void read_regs(unsigned char *regs)
{
	unsigned i;

/* read MISCELLANEOUS reg */
	*regs = inportb(VGA_MISC_OUTPUT_RD);
	regs++;
/* read SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		outportb(VGA_SEQ_ADDR_INDEX, i);
		*regs = inportb(VGA_SEQ_DATA);
		regs++;
	}
/* read CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outportb(VGA_CRTC_ADDR_INDEX, i);
		*regs = inportb(VGA_CRTC_DATA);
		regs++;
	}
/* read GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outportb(VGA_GC_ADDR_INDEX, i);
		*regs = inportb(VGA_GC_DATA);
		regs++;
	}
/* read ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
//		(void)inportb(VGA_INSTAT_READ);
		(void)inportb(VGA_INPUT_STATUS1);		
		outportb(VGA_ATTR_ADDR_INDEX, i);
		*regs = inportb(VGA_ATTR_DATA_RD);
		regs++;
	}
/* lock 16-color palette and unblank display */
//	(void)inportb(VGA_INSTAT_READ);
	(void)inportb(VGA_INPUT_STATUS1);	
	outportb(VGA_ATTR_ADDR_INDEX, 0x20);
}


void write_regs(unsigned char *regs)
{
	unsigned i;

/* write MISCELLANEOUS reg */
	outportb(VGA_MISC_OUTPUT_WR, *regs);
	regs++;
/* write SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		outportb(VGA_SEQ_ADDR_INDEX, i);
		outportb(VGA_SEQ_DATA, *regs);
		regs++;
	}
/* unlock CRTC registers */
	outportb(VGA_CRTC_ADDR_INDEX, 0x03);
	outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) | 0x80);
	outportb(VGA_CRTC_ADDR_INDEX, 0x11);
	outportb(VGA_CRTC_DATA, inportb(VGA_CRTC_DATA) & ~0x80);
/* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
/* write CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		outportb(VGA_CRTC_ADDR_INDEX, i);
		outportb(VGA_CRTC_DATA, *regs);
		regs++;
	}
/* write GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		outportb(VGA_GC_ADDR_INDEX, i);
		outportb(VGA_GC_DATA, *regs);
		regs++;
	}
/* write ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		(void)inportb(VGA_INPUT_STATUS1);
//		
		outportb(VGA_ATTR_ADDR_INDEX, i);
		outportb(VGA_ATTR_DATA_WR, *regs);
		regs++;
	}
/* lock 16-color palette and unblank display */
	(void)inportb(VGA_INPUT_STATUS1);
	outportb(VGA_ATTR_ADDR_INDEX, 0x20);
}
/*****************************************************************************
*****************************************************************************/
static void set_plane(unsigned p)
{
	unsigned char pmask;

	p &= 3;
	pmask = 1 << p;
/* set read plane */
	outportb(VGA_GC_ADDR_INDEX, 4);
	outportb(VGA_GC_DATA, p);
/* set write plane */
	outportb(VGA_SEQ_ADDR_INDEX, 2);
	outportb(VGA_SEQ_DATA, pmask);
}

static unsigned get_fb_seg(void)
{
	unsigned seg;

	outportb(VGA_GC_ADDR_INDEX, 6);
	seg = inportb(VGA_GC_DATA);
	seg >>= 2;
	seg &= 3;
	switch(seg)
	{
	case 0:
	case 1:
		seg = 0xA000;
		break;
	case 2:
		seg = 0xB000;
		break;
	case 3:
		seg = 0xB800;
		break;
	}
	return seg;
}

static unsigned vpeekb(unsigned off)
{
	return peekb(get_fb_seg(), off);
}


static void vpokeb(unsigned off, unsigned val)
{
	pokeb(get_fb_seg(), off, val);
}

static void write_pixel4p(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes, off, mask, p, pmask;

	wd_in_bytes = g_wd / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	for(p = 0; p < 4; p++)
	{
		set_plane(p);
		if(pmask & c) {
			vpokeb(off, vpeekb(off) | mask);
		} else
			vpokeb(off, vpeekb(off) & ~mask);
		pmask <<= 1;
	}
}

static void (*g_write_pixel)(unsigned x, unsigned y, unsigned c);

 
static void write_pixel8(unsigned x, unsigned y, unsigned c)
{
	unsigned wd_in_bytes;
	unsigned off;

	wd_in_bytes = g_wd;
	off = wd_in_bytes * y + x;
	vpokeb(off, c);
}

static void draw_x(void)
{
	unsigned x, y;

/* clear screen */
	for(y = 0; y < g_ht; y++)
		for(x = 0; x < g_wd; x++)
			g_write_pixel(x, y, 0);
/* draw 2-color X */
	for(y = 0; y < g_ht; y++)
	{
		g_write_pixel((g_wd - g_ht) / 2 + y, y, 1);
		g_write_pixel((g_ht + g_wd) / 2 - y, y, 2);
	}
}

void demo_graphics(void)
{
	early_kprint( PL_INFO, "test VGA!\n" );
	
/* 256-color */
	write_regs(g_320x200x256);
	g_wd = 320;
	g_ht = 200;
	g_write_pixel = write_pixel8;
	draw_x();
}

// 640x480 16 colors at a0000
void setup_vga_mode12()
{
	// write mode
	outb( vga_gc_mode, VGA_GC_ADDR_INDEX );
	outb( VGA_GC_WRITE_MODE_0, VGA_GC_DATA );

	// data rotate : rotate count 0, logical operation: none
	outb( vga_gc_data_rotate, VGA_GC_ADDR_INDEX );
	outb( VGA_GC_OP_NONE, VGA_GC_DATA );

	// misc
	outb( vga_gc_misc, VGA_GC_ADDR_INDEX );
	outb( VGA_GC_MEM_MAP_1 | VGA_GC_DISABLE_ALPHANUM, VGA_GC_DATA );

	// clocking mode
	outb( vga_seq_clocking_mode, VGA_SEQ_ADDR_INDEX );
	outb( VGA_SEQ_8DOT_MODE, VGA_SEQ_DATA );

	// plane write map
	outb( vga_seq_map_mask, VGA_SEQ_ADDR_INDEX );
	outb( VGA_SEQ_MAP_MASK_3, VGA_SEQ_DATA );

	// extend mem to 256k
	outb( vga_seq_mem_mode, VGA_SEQ_ADDR_INDEX );
	outb( VGA_SEQ_EXT_MEM, VGA_SEQ_DATA );

	// select 0x3d4, 0x3d5 first
	byte c = inb( VGA_MISC_OUTPUT_RD );
	outb( c | 0x01, VGA_MISC_OUTPUT_WR );
	
	// crtc
	outb( 0x00, 0x3d4 );
	outb( 0x5f, 0x3d5 );

	outb( 0x01, 0x3d4 );
	outb( 0x4f, 0x3d5 );

	outb( 0x02, 0x3d4 );
	outb( 0x50, 0x3d5 );

	outb( 0x03, 0x3d4 );
	outb( 0x82, 0x3d5 );

	outb( 0x04, 0x3d4 );
	outb( 0x54, 0x3d5 );

	outb( 0x05, 0x3d4 );
	outb( 0x80, 0x3d5 );

	outb( 0x06, 0x3d4 );
	outb( 0x0b, 0x3d5 );

	outb( 0x07, 0x3d4 );
	outb( 0x3e, 0x3d5 );

	outb( 0x08, 0x3d4 );
	outb( 0x00, 0x3d5 );

	outb( 0x09, 0x3d4 );
	outb( 0x40, 0x3d5 );

	outb( 0x10, 0x3d4 );
	outb( 0xea, 0x3d5 );

	outb( 0x11, 0x3d4 );
	outb( 0x8c, 0x3d5 );

	outb( 0x12, 0x3d4 );
	outb( 0xdf, 0x3d5 );

	outb( 0x13, 0x3d4 );
	outb( 0x28, 0x3d5 );

	outb( 0x14, 0x3d4 );
	outb( 0x00, 0x3d5 );

	outb( 0x15, 0x3d4 );
	outb( 0xe7, 0x3d5 );

	outb( 0x16, 0x3d4 );
	outb( 0x04, 0x3d5 );

	outb( 0x17, 0x3d4 );
	outb( 0xe3, 0x3d5 );

	// initialize VGA_ATTR_ADDR to read index 
	inb( 0x3DA );
	
	// enable all 4 color plane
	outb( 0x12, VGA_ATTR_ADDR_INDEX );
	outb( 0xff, VGA_ATTR_DATA_WR );

	// selects graphics mode
	outb( 0x10, VGA_ATTR_ADDR_INDEX );
	outb( 0x01, VGA_ATTR_DATA_WR );
	
	g_wd = 640;
	g_ht = 480;
	g_write_pixel = write_pixel4p;
	draw_x();
	
//	byte *pvgamem = (byte*)0xa0000;

	/* outb( vga_gc_enable_set_reset, VGA_GC_ADDR_INDEX ); */
	/* outb( 0x0e, VGA_GC_DATA ); */

	/* outb( vga_gc_set_reset, VGA_GC_ADDR_INDEX ); */
	/* outb( 0x0a, VGA_GC_DATA ); */
	
	/* int i = 0; */
	/* for ( ; i < 1000; i++ ) */
	/* 	*(pvgamem+i) = i; */
	
}

vga_mode13_device_t vga_mode13_dev = {
	.vga_dev = {
	},	
};

	
