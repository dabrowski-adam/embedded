		.global	div				@ div function visible out of this file
div:
		@ push R1...R4 on the stack
		stmfd	sp!, {R1, R2, R3, R4}

		mov		R2,R1
		mov		R1,R0

		@ Enter with numbers in R1 and R2.

		MOV 	R4,#1 					@ Bit to control the division.
Div1: 	CMP 	R2,#0x80000000 			@ Move R2 until greater than R1.
		CMPCC 	R2,R1
		MOVCC 	R2,R2,ASL#1
		MOVCC 	R4,R4,ASL#1
		BCC 	Div1
		MOV 	R3,#0
Div2: 	CMP 	R1,R2 					@ Test for possible subtraction.
		SUBCS 	R1,R1,R2 				@ Subtract if ok,
		ADDCS 	R3,R3,R4 				@ put relevant bit into result
		MOVS 	R4,R4,LSR#1 			@ shift control bit
		MOVNE 	R2,R2,LSR#1 			@ halve unless finished.
		BNE 	Div2

		@ Divide result in R3,
		@ remainder in R1

		@ return the result
		mov		R0,R3

		@ pop R1...R4 from the stack & return
		ldmfd	sp!, {R1, R2, R3, R4}
		bx		lr
