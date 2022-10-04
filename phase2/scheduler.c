

#include "../h/scheduler.h"
void scheduler(){
	if(currentProc != NULL){


		currentProc = removeProcQ(&readyQueue); /* get who is next*/
		/*
		if not empty
			store a value on the timer
			*/
		if(emptyQueue(&readyQueue)){
			PLT = 5;
		}
		/*length of a quantom
		when at 0 call an interupt*/
		LoadState /*privaledged instruction*/

		/*loads all 35 registers*/
		LDST(&(curentProc -> p_s));
		/*if empty
		....
		*/
		if(pcCount == 0){
			/*invoke halt */
		}else if(pcCount > 0 && softBlockCnt > 0){
			/*set status to enable interrupts*/
			d_status = TRUE;
			disable PLT; || Load it with  a very large value
			/*enter wait state*/
		}else if(pcnt > 0 && softBlockCnt == 0){
			/*panic syscall; deadlock detection action*/
		}
	}
	
	pcb_PTR nextProc = removeProcQ(&readyQueue);
	
	if(nextProc != NULL){
		currentProc = nextProc;
	}else{
	
	}
}
