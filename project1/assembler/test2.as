	lw	0	1	eight	load reg1 with 8
	lw	0	2	two	load reg2 with 2
	lw	0	3	neg2	load reg3 with -2
loop	beq	1	2	done	branch if reg1 == reg2
	add	1	3	1	reg1 = reg1 + reg3
	beq	0	0	loop	jump to loop
done	sw	0	1	acc	store value in mem
	halt
two	.fill	2
eight	.fill	8
neg2	.fill	-2
acc	.fill	0
