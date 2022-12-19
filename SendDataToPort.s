SendDataToPort:
	stw		r2,0(sp) #list_ptr
	stw		r3,4(sp) #n
	stw		r4,8(sp)
	stw		r5,12(sp)
	subi	sp,sp,16

LOOP:
	movia	r4,OUTPORT_STATUS
	ldw		r4,0(r4)
	movi	r5,0x10
	and		r4,r4,r5
	
	bne		r4,r5,LOOP
	
	ldw		r4,0(r2)
	addi	r2,r2,4
	subi	r3,r3,1
	movia	r5,OUTPORT_DATA
	stwio	r4,0(r5)
	
	bne		r3,r0,LOOP
	
	addi	sp,sp,16
	ldw		r2,0(sp)
	ldw		r3,4(sp)
	ldw		r4,8(sp)
	ldw		r5,12(sp)
	
	ret
	