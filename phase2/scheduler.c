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

#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/types.h"
#include "../h/const.h"
/* #include "../phase2/exceptions.c" */
/* #include "../phase2/interrupts.c" */
/* #include "../phase2/initial.c" */
/* #include "/usr/include/umps3/umps/libumps.h" */

cpu_t startT;

/* ---------Global Variables----------- */
extern pcb_PTR currentProc;
extern pcb_PTR readyQueue;
extern int processCnt;
extern int softBlockCnt;
/* ------------------------------------ */

void switchContext(pcb_PTR current){
	LDST(&(current -> p_s));
}

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
	switchContext(destination);
}

void scheduler(){
	if(emptyProcQ(readyQueue)){
		if(processCnt == 0){
			HALT();
		}
		else if(softBlockCnt == 0){
			PANIC();
		}
		
		else if(softBlockCnt > 0){
			currentProc = 0;
			
			/*set state*/	/* iec and im on */
			currentProc -> p_s.s_status = ALLOFF | IECON | IMON;
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
		pcb_PTR nextProc = removeProcQ(&readyQueue);
		
		if(nextProc != NULL){
			currentProc = nextProc;
			/*set timer*/
			double PLT = .5;
		
			STCK(startT);
			LDST(&currentProc -> p_s);
		}else{
			/*panic ?*/
		}
		
	}
	
}

void prepForSwitch(){
	state_PTR exState = (state_PTR) BIOSDATAPAGE;
		/*goto ready*/
	if(currentProc != NULL){
		moveState(exState, &(currentProc -> p_s));
		insertProcQ(&readyQueue, currentProc);
	}
	scheduler();
}
