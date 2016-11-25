/*
This little bit of code is executed in-place by one CPU, but copied to a different memory region
by the other CPU. Make sure it stays position-independent.
*/
    .text
    .align  4
    .global test_s32c1i_lock
    .type   test_s32c1i_lock,@function
//Args:
//a2 - lock addr
//a3 - val to lock with
//a4 - val to unlock with
//a5 - addr to increase
test_s32c1i_lock:
	entry a1, 64
	wsr a4, SCOMPARE1
lockloop:
	mov a6, a3
	s32c1i a6, a2, 0
	bne a4, a6, lockloop

	l32i a6, a5, 0
	//Give other CPU the time to mess up the inc if the lock somehow malfunctions
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	addi a6, a6, 1
	s32i a6, a5, 0


//No need to actually let this loop but hey, a hang indicates an error, right?
	wsr a3, SCOMPARE1
unlockloop:
	mov a6, a4
	s32c1i a6, a2, 0
	bne a3, a6, unlockloop
	
	retw
