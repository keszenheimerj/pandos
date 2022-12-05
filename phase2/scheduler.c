/*H***************************************************************************************************
* FILENAME :	scheduler.c
*
* DESCRIPTION :	
*	Implements a round-robin scheduling algorithm whenever a new process is needed.
*
* PUBLIC FUNCTIONS : 
*	switchContect;	takes a state pointer to load
*	copyState;	pass state pointers for source and destination to do deep copy of state
*	scheduler;	called to continue round robin scheduling
*	prepForSwitch;	called to prep ready queue by adding current process before scheduler gets called
*	
* AUTHORS :	James Keszenheimer, Evan Hanson		START DATE : 31 Aug 22
*
*H*/

#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/types.h"
#include "../h/const.h"
#include "/usr/include/umps3/umps/libumps.h"

/* ---------Global Variables----------- */
extern pcb_PTR currentProc;
extern pcb_PTR readyQueue;
extern int processCnt;
extern int softBlockCnt;
extern cpu_t sTOD;
/* ------------------------------------ */

state_PTR sour;
state_PTR dest;

/*switchContext
*Loads a new state. BANG!
*/
void switchContext(state_PTR s){
	LDST(s);
}

/*moveState
*Copies the current state using all 35 registers.
*/
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

/*copyState
*Copies the current state using all 35 registers.
*/
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

/*Scheduler
*Invokes the scheduler which implements a round-robin 
	scheduling algorithm whenever a new process is needed.
*/
void scheduler(){
	cpu_t elapsed = 0;
	
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

/*prepForSwitch
*
*/
void prepForSwitch(){
	state_PTR exState = (state_PTR) BIOSDATAPAGE;
		/*go to ready*/
	if(currentProc != NULL){
		copyState(exState, &(currentProc -> p_s));/*17.11moveState*/
		insertProcQ(&readyQueue, currentProc);
	}
	scheduler();
}
