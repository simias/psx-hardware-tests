.set mips1
.set noreorder

.text

.global main
.type main, function

/* Test the BIOS delay slots
 *
 * Output on the real console:
 *    Value in delay slot: 01234567
 *
 * Return 0 if we get the expected value, 1 otherwise.
 */

main:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s0, 20($sp)
	move  $fp, $sp

	/* Put a test value in $t0 */
	li    $s0, 0x01234567
	move  $t0, $s0
	la    $t1, test_word

	/* Load test_word into $t0 and save its value in the load
	 * delay slot */
	lw    $t0, 0($t1)
	move  $a1, $t0

	/* The value shouldn't have changed in the slot so s0 should
	 * be 0 after the xor */
	xor  $s0, $s0, $a1

	/* Display the value of $t0 during the delay slot */
	la    $a0, display_msg
	jal   bios_printf
	nop

	/* Set the return value*/
	sltu $v0, $zero, $s0

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s0, 20($sp)
	jr    $ra
	addiu $sp, 24

.data

display_msg:
.string "Value in delay slot: %08x\n"

test_word:
.word 0xbadbaaad
