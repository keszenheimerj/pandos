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

cpu_t startT;

void moveState(state_PTR source, state_PTR destination){ /*copy the source state */
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

void prepForSwitch(){
	state_PTR exState = (state_PTR) BIOSDATAPAGE;
		/*goto ready*/
	if(currentProc != NULL){
		moveState(exState, &(currentProc -> p_s));
		insertProcQ(&readyQueue, currentProc)
	}
	scheduluer();
}

void switchContext(pcb_PTR current){
	LDST(&(current -> s_pc));
}

void scheduler(){
	if(emptyProcQ(readyQ)){
		if(proccessCnt == 0){
			HALT();
		}
		else if(softBlockCnt == 0){
			PANIC();
		}
		
		else if(softBlockCnt > 0){
			currentProc = 0;
			
			/*set state*/	/* iec and im on */
			currentProc -> p_s.s_status = ALLOFF = IECON | IMON;
			/*set status*/
			setTimer(BIGNUMBER);
			WAIT();
		}
		
		/*currentProc -> p_time = currentProc -> p_time + (/*current time- startT);
		currentProc = removeProcQ(&readyQueue); /* get who is next*/
		/*
		if not empty
			store a value on the timer
			
		if(emptyQueue(&readyQueue)){
			STCK(startT);
			PLT = .5;
		}
		/*length of a quantom
		when at 0 call an interupt
		LoadState /*privaledged instruction

		/*loads all 35 registers
		moveState(source, destination);
		/*if empty
		....
		
		if(pcCount == 0){
			/*invoke halt 
		}else if(pcCount > 0 && softBlockCnt > 0){
			/*set status to enable interrupts
			d_status = TRUE;
			disable PLT; || Load it with  a very large value
			/*enter wait state
		}*/
	}else{
		pcb_PTR nextProc = removeProc(&readyQueue);
		
		if(nextProc != NULL){
			currentProc = nextProc;
			/*set timer*/
			PLT = .5;
		
			STCK(startT);
			LDST(&currentProc -> p_s);
		}else{
			/*panic ?*/
		}
		
	}
	
}
