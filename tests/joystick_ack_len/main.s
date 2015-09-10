.set mips1
.set noreorder

.text

.global main
.type main, function

/* Measure a gamepad's ack pulse duration
 *
 * Displays the number of CPU cycles measured between the moment the
 * "ack" line goes down to the point it goes back up.
 *
 * /!\ Must be used with a controller plugged in slot 1, otherwise
 *     it'll deadlock.
 *
 * The actual value will vary from pad to pad, my two digital
 * SCPH-1080 gamepads respectively display:
 *
 *    Status ack: 0x0000f087, Status ~ack: 0x0001f807, Counter: 101
 *    Status ack: 0x00021087, Status ~ack: 0x00015007, Counter: 90
 *
 * My DualShock (SCPH-1200) is a bit quicker:
 *
 *    Status ack: 0x00008087, Status ~ack: 0x0000c807, Counter: 57
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

	/* Mask all interrupts */
	li    $s1, IRQ_BASE
	sh    $0, 4($s1)
	nop

	li    $s0, JOY_BASE

	/* Controller reset */
	li    $t0, 0x40
	sh    $t0, 0xa($s0)

	/* Set baud rate */
	li    $t0, 0x88
	sh    $t0, 0xe($s0)

	/* Delay needed for my DualShock, not sure why... */
	li    $t1, 0x30
3:
	bne   $t1, $0, 3b
	addiu $t1, -1

	/* Set mode */
	li    $t0, 0xd
	sh    $t0, 0x8($s0)

	/* TXen on joystick 1 */
	li    $t0, 0x3
	sh    $t0, 0xa($s0)

	/* Configure timer 0 to count sysclock ticks */
	li    $s1, TIMER0_BASE
	sh    $0, TIMER_MODE($s1)

	/* Send byte 0x01 */
	li    $t0, 0x1
	sb    $t0, 0x0($s0)

1:
	/* Wait for ack */
	lw    $a1, 0x4($s0)
	/* Reset counter */
	sh    $0, TIMER_COUNT($s1)
	andi  $t0, $a1, 0x80
	beq   $t0, $0, 1b
	nop

2:
	/* Wait for ~ack */
	lw    $a2, 0x4($s0)
	lhu   $a3, 0($s1)
	andi  $t0, $a2, 0x80
	bne   $t0, $0, 2b
	nop

	la    $a0, display_msg
	jal   bios_printf
	nop

	/* Set return value */
	li    $v0, 0

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

.data

display_msg:
.string "Status ack: 0x%08x, Status ~ack: 0x%08x, Counter: %u\n"
