CC = aarch64-elf-gcc 
OBJCOPY = aarch64-elf-objcopy 
LD = aarch64-elf-ld 

CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -Isrc -Isrc/drivers

# On vire les variables complexes pour éviter les confusions de pattern
OBJECTS = build/boot.o build/main.o build/framebuffer.o

all: kernel8.img 

kernel8.img: build/kernel8.elf 
	$(OBJCOPY) -O binary build/kernel8.elf kernel8.img 

build/kernel8.elf: $(OBJECTS) 
	$(LD) -T./link.ld -o build/kernel8.elf $(OBJECTS)

# Compilation explicite sans aucune variable de dossier ambiguë
build/boot.o: src/boot.s
	@mkdir -p build
	$(CC) $(CFLAGS) -c src/boot.s -o build/boot.o

build/main.o: src/main.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c src/main.c -o build/main.o

build/framebuffer.o: src/drivers/framebuffer.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c src/drivers/framebuffer.c -o build/framebuffer.o

clean: 
	rm -rf build
	rm -f kernel8.img 

.PHONY: all clean