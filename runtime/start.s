.set mips1
.set noreorder

.text

.global __start
.type __start, function

__start:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	/* Initialize the heap using the BIOS InitHeap function */
	la   $a0, _end_of_program_data
	la   $a1, _stack_bottom
	subu $a1, $a1, $a0
	jal  0xa0
	li   $t1, 0x39

	/* Jump into the program's main function */
	jal main
	nop

	/* Display program status */
	la   $a0, end_msg
	jal  bios_printf
	move $a1, $v0

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

.data

end_msg:
.string "Application exited with code %d\n"
