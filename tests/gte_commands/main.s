.set mips1
.set noreorder
.set noat

.text

.global main
.type main, function

.EQU GTE_SQR,   0x28
.EQU GTE_RTPT,  0x30

.macro GTE_OP op, lm=0, cv=0, v=0, mx=0, sf=0
	li    $a0, \op | (\lm << 10) | (\cv << 13) | (\v << 15) | (\mx << 17) | (\sf << 19)
	jal   gte_run_command
	nop
.endm

.macro GTE_DATA cop_r, val
	li    $at, \val
	mtc2  $at, \cop_r
.endm

.macro GTE_CONTROL cop_r, val
	li    $at, \val
	ctc2  $at, \cop_r
.endm

main:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	jal gte_init
	nop

	GTE_CONTROL $29, 0x155
	GTE_CONTROL $30, 0x100
	GTE_CONTROL $26, 0x3e8
	GTE_CONTROL $27, 0xffffef9e
	GTE_CONTROL $28, 0x1400000
	GTE_CONTROL $24, 0x0
	GTE_CONTROL $25, 0x0
	GTE_CONTROL $24, 0x1400000
	GTE_CONTROL $25, 0xf00000
	GTE_CONTROL $26, 0x400
	GTE_CONTROL $13, 0x640
	GTE_CONTROL $14, 0x640
	GTE_CONTROL $15, 0x640
	GTE_CONTROL $21, 0x0
	GTE_CONTROL $22, 0x0
	GTE_CONTROL $23, 0x0
	GTE_CONTROL $27, 0xfffff3cb
	GTE_CONTROL $28, 0x1400000
	GTE_CONTROL $16, 0xbb80fa0
	GTE_CONTROL $17, 0xfa00fa0
	GTE_CONTROL $18, 0xfa00bb8
	GTE_CONTROL $19, 0xbb80fa0
	GTE_CONTROL $20, 0xfa0
	GTE_CONTROL $8, 0x0
	GTE_CONTROL $9, 0xfa0
	GTE_CONTROL $10, 0xf060
	GTE_CONTROL $11, 0xf060
	GTE_CONTROL $12, 0x0
	GTE_CONTROL $0, 0xffb
	GTE_CONTROL $1, 0xffb7ff44
	GTE_CONTROL $2, 0xf9ca0ebc
	GTE_CONTROL $3, 0x63700ad
	GTE_CONTROL $4, 0xffff0eb7
	GTE_CONTROL $5, 0x0
	GTE_CONTROL $6, 0xfffffeac
	GTE_CONTROL $7, 0x1700
	GTE_CONTROL $27, 0xfffffec8
	GTE_CONTROL $28, 0x1400000
	GTE_DATA $0, 0xe70119
	GTE_DATA $1, 0xcacafe65
	GTE_DATA $2, 0xe700d5
	GTE_DATA $3, 0xcacafe21
	GTE_DATA $4, 0xb90119
	GTE_DATA $5, 0xcacafe65


	la    $a0, initial_header
	jal   bios_printf
	nop

	jal gte_dump
	nop

	la    $a0, initial_footer
	jal   bios_printf
	nop

	GTE_OP GTE_RTPT, lm=0, cv=0, v=0, mx=0, sf=1

	la    $a0, result_header
	jal   bios_printf
	nop

	jal gte_dump
	nop

	la    $a0, result_footer
	jal   bios_printf
	nop

	/* Set return value */
	li  $v0, 0

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

.global gte_run_command
.type gte_run_command, function

/* Parameter: GTE opcode word */
gte_run_command:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s0, 16($sp)
	move  $fp, $sp

	move  $s0, $a0
	move  $a1, $a0
	la    $a0, show_command
	jal   bios_printf
	nop

	/* self-modifying code: rewrite gte_trampoline */
	la    $t0, gte_trampoline
	/* Address in non-cached region */
	lui   $t1, 0xa000
	or    $t0, $t1

	/* Load, patch and rewrite c2 instruction */
	lw    $t1, 0($t0)
	nop
	or    $t1, $s0
	sw    $t1, 0($t0)

	/* Make sure the pipeline doesn't mess with us, not sure if
	 * it's really necessary (seems to work without those) */
	nop
	nop
	nop
	nop

	/* Take a deep breath and jump */
	jal   $t0
	nop

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s0, 16($sp)
	jr    $ra
	addiu $sp, 32

gte_trampoline:
	c2 0 /* dynamically modified operation */
	jr $ra
	nop

.global gte_init
.type gte_init, function

gte_init:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	/* Enable GTE in cop0 */
	mfc0  $t0, $12
	li    $t1, 0x40000000
	or    $t0, $t0, $t1
	mtc0  $t0, $12
	nop
	nop

	/* Clear all the data registers */
	mtc2  $0, $0
	mtc2  $0, $1
	mtc2  $0, $2
	mtc2  $0, $3
	mtc2  $0, $4
	mtc2  $0, $5
	mtc2  $0, $6
	mtc2  $0, $7
	mtc2  $0, $8
	mtc2  $0, $9
	mtc2  $0, $10
	mtc2  $0, $11
	mtc2  $0, $12
	mtc2  $0, $13
	mtc2  $0, $14
	mtc2  $0, $15
	mtc2  $0, $16
	mtc2  $0, $17
	mtc2  $0, $18
	mtc2  $0, $19
	mtc2  $0, $20
	mtc2  $0, $21
	mtc2  $0, $22
	mtc2  $0, $23
	mtc2  $0, $24
	mtc2  $0, $25
	mtc2  $0, $26
	mtc2  $0, $27
	mtc2  $0, $28
	/* GTE register $29 is read only but writing 0 to $28 will
	 * clear it */
	mtc2  $0, $30
	/* GTE data register $31 is read-only, it contains the leading
	 * zeroes/ones of GTE data register $30, since we write 0 it'll
	 * be equal to 32 (0x20) */

	/* Clear all the control registers */
	ctc2  $0, $0
	ctc2  $0, $1
	ctc2  $0, $2
	ctc2  $0, $3
	ctc2  $0, $4
	ctc2  $0, $5
	ctc2  $0, $6
	ctc2  $0, $7
	ctc2  $0, $8
	ctc2  $0, $9
	ctc2  $0, $10
	ctc2  $0, $11
	ctc2  $0, $12
	ctc2  $0, $13
	ctc2  $0, $14
	ctc2  $0, $15
	ctc2  $0, $16
	ctc2  $0, $17
	ctc2  $0, $18
	ctc2  $0, $19
	ctc2  $0, $20
	ctc2  $0, $21
	ctc2  $0, $22
	ctc2  $0, $23
	ctc2  $0, $24
	ctc2  $0, $25
	ctc2  $0, $26
	ctc2  $0, $27
	ctc2  $0, $28
	ctc2  $0, $29
	ctc2  $0, $30
	ctc2  $0, $31

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

.macro dump_c cop_r
	cfc2  $a2, $\cop_r
	li    $a1, \cop_r
	/* Don't dump if register is 0 */
	beqz  $a2, 1f
	nop
	la    $a0, dump_word
	jal   bios_printf
	nop
1:
.endm

.macro dump_d cop_r
	mfc2  $a2, $\cop_r
	li    $a1, \cop_r
	/* Don't dump if register is 0 */
	beqz  $a2, 1f
	nop
	la    $a0, dump_word
	jal   bios_printf
	nop
1:
.endm

.global gte_dump
.type gte_dump, function

gte_dump:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	la    $a0, dump_controls
	jal   bios_printf
	nop

	dump_c 0
	dump_c 1
	dump_c 2
	dump_c 3
	dump_c 4
	dump_c 5
	dump_c 6
	dump_c 7
	dump_c 8
	dump_c 9
	dump_c 10
	dump_c 11
	dump_c 12
	dump_c 13
	dump_c 14
	dump_c 15
	dump_c 16
	dump_c 17
	dump_c 18
	dump_c 19
	dump_c 20
	dump_c 21
	dump_c 22
	dump_c 23
	dump_c 24
	dump_c 25
	dump_c 26
	dump_c 27
	dump_c 28
	dump_c 29
	dump_c 30
	dump_c 31

	la    $a0, dump_end
	jal   bios_printf
	nop

	la    $a0, dump_data
	jal   bios_printf
	nop

	dump_d 0
	dump_d 1
	dump_d 2
	dump_d 3
	dump_d 4
	dump_d 5
	dump_d 6
	dump_d 7
	dump_d 8
	dump_d 9
	dump_d 10
	dump_d 11
	dump_d 12
	dump_d 13
	dump_d 14
	dump_d 15
	dump_d 16
	dump_d 17
	dump_d 18
	dump_d 19
	dump_d 20
	dump_d 21
	dump_d 22
	dump_d 23
	dump_d 24
	dump_d 25
	dump_d 26
	dump_d 27
	dump_d 28
	dump_d 29
	dump_d 30
	dump_d 31

	la    $a0, dump_end
	jal   bios_printf
	nop

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24


.data

initial_header:
.string "    Test {\n        initial: Config {\n"

initial_footer:
.string "        },\n"

result_header:
.string "        result: Config {\n"

result_footer:
.string "        },\n    },\n"

dump_controls:
.string "            controls: &[\n"

dump_data:
.string "            data: &[\n"

show_command:
.string "        command: 0x%08x,\n"

dump_word:
.string "                (%d, 0x%08x),\n"

dump_end:
.string "                ],\n"
