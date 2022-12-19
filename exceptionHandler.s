ISR:
	stw		r2,0(sp)
	stw		r3,4(sp)
	subi	sp,sp,8
	
	rdctl	et,ipending
	movi	r2,0x10
	and		r3,r2,et
	bne		r3,r2,HandleY
	
	call	HandleIntX
	
HandleY:
	movi	r2,0x20
	and		r3,r2,et
	bne		r3,r2,Exit
	
	call	HandleIntY
	
Exit:
	addi	sp,sp,8
	ldw		r2,0(sp)
	ldw		r3,4(sp)
	
	eret