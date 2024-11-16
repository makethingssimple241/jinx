ENTRY(entry)

SECTIONS
{
    . = 0xffffffff80000000;

    .text : {
        *(.text)
    }

    /* Read-only data */
    .rodata : {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data : {
        *(.data)

        KEEP(*(.requests_start_marker))
        KEEP(*(.requests))
        KEEP(*(.requests_end_marker))
    }

    /* Read-write data (uninitialized) */
    .bss : {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    }
}
