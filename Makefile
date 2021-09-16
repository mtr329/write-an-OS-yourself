target = Minux.img
tempDir = temp
mountDir = mnt
kernelEntry = 0x30400

kIncDir = kernel/src/include
kSrcDir = kernel/src

# .h files
kerSrcIncs = $(kIncDir)/const.h  $(kIncDir)/sys.h $(kIncDir)/type.h $(kIncDir)/kb.h
# befor kernel.bin
kernelTempFiles = $(tempDir)/kernel.o $(tempDir)/start.o $(tempDir)/protect.o $(tempDir)/clib.o\
		  $(tempDir)/main.o $(tempDir)/global.o $(tempDir)/clock.o $(tempDir)/keyboard.o\
		  $(tempDir)/tty.o $(tempDir)/console.o $(tempDir)/print.o
# includes in kernel.asm
kernelFileIncs = $(kSrcDir)/util.asm $(kSrcDir)/interrupt.asm $(kIncDir)/sconst.inc \
		 $(kSrcDir)/syscall.asm

ccKerSrc = gcc -c -m32 -fno-builtin -fno-stack-protector -I $(kIncDir)

#-------------------------------------------------------------------------
$(target): $(tempDir)/boot.bin $(tempDir)/loader.bin $(tempDir)/kernel.bin $(mountDir)
	dd if=$(tempDir)/boot.bin of=$(target) bs=512 count=1
	dd if=/dev/zero of=$(target) bs=512 count=2879 seek=1
	mount $(target) $(mountDir) -o loop
	cp -f $(tempDir)/loader.bin $(mountDir)
	cp -f $(tempDir)/kernel.bin $(mountDir)
	umount $(mountDir)

$(tempDir)/kernel.bin: $(kernelTempFiles) $(tempDir)
	ld -m elf_i386 -s -Ttext $(kernelEntry) $(kernelTempFiles) -o $@

$(tempDir)/kernel.o: kernel/kernel.asm $(kernelFileIncs) $(tempDir)
	nasm -f elf $< -I kernel -o $@
#------------------------------------------------------------------------------
$(tempDir)/print.o: $(kSrcDir)/print.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/console.o: $(kSrcDir)/console.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/tty.o: $(kSrcDir)/tty.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/keyboard.o: $(kSrcDir)/keyboard.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/clock.o: $(kSrcDir)/clock.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/main.o: $(kSrcDir)/main.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/protect.o: $(kSrcDir)/protect.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/start.o: $(kSrcDir)/start.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/clib.o: $(kSrcDir)/clib.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(tempDir)/global.o: $(kSrcDir)/global.c $(kIncDir)/global.h $(tempDir)
	$(ccKerSrc) $< -o $@

$(kIncDir)/global.h: $(kerSrcIncs)

$(tempDir)/boot.bin: boot/boot.asm \
	boot/inc/fat12hdr.inc boot/inc/lib.inc $(tempDir)
	nasm boot/boot.asm -I boot -o $(tempDir)/boot.bin

$(tempDir)/loader.bin: boot/loader.asm \
	boot/inc/fat12hdr.inc boot/inc/lib.inc boot/inc/pm.inc $(tempDir)
	nasm boot/loader.asm -I boot -o $(tempDir)/loader.bin

$(tempDir):
	mkdir $(tempDir)

$(mountDir):
	mkdir $(mountDir)

clean:
	rm -rf $(tempDir)
	rm -rf $(mountDir)
	rm $(target)
	
