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
#include "../h/types.h"
#include "../h/const.h"
/* #include "../phase2/initial.c" */
/* #include "../phase2/interrupts.c" */
/*#include "/usr/include/umps3/umps/libumps.h"*/

/* ---------Global Variables----------- */
extern pcb_PTR currentProc;
extern pcb_PTR readyQueue;
extern int processCnt;
extern int softBlockCnt;
extern int deviceSema4s[MAXDEVCNT];
extern cpu_t sTOD;
extern void prepForSwitch();
extern void moveState(state_PTR source, state_PTR destination);
extern void switchContext(state_PTR s);
extern void scheduler();
extern void LDCXT(unsigned int stackPTR, unsigned int status, unsigned int pc);
extern void copyState(state_PTR source, state_PTR destination);
int sysNum;

/*state_PTR exState;*/
/* ------------------------------------ */



/* I think the TLBTrapHandler and the ProgramTrapHandler goes in the passUpOrDie()  - evan*/

/*tlbTrapHandler*/
/*HIDDEN void TLBTrapHandler(){

}*/

/*sysTrapHandler*/
/*HIDDEN void sysTrapHandler(){
	
}*/

/*state_PTR firstHalf(){
	support_t *supportP = (support_t*) exState -> s_a2;
	return s;
}*/

/*sys1*//*done*/
HIDDEN void CREATEPROCESS(state_PTR exState){
	/*state_PTR newState = (state_PTR) BIOSDATAPAGE; just filling for init
	moveState( ( (state_PTR) exState -> s_a1), newState);*/
	state_PTR newState = (state_PTR) exState -> s_a1;

	support_t *supportP = (support_t*) exState -> s_a2;
	
	pcb_PTR tim = allocPcb();
	
	if(tim == NULL){
		currentProc -> p_s.s_v0 = -1;
	}else{
		
		tim -> p_semAdd = NULL;
		tim -> p_time = 0;
		/*tim -> p_s = *newState; tim -> p_s = *newState; is memcpy error*/
		
		
		insertChild(currentProc, tim);
		insertProcQ(&(readyQueue), tim);
		
		
		copyState(newState, &(tim -> p_s));/*17.11 moveState*/
		if(supportP != 0 || supportP != NULL){
			tim -> p_supportStruct = supportP;
		}else{tim -> p_supportStruct = NULL;}
		processCnt++;
		currentProc -> p_s.s_v0 = 0;
	}
	switchContext(exState);
}

HIDDEN void TERMPROC(pcb_PTR proc){
	/*if(currentProc == NULL){
		scheduler();
	}else{*/
		while(!emptyChild(proc)){
			TERMPROC(removeChild(proc));
		}
		if(currentProc == proc){outChild(proc);}/*running*//*if it has a parent say goodbye*/
		else if(proc->p_semAdd == NULL){/*ready*/
			outProcQ(&readyQueue, proc);
		}else{/*sem*//*waiting queue*/
			int* semA = proc -> p_semAdd;
			pcb_PTR p = outBlocked(proc);
			if(p != NULL){
				/*if((semA >= &deviceSema4s[0]) && (semA <= &deviceSema4s[MAXDEVCNT-1])){
					softBlockCnt--;
				}else{(*semA)++;}*/
				
			}
			softBlockCnt++;
			(*semA)++;
		}
		freePcb(proc);
		processCnt--;
	/*}*/
}

/*recursive helper for TERMINATEPROCESS*/
HIDDEN void terminateChild(pcb_PTR child){
	if(currentProc == NULL){
		scheduler();
	}else{
		if(child != NULL){
			while(!emptyChild(child)){
				terminateChild(removeChild(child));
			}
			processCnt--;
			outProcQ(&readyQueue, child);
			
			/*check if free, active, asl*/
			
		freePcb(child);
		}
	}
}

/*sys2*//*done*/
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

/*sys3*//*done*/
HIDDEN void PASSEREN1(state_PTR exState){
	int *sema4 = (int*) (exState -> s_a1);
	(*sema4)--;
	if(*sema4<0){
		copyState(exState, &(currentProc->p_s));/*17.11moveState*/
		insertBlocked(sema4, currentProc);
		softBlockCnt++;/*17.11*//*iffy*/
		scheduler();
	}
	switchContext(exState);
	
	/*
	(exState -> s_a1) = (exState -> s_a1)- 1;
	if((exState -> s_a1)<0){
		copyState(exState, &(currentProc->p_s));/*17.11moveState*/
		/*insertBlocked(&(exState -> s_a1), currentProc);
		softBlockCnt++;/*17.11*//*iffy*/
		/*scheduler();
	}
	switchContext(exState);*/
}

/*sys4*//*done*/
HIDDEN void VERHOGEN1(state_PTR exState){
	int* sema4 = (int*) (exState->s_a1);
	(*sema4)++;
	pcb_PTR p;
	if((*sema4) <= 0){
		p = removeBlocked(sema4);
		if(p != NULL){
			/*softBlockCnt--;*/
			insertProcQ(&readyQueue, p);
		}
	}
	switchContext(exState);/*return to current proccess
	
	
	/*((exState->s_a1))++;
	pcb_PTR p;
	if(((exState->s_a1)) <= 0){
		p = removeBlocked(&(exState->s_a1));
		if(p != NULL){
			softBlockCnt--;
			insertProcQ(&readyQueue, p);
		}
	}
	switchContext(exState);/*return to current proccess*/
}

/*sys5*//*done*/
HIDDEN void WAIT_FOR_IO_DEVICE(state_PTR exState){
	copyState(exState, &(currentProc -> p_s));
	int lineN = exState -> s_a1;
	int devN = exState -> s_a2;
	
	/*a3*/
	int wait = exState-> s_a3;
	/*find which device
	test value
		insertBlocked
		scheduler();
		*/
	int device = (((lineN - 3 + wait) * DEVPERINT) + devN);/*wait addresses whether we need the offset of 8(devperint)*/
	(deviceSema4s[device])--;
	
	softBlockCnt++;
	
	insertBlocked(&(deviceSema4s[device]), currentProc);
	currentProc = NULL;
	scheduler();
	/*if(deviceSema4s[device]<0){
		softBlockCnt++;
		insertBlocked(&(deviceSema4s[device]), currentProc);
		currentProc = NULL;
		scheduler();
	}else{
		switchContext(exState);
	}*/
}

/*sys6*//*done*/
HIDDEN void GET_CPU_TIME(state_PTR exState){
	/*Bookeeping and management
		look for a call called "CPU Time" maybe...
		Write down time of day clock
		Do subtraction to figure out how much time was used
		Return time
		How much is there plus how much current time slice.
		use register s_v0
		*/
	/*copy old state to current process*/
	copyState(exState, &(currentProc->p_s));/*17.11moveState*/
	cpu_t calcTime;
	STCK(calcTime);
	currentProc -> p_s.s_v0 = currentProc -> p_time = currentProc -> p_time + (calcTime-sTOD);/* current time - sTOD */ /* getTimer from r129 */
	STCK(sTOD);
	switchContext(&currentProc->p_s); /*swap for switchContect*/
}

/*sys7*/
HIDDEN void WAIT_FOR_CLOCK(state_PTR exState){
	/*Preforms a P oporations on the Nucleus.
		This semaphore is V'ed every 100 milliseconds by the Nucleus.
		This call should always clock the Current Process on the ASL, after the scheduler is called.
		sys7 is used to transition the Current Process frim the "running" state to a "blocked" state.
		sys7 is called by placing the value 7 in a0 and then exectuting SYS.
		example sys7 request from book: SYS (WAITCLOCK, 0, 0, 0); 
		Where the mnemonic constant WAITCLOCK has the value of 7.
		*/
		/*moveState(exState, &(currentProc -> p_s));
		softBlockCnt ++;
		exState -> s_a1 = (deviceSema4s[MAXDEVCNT-1]);
		PASSEREN1(exState)*on interval timer semaphore*/
		copyState(exState, &(currentProc -> p_s));/*17.11*/
		deviceSema4s[MAXDEVCNT-1]--;
		if(deviceSema4s[MAXDEVCNT-1] < 0){
			softBlockCnt++;
			insertBlocked(&(deviceSema4s[MAXDEVCNT-1]), currentProc);
			currentProc = NULL;/*17.11*/
			scheduler();/*17.11*/
		}
		switchContext(&currentProc->p_s);
}

/*sys8*/
HIDDEN void GET_SUPPORT_DATA(state_PTR exState){
	/*requests a pointer to the Current Process's Support Structure. 
		returns the value of p_supportStruct from the Current Process's pcb.
		if no value for p_support Struct, then return NULL.
		example sys8 from book: support_t *sPtr = SYS (GETSUPPORTPTR, 0, 0, 0);
		Where the mnemonic constant GETSUPPORTPTR has the value of 8.
		*/
		copyState(exState, &(currentProc->p_s));/*17.11moveState*/
		currentProc -> p_s.s_v0 = (int)(currentProc -> p_supportStruct);/*check whether to add -> sup_asid */
		switchContext(&(currentProc -> p_s)); /*swap for switch context*/
}

void pseudoClockTick(){
	/*1. acknowledge the interrupt by loading the Interval Timer with a new value: 100 milliseconds
		2. Unblock ALL pcbs blocked on the Pseudo-clock semaphore. Hence, the semantics of this 
		semaphore are a bit different than traditional sycronization semaphores.
		3. Reset the Pseudo-clock semaphore to zero. This insures that all SYS7 calls block and that 
		the Pseudo-clock semaphoredoes not grow positiv.
		4. Return control to the Current Process: Preform a LDST on the saved exception state
		(located at the start of the BIOS Data Page).
		*/
}

void passUpOrDie(state_t *exState, int exType){
	if(currentProc -> p_supportStruct != NULL){
		/*pass up*/
		copyState(exState,&(currentProc -> p_supportStruct -> sup_exceptState[exType]));/*17.11moveState*/
		
		/*r129 seems liked we need to change execution state*/
		
		unsigned int stackP = currentProc -> p_supportStruct -> sup_exceptContext[exType].c_stackPtr;
		unsigned int statusN = currentProc -> p_supportStruct -> sup_exceptContext[exType].c_status;
		unsigned int pc = currentProc-> p_supportStruct -> sup_exceptContext[exType].c_pc;
		LDCXT(stackP, statusN, pc); 
	}else{
		/*die*/
		TERMPROC(currentProc);
		currentProc = NULL;
		scheduler();
	}
}

void SYS(){/*unsigned int num, unsigned int arg1, unsigned int arg2, unsigned int arg3*/
	/*get info from BIOSDATABAGE*/
	
	state_PTR exState;
	exState = (state_PTR) BIOSDATAPAGE;
	
	exState -> s_pc = exState -> s_t9 = exState -> s_pc + 4;
	
	/*int sysNum = ;*//*&UM 0x0...2*/
	/*if(((exState -> s_status) & STAT) != ALLBITSOFF){
		passUpOrDie(exState, 1);
	}*/
	sysNum = (exState -> s_a0);
	
	
	
	switch(sysNum){
		case(CREATETHREAD):{/*createthread*/
			CREATEPROCESS(exState);
			break;}
		case(TERMINATETHREAD):{/*terminatethread*/
			TERMPROC(currentProc);
			scheduler();
			break;}
		case(PASSERN):{
			PASSEREN1(exState);
			break;}
		case(VERHOGEN):{
			VERHOGEN1(exState);
			break;}
		case(WAITIO):{
			WAIT_FOR_IO_DEVICE(exState);
			break;}
		case(GETCPUTIME):{
			GET_CPU_TIME(exState);
			break;}
		case(WAITCLOCK):{
			WAIT_FOR_CLOCK(exState);
			break;}
		case(GETSPTPTR):{
			GET_SUPPORT_DATA(exState);
			break;}
		default:{
			passUpOrDie(exState, 1);/*generalException*/
			break;}
	}
}

