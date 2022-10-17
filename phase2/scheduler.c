/*H***************************************************************************************************
* FILENAME :	scheduler.c
*
* DESCRIPTION :	
*	example
*
* PUBLIC FUNCTIONS : 
*	
* AUTHORS :	James Keszenheimer, Evan Hanson		START DATE : 31 Aug 22
*
*H*/

#include "../h/scheduler.h"
#include "../h/types.h"
#include "/usr/include/umps3/umps/libumps.h"

void moveState(state_t source, state_t destination){
	/*do stuff with all 35 regs in a for*/
	for(int i = 0; i < STATEREGNUM;i++){
		destination -> s_reg[i] = source -> s_reg[i];
	}
	
	/*four other lines to init fields of state*/
	destination -> s_entryHI = source -> s_entryHI;
	destination -> s_cause = source -> s_cause;
	destination -> s_status = source -> s_status;
	destination -> s_pc = source -> s_pc;
	switchContect(destination);
}

void prepForSwitch(pcb_PTR current){
	current -> s_pc = current -> s_pc + 4;
}

void switchContext(pcb_PTR current){
	LDST(&(current -> s_pc));
}

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
		moveState(source, destination);
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
