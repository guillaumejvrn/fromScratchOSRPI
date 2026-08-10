SECTIONS
{
    . = 0x80000;
    .text : ALIGN(16) {
        KEEP(*(.text.boot))
        *(.text .text.* .gnu.linkonce.t.*)
    }
    .rodata : ALIGN(16) {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
    }
    .data : ALIGN(16) {
        *(.data .data.* .gnu.linkonce.d.*)
    }
    .bss : ALIGN(16) {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        . = ALIGN(16);
        __bss_end = .;
    }
    __bss_size = (__bss_end - __bss_start) >> 3;
    . = ALIGN(4096);
    _end = .;
}