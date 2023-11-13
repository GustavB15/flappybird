  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
.global hexasc
.global delay
.global time2string
.global enable_interrupt
.text
hexasc:
	slti	$t0,$a1,10	# if the value inserted in a0 is smaller than 10 then t0 is 1
	beq	$t0,1,number	# if t0 is 1 then jump to subroutine number
	addi	$v0,$a1,0x37
	jr	$ra
	nop
  
number:
	addi	$v0,$a1,0x30
	jr	$ra
	nop

delay:
	li	$t0,5330	# constant for amount of loopss
	slt	$t1,$0,$a0	# checks if the value of a0 is larger than 0
	beq	$t1,1,while	# jumps to the while subroutine if above is true
	jr	$ra		# returns to the main routine if above is false
	nop

while:
	li	$t2,0
	slt	$t1,$0,$a0	# checks if the value of a0 is larger than 0
	beq	$t1,1,loop	# jumps to loop if above is true
	j	delay		# jumps back the delay subroutine if above is false

loop:
	addi	$t2,$t2,1	# adds 1 to t3
	slt	$t1,$t2,$t0
	beq	$t1,1,loop
	addi	$a0,$a0,-1	# subtracts 1 from a0, bringing it closer to 0
	j	while

time2string:
	PUSH	$ra
	move	$t1,$a1
	srl	$a1,$a1,4
	andi	$a1,$a1,0xf
	jal	hexasc
	nop
	move	$t2,$v0
	sll	$t2,$t2,8
	addi	$t2,$t2,0x3a
	sll	$t2,$t2,8
	move	$a1,$t1
	srl	$a1,$a1,8
	andi	$a1,$a1,0xf
	jal	hexasc
	nop
	add	$t2,$t2,$v0
	sll	$t2,$t2,8
	move	$a1,$t1
	srl	$a1,$a1,12
	andi	$a1,$a1,0xf
	jal	hexasc
	nop
	add	$t2,$t2,$v0
	sw	$t2,0($a0)
	move	$a1,$t1
	andi	$a1,$a1,0xf
	jal	hexasc
	nop
	sw	$v0,4($a0)
	li	$t2,0x00
	sb	$t2,5($a0)
	POP	$ra
	jr	$ra

enable_interrupt:
	ei
	jr $ra
	nop