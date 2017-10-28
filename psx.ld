MEMORY
{
    /* RAM through the KSEG0 region: 2MB minus 64K reserved for the BIOS */
    RAM (rwx) : ORIGIN = 0x80000000 + 0x10000, LENGTH = 0x200000 - 0x10000
}

ENTRY(__start);

SECTIONS
{
	.text :
	{
		*(.text*)
	} > RAM

	/* The PSX doesn't have an MMU so we don't really care about
	 * RO vs RW
	 */
	.data :
	{
		*(.data*)
		*(.rodata*)
	} > RAM

	/* This section must be stored in the "memfill" part of the
	 * header to be initialized to 0 at bootup by the BIOS/loader.
	 */
	.bss :
	{
		*(.bss*)
		*(COMMON)
	} > RAM

	PROVIDE(_end_of_program_data = (. + 3) & ~ 3);

	/* Use 4k for the stack */
	PROVIDE(_stack_bottom = ORIGIN(RAM) + LENGTH(RAM) - 4096);
}
