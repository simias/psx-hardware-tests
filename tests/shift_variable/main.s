.set mips1
.set noreorder

.text

.global main
.type main, function

/* Test the behavior of SLLV when the shift value is out of range
 *
 * Output on the real console:
 *    Success
 *
 * The shift amount should always be truncated to the 5 LSBs. Since
 * testing all 2**32 possible values takes a long time I only test up
 * to 0x1000000.
 *
 * Returns 0 on success, 1 on failure
 */


main:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	move  $t0, $zero
	/* Load $t1 with full ones */
	not   $t1, $zero
	/* Target value */
	li    $t5, 0x1000000

	/* Test that $t1 << $t0 is equal to $t1 << ($t0 & 0x1f) for
	 * all values of $t0 up to $t5 */
1:	sllv  $t2, $t1, $t0
	andi  $t3, $t0, 0x1f
	sllv  $t4, $t1, $t3

	xor   $t2, $t2, $t4

	bne   $t2, $zero, 2f

	/* Test up to target */
	bne   $t0, $t5, 1b
	addiu $t0, $t0, 1

	/* We're done */
	la     $a0, success_msg
	jal    bios_printf
	nop

	b      3f
	move   $v0, $zero

2:	/* Error */
	la    $a0, error_msg
	jal   bios_printf
	addiu $a1, $t0, -1

	li    $v0, 1

3:	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

.data

success_msg:
.string "Success\n"

error_msg:
.string "Error: 0x%08x\n"
