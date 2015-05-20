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
