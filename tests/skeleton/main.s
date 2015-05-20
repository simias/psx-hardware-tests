.set mips1
.set noreorder

.text

.global main
.type main, function

main:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	/* Display a message using the BIOS's printf implementation */
	lui $a0, %hi(hello)
	jal bios_printf
	ori $a0, %lo(hello)

	/* Set drawing area top left at 0,0 */
	jal bios_gpu_command_word
	lui $a0, 0xe300

	/* Set drawing area bottom right at 256,256 */
	lui $a0, 0xe404
	jal bios_gpu_command_word
	ori $a0, 0x0100

	/* Set drawing offset at 0,0 */
	jal bios_gpu_command_word
	lui $a0, 0xe500

	/* Draw quad */
	la $a0, shaded_quad_cmd
	jal bios_gpu_command_word_and_params
	li $a1, 8

	/* Loop for a while to give us the time to see the quad before
	we return */
	li $t0, 10000000
	bnez $t0, .
	addiu $t0, -1

	/* Set return value */
	li  $v0, 0

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

.data

hello:
.string "Hello, world!\n"

shaded_quad_cmd:
.word 0x38000000, 0x00000000, 0x00ff0000, 0x00000100
.word 0x0000ff00, 0x01000000, 0x000000ff, 0x01000100
