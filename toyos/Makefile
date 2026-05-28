K = kernel
U = user

TOOLPREFIX ?= riscv64-unknown-elf-
CC = $(TOOLPREFIX)gcc
LD = $(TOOLPREFIX)ld
OBJCOPY = $(TOOLPREFIX)objcopy
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
  $(K)/main.o \
  $(K)/sbi.o \
  $(K)/console.o \
  $(K)/printf.o \
  $(K)/string.o \
  $(K)/kalloc.o \
  $(K)/vm.o \
  $(K)/proc.o \
  $(K)/elf.o \
  $(K)/apps.o \
  $(K)/syscall.o \
  $(K)/trap.o \
  $(K)/trampoline.o \
  $(K)/apps.ld.o \
  $(U)/initcode.ld.o

UOBJS = \
  $(U)/initcode.o \
  $(U)/user.o \
  $(U)/usys.o

APPOBJS = \
  $(U)/appentry.o \
  $(U)/app.o \
  $(U)/user.o \
  $(U)/usys.o

$(K)/kernel: $(OBJS) $(K)/kernel.ld
	$(LD) $(LDFLAGS) -T $(K)/kernel.ld -o $(K)/kernel $(OBJS)
	$(OBJDUMP) -S $(K)/kernel > $(K)/kernel.asm

$(K)/%.o: $(K)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(K)/%.o: $(K)/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(K)/apps.ld.o: $(K)/apps.ld.S $(U)/app
	$(CC) $(CFLAGS) -c -o $@ $(K)/apps.ld.S

$(U)/usys.S: $(U)/usys.pl
	perl $(U)/usys.pl > $(U)/usys.S

$(U)/%.o: $(U)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(U)/%.o: $(U)/%.S
	$(CC) $(CFLAGS) -c -o $@ $<

$(U)/initcode: $(UOBJS) $(U)/user.ld
	$(LD) $(LDFLAGS) -T $(U)/user.ld -o $(U)/initcode $(UOBJS)
	$(OBJDUMP) -S $(U)/initcode > $(U)/initcode.asm

$(U)/app: $(APPOBJS) $(U)/user.ld
	$(LD) $(LDFLAGS) -T $(U)/user.ld -o $(U)/app $(APPOBJS)
	$(OBJDUMP) -S $(U)/app > $(U)/app.asm

$(U)/initcode.bin: $(U)/initcode
	$(OBJCOPY) -S -O binary $(U)/initcode $(U)/initcode.bin

$(U)/initcode.ld.o: $(U)/initcode.ld.S $(U)/initcode.bin
	$(CC) $(CFLAGS) -c -o $@ $(U)/initcode.ld.S

.PHONY: qemu clean

qemu: $(K)/kernel
	$(QEMU) -machine virt -kernel $(K)/kernel -m 128M -smp 1 -nographic

clean:
	rm -f $(K)/*.o $(K)/*.asm $(K)/kernel
	rm -f $(U)/*.o $(U)/*.asm $(U)/usys.S $(U)/initcode $(U)/initcode.bin $(U)/app
