.set mips1
.set noreorder

.text

.global main
.type main, function

/*
 * Check GPU state after the GP1 Reset command by dumping the GPUSTAT
 * register and using the GP1(0x10) "get GPU info" command
 */
main:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	/* Dump initial GPU info */
	la $a0, base_msg
	jal bios_printf
	nop

	jal dump_gpu_info
	nop

	/* Send reset to the GPU */
	jal bios_gpu_gp1_command_word
	li $a0, 0

	la $a0, reset_msg
	jal bios_printf
	nop

	jal dump_gpu_info
	nop

	/* Enable display */
	jal bios_gpu_gp1_command_word
	li $a0, 0x03000000

	la $a0, enable_msg
	jal bios_printf
	nop

	jal dump_gpu_info
	nop

	/* Set return value */
	li  $v0, 0

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

dump_gpu_info:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	sw    $s0, 20($sp)
	move  $fp, $sp

	/* Dump GPUSTAT register */
	jal bios_gpu_get_status
	nop

	la $a0, display_status
	jal bios_printf
	move $a1, $v0

	/* Loop from 0 to 8 to get all the available info from the
	 * GP1(0x10) command */
	li $s0, 0
1:
	jal  get_gpu_info
	move $a0, $s0

	la $a0, display_info
	move $a1, $s0
	jal bios_printf
	move $a2, $v0

	sltiu $t0, $s0, 9
	bnez  $t0, 1b
	addiu $s0, 1

	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	lw    $s0, 20($sp)
	jr    $ra
	addiu $sp, 32

get_gpu_info:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	/* Request GPU info */
	li    $t0, 0x10000000
	jal   bios_gpu_gp1_command_word
	or    $a0, $a0, $t0

	/* Read value from GPUREAD */
	lui   $t0, 0x1f80
	lw    $v0, 0x1810($t0)

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

.data

base_msg:
.string "\nInitial GPU info:\n"

reset_msg:
.string "\nGPU info after reset:\n"

enable_msg:
.string "\nGPU info after display enable:\n"

display_status:
.string "Status: 0x%08x\n"

display_info:
.string "GP1(0x1000000%d): 0x%08x\n"
