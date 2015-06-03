.set mips1
.set noreorder

.text

.global main
.type main, function

/* Test modifying a target register inside the load delay slot
 *
 * Output on the real console:
 *    Value after load+move: 01234567
 *
 * Return 0 if we get the expected value, 1 otherwise.
 */

main:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s0, 20($sp)
	move  $fp, $sp

	/* Put a magic number in $s0 */
	li    $s0, 0x01234567
	la    $t0, test_word

	move  $a1, $zero

	/* Load test_word into $t0 and then change its value in the
	 * load delay slot */
	lw    $a1, 0($t0)
	move  $a1, $s0

	/* The value of the load should have been overriden by the
	 * move even though it took place in the delay slot */
	xor  $s0, $s0, $a1

	/* Display the value of $t0 during the delay slot */
	la    $a0, display_msg
	jal   bios_printf
	nop

	/* Set the return value */
	sltu  $v0, $zero, $s0

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s0, 20($sp)
	jr    $ra
	addiu $sp, 24

.data

display_msg:
.string "Value after load+move: %08x\n"

test_word:
.word 0xbadbaaad
