#ifndef _vga_h
#define _vga_h

/*******************************************************************************/
/* this file contains basic vga adapter driver                                 */
/* and abstract layer for output in graphics mode                              */
/*******************************************************************************/

//general regs
#define VGA_MISC_OUTPUT_RD		0x3CC
#define VGA_MISC_OUTPUT_WR		0x3C2
#define VGA_INPUT_STATUS0		0x3C2
#define VGA_INPUT_STATUS1		0x3DA
#define VGA_FEATURE_CTRL_RD		0x3CA
#define VGA_FEATURE_CTRL_WR		0x3DA
#define VGA_VIDEO_ENABLE		0x3C3


//sequencer regs				 
#define VGA_SEQ_ADDR_INDEX		0x3C4
#define VGA_SEQ_DATA			0x3C5

/*
 * Index 00h -- Reset Register
 * Index 01h -- Clocking Mode Register
 * Index 02h -- Map Mask Register
 * Index 03h -- Character Map Select Register
 * Index 04h -- Sequencer Memory Mode Register
 */
enum vga_seq_index {
	vga_seq_reset = 0,
	vga_seq_clocking_mode = 0x01,
	vga_seq_map_mask = 0x02,
	vga_seq_char_map_select = 0x03,
	vga_seq_mem_mode = 0x04
};
	
#define VGA_SEQ_8DOT_MODE   0x1
#define VGA_SEQ_9DOT_MODE   0x0

// plane write enable
#define VGA_SEQ_MAP_MASK_0  0x00
#define VGA_SEQ_MAP_MASK_1  0x01
#define VGA_SEQ_MAP_MASK_2  0x10
#define VGA_SEQ_MAP_MASK_3  0x11

// seq memory mode
#define VGA_SEQ_EXT_MEM          0x02
#define VGA_SEQ_ODD_EVEN_DISABLE 0x04
#define VGA_SEQ_CHAIN_4          0x08

// crt controller regs	(3D4/5 or 3B4/5 according to 
#define VGA_CRTC_ADDR_INDEX		0x3D4
#define VGA_CRTC_DATA			0x3D5

/*
 * Index 00h -- Horizontal Total Register
 * Index 01h -- End Horizontal Display Register
 * Index 02h -- Start Horizontal Blanking Register
 * Index 03h -- End Horizontal Blanking Register
 * Index 04h -- Start Horizontal Retrace Register
 * Index 05h -- End Horizontal Retrace Register
 * Index 06h -- Vertical Total Register
 * Index 07h -- Overflow Register
 * Index 08h -- Preset Row Scan Register
 * Index 09h -- Maximum Scan Line Register
 * Index 0Ah -- Cursor Start Register
 * Index 0Bh -- Cursor End Register
 * Index 0Ch -- Start Address High Register
 * Index 0Dh -- Start Address Low Register
 * Index 0Eh -- Cursor Location High Register
 * Index 0Fh -- Cursor Location Low Register
 * Index 10h -- Vertical Retrace Start Register
 * Index 11h -- Vertical Retrace End Register
 * Index 12h -- Vertical Display End Register
 * Index 13h -- Offset Register
 * Index 14h -- Underline Location Register
 * Index 15h -- Start Vertical Blanking Register
 * Index 16h -- End Vertical Blanking
 * Index 17h -- CRTC Mode Control Register
 * Index 18h -- Line Compare Register
 */
enum vga_crtc_index {
	vga_crtc_hor_total = 0x0,
	vga_crtc_end_hor_display = 0x01,
	vga_crtc_start_hor_blanking = 0x02,
	vga_crtc_end_hor_blanking = 0x03,
	vga_crtc_start_hor_retrace = 0x04,
	vga_crtc_end_hor_retrace = 0x05,
	vga_crtc_vert_total = 0x06,
	vga_crtc_overflow = 0x07,
	vga_crtc_preset_row_scen = 0x08,
	vga_crtc_max_scanline = 0x09,
	vga_crtc_cursor_start = 0x0a,
	vga_crtc_cursor_end = 0x0b,
	vga_crtc_start_addr_high = 0x0c,
	vga_crtc_start_addr_low = 0x0d,
	vga_crtc_cursor_loc_high = 0x0e,
	vga_crtc_cursor_loc_low = 0x0f,
	vga_crtc_vert_retrace_start = 0x10,
	vga_crtc_vert_retrace_end = 0x11,
	vga_crtc_vert_display_end = 0x12,
	vga_crtc_offset = 0x13,
	vga_crtc_underline_loc = 0x14,
	vga_crtc_start_vert_blanking = 0x15,
	vga_crtc_end_vert_blanking = 0x16,
	vga_crtc_mode_control = 0x17,
	vga_crtc_line_compare = 0x18
};

	
//graphics controller regs			 
#define VGA_GC_ADDR_INDEX	0x3CE
#define VGA_GC_DATA			0x3CF
/**
 * Index 00h -- Set/Reset Register
 * Index 01h -- Enable Set/Reset Register
 * Index 02h -- Color Compare Register
 * Index 03h -- Data Rotate Register
 * Index 04h -- Read Map Select Register
 * Index 05h -- Graphics Mode Register
 * Index 06h -- Miscellaneous Graphics Register
 * Index 07h -- Color Don't Care Register
 * Index 08h -- Bit Mask Register
 */
enum vga_gc_index {
	vga_gc_set_reset = 0x00,
	vga_gc_enable_set_reset = 0x01,
	vga_gc_color_compare = 0x02,
	vga_gc_data_rotate = 0x03,
	vga_gc_read_map_select = 0x04,
	vga_gc_mode = 0x05,
	vga_gc_misc = 0x06,
	vga_gc_color_dont_care = 0x07,
	vga_gc_bit_mask = 0x08,
	vga_gc_index_max = vga_gc_bit_mask
};

// vga_gc_mode
#define VGA_GC_WRITE_MODE_0 0x00
#define VGA_GC_WRITE_MODE_1 0x01
#define VGA_GC_WRITE_MODE_2 0x10
#define VGA_GC_WRITE_MODE_3 0x11

#define VGA_GC_READ_MODE_0  (0x00)
#define VGA_GC_READ_MODE_1  (0x80)
/*
 * 00b - Result is input from previous stage unmodified.
 * 01b - Result is input from previous stage logical ANDed with latch register.
 * 10b - Result is input from previous stage logical ORed with latch register.
 * 11b - Result is input from previous stage logical XORed with latch register.
 */
#define VGA_GC_OP_NONE   0x00
#define VGA_GC_OP_AND    (0x01<<3)
#define VGA_GC_OP_OR     (0x10<<3)
#define VGA_GC_OP_XOR    (0x11<<3)
/*
    * Memory Map Select
      This field specifies the range of host memory addresses that is decoded by the VGA hardware and mapped into display memory accesses.  The values of this field and their corresponding host memory ranges are:
          o 00b -- A0000h-BFFFFh (128K region)
          o 01b -- A0000h-AFFFFh (64K region)
          o 10b -- B0000h-B7FFFh (32K region)
          o 11b -- B8000h-BFFFFh (32K region)
      Chain O/E -- Chain Odd/Even Enable
      "When set to 1, this bit directs the system address bit, A0, to be replaced by a higher-order bit. The odd map is then selected when A0 is 1, and the even map when A0 is 0."
      Alpha Dis. -- Alphanumeric Mode Disable
      "This bit controls alphanumeric mode addressing. When set to 1, this bit selects graphics modes, which also disables the character generator latches."
*/
#define VGA_GC_MEM_MAP_0   0
#define VGA_GC_MEM_MAP_1   (0x01<<2)
#define VGA_GC_MEM_MAP_2   (0x10<<2)
#define VGA_GC_MEM_MAP_3   (0x11<<2)

#define VGA_GC_DISABLE_ALPHANUM 0x01

// attribute controller				 
#define VGA_ATTR_ADDR_INDEX		0x3C0
#define VGA_ATTR_DATA_RD		0x3C1
#define VGA_ATTR_DATA_WR		0x3C0

// video dac palette registers 
#define VGA_DAC_WRITE_ADDR      0x3C8
#define VGA_DAC_READ_ADDR       0x3C7
#define VGA_DAC_STATE           0x3C7
#define VGA_DAC_DATA            0x3C9
#define VGA_DAC_PEL_MASK        0x3C6

#define	VGA_NUM_SEQ_REGS	5
#define	VGA_NUM_CRTC_REGS	25
#define	VGA_NUM_GC_REGS		9
#define	VGA_NUM_AC_REGS		21
#define	VGA_NUM_REGS		(									\
		1 + VGA_NUM_SEQ_REGS + VGA_NUM_CRTC_REGS +				\
		VGA_NUM_GC_REGS + VGA_NUM_AC_REGS						\
		)

typedef int VGA_COLOR;

// i want it to be abstract device for vga used by all drawing routines
struct vga_device_struct
{
	int width;   // visible width 
	int height;  // visible height
	int colors;  // number of colors available

	void (*init_device)(struct vga_device_struct *vga_dev);
	void (*release_device)(struct vga_device_struct *vga_dev);
	
	void (*write_pixel)(int x, int y, VGA_COLOR clr);
	VGA_COLOR (*read_pixel)(int x, int y);
	void (*clear_screen)();
	// cursor, scroll...
};

typedef struct vga_mode13_device_struct 
{
	struct vga_device_struct vga_dev;
	
} vga_mode13_device_t;

extern vga_mode13_device_t vga_mode13_dev;

extern void demo_graphics(void);
extern void setup_vga_mode12();

#endif
