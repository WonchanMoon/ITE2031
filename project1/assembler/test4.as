	lw	0	2	five	load 5 in reg2
	lw	0	3	zero	load 1 in reg3
	lw	0	4	neg1	load -1 in reg4
start	beq	1	2	done	branch if reg1 == reg2
	sw	2	2	arr0	store reg2 in arr[reg2]
	add	2	4	2	reg2 = reg2 + (-1)
	beq	0	0	start	goto start
done	halt
zero	.fill	0
one	.fill	1
neg1	.fill	-1
five	.fill	5
arr0	.fill	0
arr1	.fill	0
arr2	.fill	0
arr3	.fill	0
arr4	.fill	0
arr5	.fill	0
