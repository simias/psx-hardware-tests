.set mips1
.set noreorder

.text

.global main
.type main, function

/* Reset the MDEC and read its post-reset status.
 *
 * On my PAL SCPH-7502 console this displays:
 *
 *    MDEC status: 80040000
 *
 * Note that I have to wait a few cycles after the reset request to
 * get this "stable" status value. If I read it straight away I get
 * strange values, probably because the reset takes a few cycles
 * to complete.
 */

.EQU MDEC_BASE, 0x1f801820

main:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s1, 20($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

	li $s0, MDEC_BASE

	/* Changing this to 0xe0000000 (i.e. setting the other control
	   bits) doesn't change anything, the reset seems to override
	   them */
	li $t0, 0x80000000
	sw $t0, 4($s0)

	/* If we read back the status straight away we get
	   weird values, it seems the reset takes quite some time.  */
	nop
	nop
	nop

	lw $a1, 4($s0)
	la $a0, display
	jal bios_printf
	nop

	/* Set return value */
	li  $v0, 0

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32
.data

display:
.string "MDEC status: %08x\n"
