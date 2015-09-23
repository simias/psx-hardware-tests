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
.equ IRQ_BASE,   0x1f801070

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

	li    $s0, CDROM_BASE
	li    $s1, IRQ_BASE

	/* Mask top level CDROM interrupt */
	lhu   $t0, 4($s1)
	nop
	andi  $t0, $t0, 0xfffb
	sh    $t0, 4($s1)
	nop

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

	/* Set loc */
	li    $a0, 0
	li    $a1, 2
	li    $a2, 4
	jal   set_loc
	nop

	jal   seek_l
	nop

	/* Set mode */
	li    $a0, 0x0
	jal   set_mode
	nop

	/* Read sector in static buffer */
	jal   read_sector
	nop

	/* Dump sector data */
	li   $s0, 0
	la   $s1, sector_buffer

1:
	lbu   $a1, 0($s1)
	la    $a0, display_byte
	jal   bios_printf
	nop

	addiu $s0, 1
	addiu $s1, 1

	/* Display 32 bytes per line */
	andi  $t0, $s0, 0x1f
	bnez  $t0, 1b
	nop

	/* Display newline */
	la    $a0, newline
	jal   bios_printf
	nop

	li    $t0, 2048
	bne   $s0, $t0, 1b
	nop

	/* Set return value */
	/*li  $v0, 0*/

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

/* u8 set_loc(u8 mm, u8 ss, u8 ff)
 *
 * Returns the interrupt status in v0
 */
.type set_loc, function

set_loc:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s1, 20($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

	li    $s0, CDROM_BASE

	/* Select index 0 */
	sb    $0, 0($s0)

	/* Send command 0x02: set loc */
	sb    $a0, 2($s0)
	sb    $a1, 2($s0)
	sb    $a2, 2($s0)
	li    $t0, 0x02
	sb    $t0, 1($s0)

	/* Select index 1 */
	li    $t0, 1
	sb    $t0, 0($s0)

1:	/* Wait for interrupt */
	lbu   $v0, 3($s0)
	nop
	andi  $v0, 0x7
	beqz  $v0, 1b
	nop

	/* Discard response */
	lbu   $0, 1($s0)

	/* Ack CDROM interrupt */
	li    $t0, 0x1f
	sb    $t0, 3($s0)

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

/* u8 seek_l(void)
 *
 * Returns the 2nd interrupt status in v0
 */
.type seek_l, function

seek_l:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s1, 20($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

	li    $s0, CDROM_BASE

	/* Select index 0 */
	sb    $0, 0($s0)

	/* Send command 0x15: seek_l */
	li    $t0, 0x15
	sb    $t0, 1($s0)

	/* Select index 1 */
	li    $t0, 1
	sb    $t0, 0($s0)

1:	/* Wait for interrupt */
	lbu   $v0, 3($s0)
	nop
	andi  $v0, 0x7
	beqz  $v0, 1b
	nop

	/* Discard response */
	lbu   $0, 1($s0)

	/* Ack CDROM interrupt */
	li    $t0, 0x1f
	sb    $t0, 3($s0)

1:	/* Wait for interrupt */
	lbu   $v0, 3($s0)
	nop
	andi  $v0, 0x7
	beqz  $v0, 1b
	nop

	/* Discard 2nd response */
	lbu   $0, 1($s0)

	/* Ack CDROM interrupt */
	li    $t0, 0x1f
	sb    $t0, 3($s0)

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

/* u8 set_mode(u8 mode)
 *
 * Returns the interrupt status in v0
 */
.type set_mode, function

set_mode:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s1, 20($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

	li    $s0, CDROM_BASE

	/* Select index 0 */
	sb    $0, 0($s0)

	/* Send command 0x0e: set mode */
	sb    $a0, 2($s0)
	li    $t0, 0x0e
	sb    $t0, 1($s0)

	/* Select index 1 */
	li    $t0, 1
	sb    $t0, 0($s0)

1:	/* Wait for interrupt */
	lbu   $v0, 3($s0)
	nop
	andi  $v0, 0x7
	beqz  $v0, 1b
	nop

	/* Discard response */
	lbu   $0, 1($s0)

	/* Ack CDROM interrupt */
	li    $t0, 0x1f
	sb    $t0, 3($s0)

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

/* void read_sector(void) */
.type set_loc, function

read_sector:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s1, 20($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

	li    $s0, CDROM_BASE

	/* Select index 0 */
	sb    $0, 0($s0)

	/* Send command 0x06: readN */
	li    $t0, 0x06
	sb    $t0, 1($s0)

	/* Select index 1 */
	li    $t0, 1
	sb    $t0, 0($s0)

1:	/* Wait for interrupt */
	lbu   $v0, 3($s0)
	nop
	andi  $v0, 0x7
	beqz  $v0, 1b
	nop

	/* Discard response */
	lbu   $0, 1($s0)

	/* Ack CDROM interrupt */
	li    $t0, 0x1f
	sb    $t0, 3($s0)

1:	/* Wait for interrupt */
	lbu   $v0, 3($s0)
	nop
	andi  $v0, 0x7
	beqz  $v0, 1b
	nop

	/* Discard 2nd response */
	lbu   $0, 1($s0)

	/* Select index 0 */
	li    $t0, 0
	sb    $t0, 0($s0)

	/* Set "want data" bit */
	li    $t0, 0x80
	sb    $t0, 3($s0)

	la    $s1, sector_buffer
2:	/* Wait for data */
	lbu   $t0, 0($s0)
	nop
	andi  $t0, 0x40
	beqz  $t0, 2b
	nop

	/* Got data */
	lbu   $t0, 2($s0)
	nop
	sb    $t0, 0($s1)

	/* Wait for next data unless we reached the end of the sector */
	la    $t0, sector_buffer_end - 1
	bne   $s1, $t0, 2b
	addiu $s1, 1

	/* Clear "want bit" */
	sb    $0, 3($s0)

	/* Ack CDROM interrupt */
	li    $t0, 0x1f
	sb    $t0, 3($s0)

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

.data

display_byte:
.string "%02x "

newline:
.string "\n"

sector_buffer:
.skip 2048
sector_buffer_end:
