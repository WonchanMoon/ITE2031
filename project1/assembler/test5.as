	lw	0	1	zero	load reg1 with 0 (a = 0)
	lw	0	2	one	load reg2 with 1 (b = 1)
	lw	0	7	neg1	load reg7 with -1
	lw	0	3	ten	load reg3 with 10 (loop counter)
loop	beq	0	3	done	branch if counter == 0
	add	2	1	4	reg4 = reg1 + reg2 (next fib number)
	add	0	2	1	reg1 = reg2 (a = b)
	add	0	4	2	reg2 = reg4 (b = next fib number)
	add	3	7	3	counter--
	beq	0	0	loop	jump to loop
done	sw	0	4	fib11	store 11th fib number
	halt
zero	.fill	0
one	.fill	1
ten	.fill	10
neg1	.fill	-1
fib11	.fill	0
