.global _start
_start:

	movia	r2,LIST1
	movia	r3,LIST2
	ldb		r4,N(r0)
	call	ListCopy
	br		_exit
	
ListCopy:
	stw		r2,0(sp)
	stw		r3,4(sp)
	stw		r4,8(sp)
	stw		r5,12(sp)
	stw		r6,16(sp)
	subi		sp,sp,20
	
	movi	r5,0
	
Listfor:
	ldw		r6,0(r2)
	stw		r6,0(r3)
	
	addi	r2,r2,4
	addi	r3,r3,4
	subi	r4,r4,1
	beq		r4,r0,ListExit
	br		Listfor
ListExit:
	addi	sp,sp,20
	ldw		r2,0(sp)
	ldw		r3,4(sp)
	ldw		r4,8(sp)
	ldw		r5,12(sp)
	ldw		r6,16(sp)
	ret
	
_exit:
	break
	
	
	
LIST1: .word 2,5,6,8,9
LIST2: .skip 20 
N:.byte 5
