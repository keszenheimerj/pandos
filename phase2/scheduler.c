/*H***************************************************************************************************
* FILENAME :	scheduler.c
*
* DESCRIPTION :	
*	implements a round-robin scheduling algoithm whenever a new process is needed.
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
#include "/usr/include/umps3/umps/libumps.h"

/*cpu_t elapsed;*/

/* ---------Global Variables----------- */
extern pcb_PTR currentProc;
extern pcb_PTR readyQueue;
extern int processCnt;
extern int softBlockCnt;
extern cpu_t sTOD;
/* ------------------------------------ */
state_PTR sour;
state_PTR dest;

/*void switchContext(pcb_PTR current){
	LDST(&(current -> p_s));
}*/
void switchContext(state_PTR s){
	LDST(s);
}

void moveState(state_PTR source, state_PTR destination){ /*copy the source state */
	/*do stuff with all 35 regs in a for*/
	int i = 0;
	while(i < STATEREGNUM){
		destination -> s_reg[i] = source -> s_reg[i];
		i++;
	}
	
	/*four other lines to init fields of state*/
	destination -> s_entryHI = source -> s_entryHI;
	destination -> s_cause = source -> s_cause;
	destination -> s_status = source -> s_status;
	destination -> s_pc = source -> s_pc;
	switchContext(destination);
}

void copyState(state_PTR source, state_PTR destination){ /*copy the source state */
	/*do stuff with all 35 regs in a for*/
	sour = source;
	dest = destination;
	int i = 0;
	while(i < STATEREGNUM){
		destination -> s_reg[i] = source -> s_reg[i];
		i++;
	}
	
	/*four other lines to init fields of state*/
	destination -> s_entryHI = source -> s_entryHI;
	destination -> s_cause = source -> s_cause;
	destination -> s_status = source -> s_status;
	destination -> s_pc = source -> s_pc;
}

void scheduler(){
	cpu_t elapsed = 0;
	/*if(currentProc != NULL){emptyProcQ(readyQueue)
		if(processCnt == 0){
			HALT();
		}
		else if(softBlockCnt == 0){
			PANIC();
		}
		
		else if(softBlockCnt > 0){
			currentProc = 0;
			
			/*set state*/	/* iec and im on 
			currentProc -> p_s.s_status = ALLBITSOFF | IECON | IMON;
			/*set status
			WAIT();
		}
		
		/*currentProc -> p_time = currentProc -> p_time + (current time- elapsed);
		currentProc = removeProcQ(&readyQueue);  get who is next
		
		if not empty
			store a value on the timer
			
		if(emptyQueue(&readyQueue)){
			STCK(elapsed);
			PLT = .5;
		}
		length of a quantom
		when at 0 call an interupt
		LoadState privaledged instruction

		loads all 35 registers
		moveState(source, destination);
		if empty
		....
		
		if(pcCount == 0){
			invoke halt 
		}else if(pcCount > 0 && softBlockCnt > 0){
			set status to enable interrupts
			d_status = TRUE;
			disable PLT; || Load it with  a very large value
			enter wait state
		}*
	}else{
		pcb_PTR nextProc = removeProcQ(&readyQueue);
		
		if(nextProc != NULL){
			currentProc = nextProc;
			/*set timer
			/*double PLT = .5;	unused
		
			STCK(elapsed);
			LDST(&currentProc -> p_s);
		}else{
			panic ?
		}
		
	}*/
	
	if(currentProc != NULL){
		STCK(elapsed);
		currentProc -> p_time = currentProc -> p_time + (elapsed - sTOD);
		LDIT(IO);
		
	}
	pcb_PTR nProc = removeProcQ(&readyQueue);/*reached*/
	if(nProc != NULL){
		currentProc = nProc;
		STCK(sTOD);
		setTIMER(QUANTUM);
		switchContext(&(currentProc -> p_s));
	}else{
		if(processCnt == 0){
			HALT();
		}
		if(processCnt > 0 && softBlockCnt > 0){
			setTIMER((unsigned int)100000000000000000);/*not needed but repeated*/
			setSTATUS((int) ALLBITSOFF | IECON | IMON);
			WAIT();
		}
	}
	
}

void prepForSwitch(){
	state_PTR exState = (state_PTR) BIOSDATAPAGE;
		/*goto ready*/
	if(currentProc != NULL){
		copyState(exState, &(currentProc -> p_s));/*17.11moveState*/
		insertProcQ(&readyQueue, currentProc);
	}
	scheduler();
}
