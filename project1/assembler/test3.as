	noop				nothing to do
	lw	0	2	a	load reg2 with 30
	lw	0	3	b	load reg3 with 40
	lw	0	7	sumPtr	load reg7 with sumPtr
start	jalr	7	6		jump to sum
	halt
sum	add	2	3	1
	sw	0	1	return	store return value
	jalr	6	7		go back
sumPtr	.fill	sum
a	.fill	30
b	.fill	40
return	.fill	0
