.set mips1
.set noreorder

.text

.global main
.type main, function

.EQU TIMER0_BASE, 0x1f801100

.EQU TIMER_COUNT,  0
.EQU TIMER_MODE,   4
.EQU TIMER_TARGET, 8

.macro bench_instruction
	lw    $0, 0($0)
.endm

main:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	li    $t0, TIMER0_BASE

	sw    $0, TIMER_MODE($t0)

	li    $a1, 0
	li    $t3, 0x100
1:
	/* Reset counter */
	sw    $0, TIMER_COUNT($t0)
	nop
	/* Execute load from RAM */
	bench_instruction
	bench_instruction
	bench_instruction
	bench_instruction
	bench_instruction
	bench_instruction
	bench_instruction
	bench_instruction
	bench_instruction
	bench_instruction
	/* Load counter value */
	lhu   $t1, TIMER_COUNT($t0)
	addi  $t3, $t3, -1
	/* Add to $a0 */
	add   $a1, $a1, $t1
	/* loop */
	bne   $t3, $0, 1b
	nop

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
.string "Total latency: %u\n"
