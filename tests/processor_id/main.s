.set mips1
.set noreorder

.text

.global main
.type main, function

main:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	mfc0  $a1, $15
	la    $a0, display
	jal   bios_printf
	nop

	/* Set return value */
	li  $v0, 0

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

.data

display:
.string "Processor ID (Cop0r15): 0x%08x\n"
