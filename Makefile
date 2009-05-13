LD=ld
AS=as -I./include
DD=dd
CC=gcc
EXTRA_FLAGS= -nostdinc
CFLAGS= -I./include $(EXTRA_FLAGS) -fno-builtin -Wall -g
CAT=cat

all: kernel.img unittest

kernel.img: bootsect kernel
	$(CAT) bootsect kernel > $@
	$(DD) if=/dev/zero of=$@ seek=16 count=2801

unittest: test.c io.c
	$(CC) -g -Wall -fno-builtin -D_YOS_LIBTEST -o $@ $^ -I./include

bootsect: bootsect.o 
	$(LD) --oformat binary -N -o $@ -e _init -Ttext 0x7c00 $<

kernel: kernel.o isr.o io.o timer.o init.o 
	$(LD) --oformat binary -N -o $@ -e _kernel_start -Ttext 0x0000 $^
	$(LD) -N -o $@.elf -e _kernel_start -Ttext 0x0000 $^
	objdump -D $@.elf > kernel.img.S
	objcopy --only-keep-debug kernel.elf kernel.dbg

kernel.o: kernel.s 
	$(AS) -a -o $@ $< > $<.map

bootsect.o: bootsect.s 
	$(AS) -a -o $@ $< > $<.map

isr.o: isr.s 
	$(AS) -a -o $@ $< > $<.map

%.o: %.c common.h 
	$(CC) $(CFLAGS) -c -o $@ $<

kernel.o bootsect.o: common.inc

%.c: %.h

common.inc:

.PHONY:	clean

clean:
	-rm bootsect kernel.img *.o kernel.img.S kernel.dbg kernel.elf unittest *.map kernel

