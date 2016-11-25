/*
This little bit of code is executed in-place by one CPU, but copied to a different memory region
by the other CPU. Make sure it stays position-independent.
*/
    .text
    .align  4
    .global test_fastbus_cp
    .type   test_fastbus_cp,@function
//Args:
//a2 - fifo addr
//a3 - buf addr
//a4 - len
//a5 - ptr to int to use
test_fastbus_cp:
	entry a1,64
back:
	beqi a4, 0, out		//check if loop done
	s32i a4, a5, 0		//store value, for shits and/or giggles
	memw				//make sure write happens
	l32i a4, a5, 0		//load value again, to thwart any prediction in the pipeline
	bbsi a4, 0, pred	//Random jump to check predictive reads. Both branches should do the same.
	l32i a6, a2, 0		//read from fifo 1
	j predout
pred:
	l32i a6, a2, 0		//read from fifo 2
predout:
	s8i a6, a3, 0		//store result
	addi a3, a3, 1		//inc ptr
	addi a4, a4, -1		//next
	j back				//loop again
out:
	retw				//and we are done
