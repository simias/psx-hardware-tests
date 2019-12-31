.set mips1
.set noreorder

.text

.global main
.type main, function

main:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s1, 20($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

        /* IRQ base */
        li $s0, 0x1f801070

        /* Clear mask */
        sw $0, 4($s0)
        /* Ack everything */
        sw $0, 0($s0)

        /* Dummy bus read to get a known previous value */
        la $t0, dummy_val
        lw $0, 0($t0)
        nop

        lw $a2, 0($s0)
        lw $a3, 4($s0)
        nop

        move $a1, $s0

        /* The two IRQ registers are 16 bit wide. The 16 MSBs aren't defined,
         * which in practice seems to mean that they maintain the value of the
         * previous read. In other words the two reads above should both have
         * returned 0xabcd0000 (both 16 bit registers being cleared to 0 at the
         * start of this code, and the 16 bits of dummy_val for the MSBs). This
         * appear to be true regardless of which region is used to read the
         * registers. */

        /* Set $s0 to 0 if both registers read 0xabcd0000 */
        lui $t1, 0xabcd
        xor $s0, $a2, $t1
        xor $t0, $a3, $t1
        or  $s0, $s0, $t0

        la $a0, display_regs

        jal bios_printf
        nop

	/* Set return value */
	move  $v0, $s0

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s1, 20($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

.data

dummy_val:
.word 0xabcdef01

display_regs:
.string "Reading through 0x%08x: 0x%08x 0x%08x\n"
