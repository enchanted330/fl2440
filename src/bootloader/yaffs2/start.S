
/********************************************************************************************
 *        File:  start.S - Startup Code for ARM920 CPU-core
 *     Version:  1.0.0
 *   Copyright:  2011 (c) Guo Wenxue <guowenxue@gmail.com>
 * Description:  When system power up, the CPU will comes here to excute the first code here.
 *   ChangeLog:  1, Release initial version on "Tue Jul 12 16:43:18 CST 2011"
 *
 *******************************************************************************************/


/*
 *************************************************************************
 *
 * Jump vector table as in table 3.1 in [1]
 *
 *************************************************************************
 */

.globl _start
_start:	b	start_code

_TEXT_BASE:
    .word   TEXT_BASE 
    
.globl _armboot_start
_armboot_start:
    .word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

start_code:
	/* Set up the stack	*/
stack_setup:
	ldr	r0, =TEXT_BASE		/* upper 128 KiB: relocated uboot   */
	sub	r0, r0, #CONFIG_SYS_MALLOC_LEN	/* malloc area              */
	sub	sp, r0, #12		/* leave 3 words for abort-stack    */
	bic	sp, sp, #7		/* 8-byte alignment for ABI compliance */

clear_bss:
	ldr	r0, _bss_start		/* find start of bss segment        */
	ldr	r1, _bss_end		/* stop here                        */
	mov	r2, #0x00000000		/* clear                            */

clbss_l:str	r2, [r0]		/* clear loop...                    */
	add	r0, r0, #4
	cmp	r0, r1
	ble	clbss_l

    bl  bootstrap_main

