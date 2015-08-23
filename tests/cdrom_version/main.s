.set mips1
.set noreorder

.text

.global main
.type main, function

/* Read the CDROM controller's version.
 *
 * This is almost exactly the first CDROM command sequence used by the
 * BIOS. That's why I left the IRQ Ack/Enable even though they're not
 * used in this particular test since I'm polling the response status
 * flag.
 *
 * On my PAL SCPH-7502 console this displays:
 *
 *    Response: 0x98
 *    Response: 0x06
 *    Response: 0x10
 *    Response: 0xc3
 *
 * Based on the Nocash spec the first three bytes are a year, month
 * and day (1998-06-10?) while the fourth byte is the version (0xc3)
 */

.equ CDROM_BASE, 0x1f801800
.equ IRQ_MASK,   0x1f801074

main:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s1, 20($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

	/* Mask CDROM interrupt */
	li    $s0, IRQ_MASK
	lhu   $t0, 0($s0)
	nop
	andi  $t0, $t0, 0xfffb
	sh    $t0, 0($s0)
	nop

	li    $s0, CDROM_BASE

	/* Select index 1 */
	li    $t0, 1
	sb    $t0, 0($s0)

	/* Ack interrupts */
	li    $t1, 0x1f
	sb    $t1, 3($s0)

	/* Enable interrupts */
	sb    $t1, 2($s0)

	/* Reset parameter FIFO */
	li    $t2, 0x40
	sb    $t2, 3($s0)

	/* Select index 0 */
	sb    $zero, 0($s0)

	/* Send command 0x19 0x20: Get BIOS date/version */
	li    $t0, 0x20
	sb    $t0, 2($s0)
	li    $t2, 0x19
	sb    $t2, 1($s0)

	/* We expect 4 response bytes */
	li    $s1, 4

	/* Wait for response byte */
1:
	lbu   $t1, 0($s0)
	nop
	andi  $t1, 0x20
	beq   $t1, $zero, 1b
	nop

	/* Select index 1 */
	li    $t0, 1
	sb    $t0, 0($s0)

	/* Read response byte */
	lbu   $a1, 1($s0)
	la    $a0, display_msg
	jal   bios_printf
	nop

	/* Wait for next byte (if any) */
	addiu $s1, $s1, -1
	bne   $s1, $0, 1b
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

display_msg:
.string "Response: 0x%02x\n"
