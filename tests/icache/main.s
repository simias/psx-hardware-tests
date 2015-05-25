.set mips1
.set noreorder

.text

.global main
.type main, function

/* Test instruction cache line fetch behaviour.
 *
 * Output on the real console:
 *
 *   Cacheline: 0x00000000 0x03e00008 0x00000000 0xbadc0ffe
 *   Cacheline: 0x00000000 0x00000000 0x00000000 0x00000000
 *
 * Note that the `0xdeadbeef` is not fetched even though it belongs in
 * the same cacheline
 */

main:
	addiu $sp, -24
	sw    $ra, 20($sp)
	sw    $fp, 16($sp)
	move  $fp, $sp

	/* Invalidate the cache */
	jal bios_flush_cache
	nop

	/* Jump to cache_test, mid cacheline */
	jal cache_test
	nop

	jal dump_icache
	nop

	/* Invalidate the cache once again */
	jal bios_flush_cache
	nop

	/* Everything should be 0 (assuming we're not unlucky with
	 * cache aliasing...) */
	jal dump_icache
	nop

	/* Set return value */
	li  $v0, 0

	move  $sp, $fp
	lw    $ra, 20($sp)
	lw    $fp, 16($sp)
	jr    $ra
	addiu $sp, 24

dump_icache:
	addiu $sp, -32
	sw    $ra, 28($sp)
	sw    $fp, 24($sp)
	move  $fp, $sp

	/* Save status register */
	mfc0 $t7, $12
	
	/* Move to KSEG1 (uncached region) */
	la  $t0, 1f
	lui $t1, 0xa000
	or  $t0, $t1
	jr  $t0
	nop
1:	/* We're now running from uncached memory */

	/* Save Cache Control register */
	li $t6, 0xfffe0130
	lw $t5, 0($t6)

	/* Enable I-Cache */
	li $t0, 0x800
	sw $t0, 0($t6)

	/* Isolate cache */
	li   $t1, 0x10000
	mtc0 $t1, $12
	nop
	/* Not sure why we need the 2nd NOP here but that's how the
	 * PSX BIOS does it... */
	nop

	/* Load cache aligned address of cache_test */
	la  $t0, cache_test
	li  $t1, ~0xf
	and $t0, $t1

	lw $a1, 0($t0)
	lw $a2, 4($t0)
	lw $a3, 8($t0)
	/* We can't store the 5th printf parameter directly on the
	 * stack since the cache is isolated */
	lw $t2, 12($t0)
	nop

	mtc0 $zero, $12
	nop

	/* Restore cache control and SR */
	sw   $t5, 0($t6)
	mtc0 $t7, $12
	nop

	/* Print the dumped values */
	sw  $t2, 16($sp)
	la  $a0, display_msg
	jal bios_printf
	nop

	/* Return */
	move  $sp, $fp
	lw    $ra, 28($sp)
	lw    $fp, 24($sp)
	jr    $ra
	addiu $sp, 32

/* The instruction cachelines are 4 words long. I put cache_test in
 * the middle of a line with two magic tokens before. Since the magic
 * words never get executed and the control jumps on the middle of the
 * cacheline they should never be fetched by the cache. */
.align 4
.word 0xdeadbeef
cache_test:
	jr $ra
	nop
.word 0xbadc0ffe

.data	

display_msg:
.string "Cacheline: 0x%08x 0x%08x 0x%08x 0x%08x\n"
