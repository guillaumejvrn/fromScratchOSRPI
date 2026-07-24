CC = aarch64-elf-gcc
OBJCOPY = aarch64-elf-objcopy
LD = aarch64-elf-ld

CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -Isrc -Isrc/drivers -Isrc/kernel -Isrc/arch

# Sources Assembleur
SRCS_S = src/boot.s src/arch/vectors.s

# Sources C
SRCS_C = src/main.c \
         src/drivers/framebuffer.c \
         src/drivers/font.c \
         src/drivers/console.c \
         src/drivers/timer.c \
         src/kernel/exception.c

OBJS = $(patsubst src/%.s, build/%.o, $(SRCS_S)) \
       $(patsubst src/%.c, build/%.o, $(SRCS_C))

all: kernel8.img

kernel8.img: build/kernel8.elf
	$(OBJCOPY) -O binary build/kernel8.elf kernel8.img

build/kernel8.elf: $(OBJS)
	$(LD) -T link.ld -o $@ $(OBJS)

build/%.o: src/%.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/
	rm -f kernel8.img

.PHONY: all clean