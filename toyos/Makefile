K = kernel

TOOLPREFIX ?= riscv64-unknown-elf-
CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJDUMP = $(TOOLPREFIX)objdump
QEMU = qemu-system-riscv64

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb
CFLAGS += -march=rv64gc -mcmodel=medany
CFLAGS += -ffreestanding -fno-common -nostdlib
CFLAGS += -fno-builtin -fno-stack-protector
CFLAGS += -I.

LDFLAGS = -z max-page-size=4096

OBJS = \
  $(K)/entry.o \
  $(K)/start.o \
  $(K)/main.o

$(K)/kernel: $(OBJS) $(K)/kernel.ld
	$(LD) $(LDFLAGS) -T $(K)/kernel.ld -o $(K)/kernel $(OBJS)
	$(OBJDUMP) -S $(K)/kernel > $(K)/kernel.asm

$(K)/%.o: $(K)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K)/%.o: $(K)/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: qemu clean

qemu: $(K)/kernel
	$(QEMU) -machine virt -kernel $(K)/kernel -m 128M -smp 1 -nographic

clean:
	rm -f $(K)/*.o $(K)/*.asm $(K)/kernel