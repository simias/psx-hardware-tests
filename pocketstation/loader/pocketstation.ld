MEMORY
{
    /* RAM, first 512 bytes are reserved for kernel */
    RAM (rwx) : ORIGIN = 512, LENGTH = 2 * 1024 - 512,
    /* Max flash size 15 blocks of 8KB */
    FLASH (rx) : ORIGIN = 0x02000000, LENGTH = 15 * 8 * 1024
}

ENTRY(_start);

SECTIONS
{
	.header :
	{
		KEEP(*(.header.title))
		KEEP(*(.header.icon))
		KEEP(*(.header.fn_table))
		KEEP(*(.header.mono_icon_descs))
	} > FLASH

	.text :
	{
		. = ALIGN(4);
		*(.text*)
	} > FLASH

	.rodata :
	{
		. = ALIGN(4);
		*(.rodata*)
		*(.bss*)

		_ldata = .;
	} > FLASH

	.data :
	  AT(_ldata)
	{
		. = ALIGN(4);
		_sdata = .;

		*(.data*)

		_edata = .;
	} > RAM

	.bss (NOLOAD) :
	{
		. = ALIGN(4);
		_sbss = .;

		*(.bss*)
		*(COMMON)

		_ebss = .;
	} > RAM
}
