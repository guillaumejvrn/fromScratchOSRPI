SECTIONS
{
    . = 0x80000;
    .text : { KEEP(*(.text.boot)) *(.text .text.* .gnu.linkonce.t.*) }
    .rodata : { *(.rodata .rodata.* .gnu.linkonce.r.*) }
    .data : { *(.data .data.* .gnu.linkonce.d.*) }
    .bss : {
        __bss_start = .;
        *(.bss .bss.*)
        *(COMMON)
        __bss_end = .;
    }
    _end = .;
}
