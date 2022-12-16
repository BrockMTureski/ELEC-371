.global _start
_start:
	movia	r3,src_list1
	movia	r4,src_list2
	movia	r5,dest_list
	ldb		r6,N(r0)
	call	compare
	br		_exit
	
#--------------------------------------------------------
compare:
	stw		r2,0(sp)
	stw		r3,4(sp)
	stw		r4,8(sp)
	stw		r5,12(sp)
	stw		r6,16(sp)
	stw		r7,20(sp)
	stw		r8,24(sp)
	addi	sp,sp,28
comparefor:
	ldw		r7,0(r3)
	ldw		r8,0(r4)
	
	bgt		r7,r8,compareelse
	mov		r2,r8
	br		compareexit
compareelse:
	mov		r2,r7
compareexit:
	stw		r2,0(r5)
	
	addi 	r5,r5,4
	addi	r3,r3,4
	addi	r4,r4,4
	subi	r6,r6,1
	
	bgt		r6,r0,comparefor
	
	subi	sp,sp,28
	ldw		r2,0(sp)
	ldw		r3,4(sp)
	ldw		r4,8(sp)
	ldw		r5,12(sp)
	ldw		r6,16(sp)
	ldw		r7,20(sp)
	ldw		r8,24(sp)
	ret
#-----------------------------------------------------------
_exit:
	break

src_list1:	.word -9,13,0xFF,167
src_list2:	.word 24,-5,0,192
dest_list:	.skip 16
N:		.byte 4