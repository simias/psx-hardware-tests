.set mips1
.set noreorder

.text

.global main
.type main, function

/* Test the joystick controller's reset behavior
 *
 * Bit 6 of the joystick control register can be used to soft-reset
 * the controller. This should clear the mode and control
 * registers. If other bits are set along bit 6 in the control
 * register they seem to be ignored, we always read 0 back.
 *
 * Output on the real hardware:
 *
 *    Joy control: 0000
 *    Joy mode: 00
 *    Joy baud: 00
 */

.equ JOY_BASE, 0x1f801040
.equ IRQ_BASE, 0x1f801070

.EQU TIMER0_BASE, 0x1f801100
.EQU TIMER1_BASE, 0x1f801110
.EQU TIMER2_BASE, 0x1f801120

.EQU TIMER_COUNT,  0
.EQU TIMER_MODE,   4
.EQU TIMER_TARGET, 8


main:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s1, 20($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

	mtc0  $0, $12

	/* Mask all interrupts */
	li    $s1, IRQ_BASE
	sh    $0, 4($s1)
	nop

	li    $s0, JOY_BASE

	/* Controller reset */
	li    $t0, 0x40
	sh    $t0, 0xa($s0)

	/* Set baud rate */
	li    $t0, 0x188
	sh    $t0, 0xe($s0)

	/* Set mode */
	li    $t0, 0xd
	sh    $t0, 0x8($s0)

	/* Set ctrl to 0 (redundant) */
	sh    $0, 0xa($s0)

	/* Select controller 0 */
	li    $t0, 0x2
	sh    $t0, 0xa($s0)

	/* Select controller 1 */
	li    $t0, 0x2002
	sh    $t0, 0xa($s0)

	/* Deselect everything */
	sh    $0, 0xa($s0)

	/* Select controller 0 */
	li    $t0, 0x2
	sh    $t0, 0xa($s0)

	/* TXen on joystick 1 */
	li    $t0, 0x1003
	sh    $t0, 0xa($s0)

	/* Ack interrupts */
	mtc0  $0, $12
	sh    $0, 4($s1)
	sw    $0, 0($s1)
	li    $t0, 0x1013
	sh    $t0, 0xa($s0)

	/* Send byte 0x81 (select memory card) */
	li    $t0, 0x81
	sb    $t0, 0x0($s0)

	la    $a0, debug_msg
	jal   bios_printf

1:
	/* Wait for ack */
	lw    $a1, 0x4($s0)
	nop
	andi  $t0, $a1, 0x80
	beq   $t0, $0, 1b
	nop

	/* Ack at top level */
	sh    $0, 0($s1)
	/* Ack in controller */
	li    $t0, 0x1013
	sh    $t0, 0xa($s0)
	nop
	nop
	nop
	lhu   $a2, 4($s0)

	/* Send byte 0xcc */
	li    $t0, 0xcc
	sb    $t0, 0x0($s0)

	jal   dump_state
	nop
end:

	/* Set return value */
	li    $v0, 0

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

dump_state:
	move  $s1, $ra

1:
	/* Wait for data in RX*/
	lw    $a1, 0x4($s0)
	nop
	andi  $t3, $a1, 2

	beq   $t3, $0, 1b
	nop

	/* Read joy data */
	lbu   $a2, 0x0($s0)
	la    $a0, display_data
	jal   bios_printf
	nop

	/* Ack irq */
	lhu   $t3, 0xa($s0)
	nop
	ori   $t3, $t3, 0x10
	sh    $t3, 0xa($s0)

	move  $ra, $s1
	jr    $ra
	nop

dump_state_nodata:
	move  $s1, $ra

1:
	/* Read joy data */
	lw   $a1, 0x4($s0)
	la    $a0, display_status
	jal   bios_printf
	nop

	move  $ra, $s1
	jr    $ra
	nop

.data

display_data:
.string "Status: %08x Data: %02x\n"

display_status:
.string "Status: %08x\n"

display_ack:
.string "Status: %08x, irq: %04x\n"

debug_msg:
.string "Waiting for ack...\n"
