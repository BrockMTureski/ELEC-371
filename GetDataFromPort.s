GetDataFromPort:
	stw	r2,0(sp)
	stw	r3,4(sp)
	stw	r4,8(sp)  #r4 is list_ptr
	stw	r5,12(sp) #r5 is n
	stw	r6,16(sp)
	subi	sp,sp,20
	
LOOP:
	movia	r2,PORT_STATUS
	movia	r3,PORT_DATA
	movi	r6,0x40
	ldw	r2,0(r2)
	and	r2,r6,r2
	bne	r6,r2,LOOP
	
	ldwio	r3,0(r3)
	stw	r3,0(r4)
	
	addi	r4,r4,4
	subi	r5,r5,1
	
	bne	r5,r0,LOOP
	
	addi	sp,sp,20
	ldw	r2,0(sp)
	ldw	r3,4(sp)
	ldw	r4,8(sp)
	ldw	r5,12(sp)
	ldw	r6,16(sp)
	
	ret
	
	
			
