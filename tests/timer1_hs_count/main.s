.set mips1
.set noreorder

.text

.global main
.type main, function

/* Measure the average line length */

.EQU TIMER0_BASE, 0x1f801100
.EQU TIMER1_BASE, 0x1f801110
.EQU TIMER2_BASE, 0x1f801120

.EQU TIMER_COUNT,  0
.EQU TIMER_MODE,   4
.EQU TIMER_TARGET, 8

main:
	addiu $sp, -40
	sw    $ra, 36($sp)
	sw    $fp, 32($sp)
	sw    $s2, 28($sp)
	sw    $s1, 24($sp)
	sw    $s0, 20($sp)
	move  $fp, $sp

	li    $a0, 0x08000000
	jal   bios_gpu_gp1_command_word
	nop

2:
	li    $s0, TIMER1_BASE  /* hsync */
	li    $s1, TIMER2_BASE  /* dotclk */

	/* Select alternate source for both timers */
	li    $t0, 0x300

	sw    $t0, TIMER_MODE($s0)
	sw    $t0, TIMER_MODE($s1)

	sw    $zero, TIMER_COUNT($s0)
	sw    $zero, TIMER_COUNT($s1)

	/* Wait until the beginning of the next line*/
1:	lhu   $a1, TIMER_COUNT($s0) /* Load hsync count */
	lhu   $a2, TIMER_COUNT($s1) /* Load dotclk */
	beq   $a1, $zero, 1b
	nop

	/* Reset everything and count 100 lines */
	sw    $zero, TIMER_COUNT($s0)
	sw    $zero, TIMER_COUNT($s1)
	li    $s2, 100

1:	lhu   $a1, TIMER_COUNT($s0) /* Load hsync count */
	lhu   $a2, TIMER_COUNT($s1) /* Load dotclk */
	bne   $a1, $s2, 1b
	nop

	la    $a0, display_timers
	jal   bios_printf
	move  $s2, $a2

	b     2b

	li    $v0, 0

	move  $sp, $fp
	lw    $ra, 36($sp)
	lw    $fp, 32($sp)
	lw    $s2, 28($sp)
	lw    $s1, 24($sp)
	lw    $s0, 20($sp)
	jr    $ra
	addiu $sp, 40

.data

display_timers:
.string "Hsync: %u, timer: %u\n"
