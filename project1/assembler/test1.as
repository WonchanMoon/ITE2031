	lw	0	1	one	load reg1 with 1
	nor	0	0	2	reg2 = nor(0,0)
	nor	0	1	3	reg3 = nor(0,1)
	nor	1	0	4	reg4 = nor(1,0)
	nor	1	1	5	reg5 = nor(1,1)
	sw	0	2	nor00	store reg2 into nor00 // ~0 = -1
	sw	0	3	nor01	store reg2 into nor01 // ~1 = -2
	sw	0	4	nor10	store reg2 into nor10 // ~1 = -2
	sw	0	5	nor11	store reg2 into nor11 // ~1 = -2
	halt				end of program
one	.fill	1
nor00	.fill	0
nor01	.fill	0
nor10	.fill	0
nor11	.fill	0
