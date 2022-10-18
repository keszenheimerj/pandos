/*H***************************************************************************************************
* FILENAME :	exceptions.c
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
#include "../h/initial.h"
#include "../h/types.h"
#include "../h/const.h"
#include "/usr/include/umps3/umps/libumps.h"

void passUpOrDie(int exType, state_t *exState){
	if(currentProc -> p_supportStruct != NULL){
		/*pass up*/
	}else{
		
		/*die*/
		TERMINATEPROCESS();
	}
}

/*tlbTrapHandler*/
HIDDEN void TLBTrapHandler(){

}

/*sysTrapHandler*/
HIDDEN void sysTrapHandler(){
	
}

void sysCall(state_PTR state){
	/*get info from BIOSDATABAGE*/
	
	exState = state;
	
	exState -> s_pc = exState -> s_pc + 4;
	
	int sysNum = exState -> s_a0;
	
	
	
	switch(sysNum){
		case(1):
			CREATEPROCESS(state);
			break;
		case(2):
			TERMINATEPROCESS();
			break;
		case(3):
			PROBEREN(sema4);
			break;
		case(4):
			VERHOGEN(sema4);
			break;
		case(5):
			WAIT_FOR_IO_DEVICE();
			break;
		case(6):
			GET_CPU_TIME();
			break;
		case(7):
			WAIT_FOR_CLOCK();
			break;
		case(8):
			GET_SUPPORT_DATA();
			break;
		default:
			passUpOrDie(stuff);
	}
}

/*sys1*/
HIDDEN void CREATEPROCESS(){
	state_PTR newState = (statePTR) exState -> s_a1;
	support_t *supportP = (support_t*) exState -> s_a2;
	pcb_PTR tim = allocPCB();
	
	if(tim == NULL){
		currentProc -> p_s.s_v0 = -1;
	}else{
		insertProcQ(&(readyQueue), tim);
		insertChild(currentProc, tim);
		currentProc -> p_s.s_v0 = 0;
		
		processCnt ++;
		
		tim -> p_semAdd = NULL;
		tim -> p_time = 0;
		tim -> p_s = newState;
		tim -> p_supportStruct = supportP;
		
	}
	LDST(exState);
}

/*sys2*/
HIDDEN void TERMINATEPROCESS(){
	/*
	this service causes the executing process to cease to exist
	reccursively call children and terminate their processes as well
	*/
	/*if(emptyChild(p)){
		removerProc(p);
	}else{
	
		while(!emptyChild(p)){
			TERMINATEPROCESS(p -> p_child);
		}
	}*/
	outChild(currentProc);
	terminateChild(currentProc);
	currentProc = NULL;
	/*call scheduler*/
	scheduler();
}

/*recursive helper for TERMINATEPROCESS*/
void terminateChild(pcb_PTR child){
	if(child != NULL){
		while(!emptyChild(child){
			terminateChild(removeChild(child));
		}
		processCnt --;
		outProcQ(&readyQueue, child);
		
		/*check if free, active, asl*/
		
		freePCB(child);
	}
}

/*sys3*/
HIDDEN void PROBEREN(sem_PTR sema4){
	sema4--;
	if(sema4<0){
		insertBlocked(&sema4, currentProc);
		scheduler();
	}else{
		LDST(sema4);
	}
}

/*sys4*/
HIDDEN void VERHOGEN((sem_PTR sema4){
	sema4++;
	if(sema4 <= 0){
		p = removeBlocked(&sema4);
		insertPRocQ(&readyQueue, p);
	}
	LDST(sema4);/*return to current proccess*/
}

/*sys5*/
HIDDEN void WAIT_FOR_IO_DEVICE(){
	int n = exState -> s_a1;
	int dN = exState -> s_a2
	/*find which device
	test value
		insertBlocked
		scheduler();
		*/
	sema4--;
	if(sema4<0){
		insertBlocked(&sema4, currentProc);
		scheduler();
	}else{
		LDST(sema4);
	}
}

/*sys6*/
HIDDEN void GET_CPU_TIME(P){
	/*Bookeeping and management
		look for a call called "CPU Time" maybe...
		Write down time of day clock
		Do subtraction to figure out how much time was used
		Return time
		How much is there plus how much current time slice.
		use register s_v0
		*/
	currentProc -> p_time = currentProc -> p_time + (	-startT);/*current time - startTime*/
	exState -> s_v0 = currentProc -> p_time;
	STCK(startTime);
	LDST(exState); /*swap for switchContect*/
}

/*sys7*/
HIDDEN void WAIT_FOR_CLOCK(){
	/*Preforms a P oporations on the Nucleus.
		This semaphore is V'ed every 100 milliseconds by the Nucleus.
		This call should always clock the Current Process on the ASL, after the scheduler is called.
		sys7 is used to transition the Current Process frim the "running" state to a "blocked" state.
		sys7 is called by placing the value 7 in a0 and then exectuting syscall.
		example sys7 request from book: SYSCALL (WAITCLOCK, 0, 0, 0); 
		Where the mnemonic constant WAITCLOCK has the value of 7.
		*/
		softBlockCnt ++;
		PASSERN(&deviceSema4s[/*on interval timer semaphore*/
}

/*sys8*/
HIDDEN void GET_SUPPORT_DATA(){
	/*requests a pointer to the Current Process's Support Structure. 
		returns the value of p_supportStruct from the Current Process's pcb.
		if no value for p_support Struct, then return NULL.
		example sys8 from book: support_t *sPtr = SYSCALL (GETSUPPORTPTR, 0, 0, 0);
		Where the mnemonic constant GETSUPPORTPTR has the value of 8.
		*/
		exstate -> s_v0 = currentProc -> p_supportStruct;
		LDST(exState); /*swap for switch context*/
}


