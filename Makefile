# -----------------------------------------------------------------------------
# Compilator and Tools Definitions
# -----------------------------------------------------------------------------
CC = aarch64-elf-gcc
OBJCOPY = aarch64-elf-objcopy
LD = aarch64-elf-ld

# -----------------------------------------------------------------------------
# Compilation Flags (Including src/ for easy headers resolution)
# -----------------------------------------------------------------------------
CFLAGS = -Wall -O2 -ffreestanding -nostdlib -nostartfiles -Isrc

# -----------------------------------------------------------------------------
# Output and Build Directories
# -----------------------------------------------------------------------------
BUILD_DIR = build
SRC_DIR = src

# -----------------------------------------------------------------------------
# Source Files Selection
# -----------------------------------------------------------------------------
SOURCES_ASM = $(SRC_DIR)/boot.s
SOURCES_C   = $(SRC_DIR)/main.c \
              $(SRC_DIR)/drivers/uart.c \
              $(SRC_DIR)/drivers/mailbox.c \
              $(SRC_DIR)/drivers/framebuffer.c

# -----------------------------------------------------------------------------
# Map Source Files to Object Files inside the Build Directory
# -----------------------------------------------------------------------------
OBJECTS = $(patsubst $(SRC_DIR)/%.s, $(BUILD_DIR)/%.o, $(SOURCES_ASM)) \
          $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SOURCES_C))

# -----------------------------------------------------------------------------
# Main Target Rules
# -----------------------------------------------------------------------------
all: kernel8.img

# Generate the final binary image from the link layout
kernel8.img: $(BUILD_DIR)/kernel8.elf
	$(OBJCOPY) -O binary $(BUILD_DIR)/kernel8.elf kernel8.img

# Link all object files together inside the build directory
$(BUILD_DIR)/kernel8.elf: $(OBJECTS)
	$(LD) -T./link.ld -o $(BUILD_DIR)/kernel8.elf $(OBJECTS)

# Compile Assembly source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile C source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# -----------------------------------------------------------------------------
# Clean Rule
# -----------------------------------------------------------------------------
clean:
	rm -rf $(BUILD_DIR)
	rm -f kernel8.img