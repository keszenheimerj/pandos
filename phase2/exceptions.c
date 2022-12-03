/*H***************************************************************************************************
* FILENAME :	exceptions.c
*
* DESCRIPTION :	
*	Handles the exceptions that occur. Provides the appropriate handlers for program traps, 
	translation lookaside buffer (TLB) exceptions, as well as syscalls. For program traps and TLB exceptions, if the offending 
    process does not have its appropriate handler set up to manage the exception, the processes dies in a technique 
    known as "passing up or dying." For syscalls, a variety of helper functions exist to assist in program execution, 
    such as the copy state function, which will transfer the copies of one state to another, terminate progeny, which 
    will perform tail recursion on a process and all its children, a handler for requesting a syscall while in 
    user mode, the afformentioned pass up or die function, as well as global function for context switching. If
    a syscall is requested in user mode, a reserved instruction will be placed in the state's cause register and 
    will enter a program trap. Otherwise, the corresponding syscall will take control of execution.
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
/* ------------------------------------ */

/*sys1 - Create Process
*Creates a new process by allocating a new pcb_t.
	If the pcb_t is not null, then processCnt is 
	incremented by 1, it is inserted into ready 
	queue. If the pcb_t is either null or not, 
	a switchContect occurs.
*/
HIDDEN void CREATEPROCESS(state_PTR exState){
	state_PTR newState = (state_PTR) exState -> s_a1;

	support_t *supportP = (support_t*) exState -> s_a2;
	
	pcb_PTR tim = allocPcb();
	
	if(tim == NULL){
		currentProc -> p_s.s_v0 = -1;
	}else{
		
		tim -> p_semAdd = NULL;
		tim -> p_time = ZERO;	
		
		insertChild(currentProc, tim);
		insertProcQ(&(readyQueue), tim);
		
		
		copyState(newState, &(tim -> p_s));/*17.11 moveState*/
		if(supportP != ZERO || supportP != NULL){
			tim -> p_supportStruct = supportP;
		}else{tim -> p_supportStruct = NULL;}
		processCnt++;
		currentProc -> p_s.s_v0 = ZERO;
	}
	switchContext(exState);
}

/*sys2 - Terminate Process
*This services causes the executing process to terminate.
	This function is recursivly called in order for 
	all children to terminate too. Then, a new job 
	is acquired.
*/
HIDDEN void TERMPROC(pcb_PTR proc){
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
				if((semA >= &deviceSema4s[ZERO]) && (semA <= &deviceSema4s[MAXDEVCNT-1])){
					softBlockCnt--;
				}else{(*semA)++;}
			}
		}
		freePcb(proc);
		processCnt--;
}

/*sys3 - Passeren
*Perfroms a P operation on a specified synchronization semaphore in the
	$a1 reg of the old state. If the semaphore is less than zero, 
	the currrent processes is blocked, the old state is copied, and 
	a new job is retrieved via scheduler call. Otherwise, a context 
	switch occurs on the old state.
*/
HIDDEN void PASSEREN1(state_PTR exState){
	int *sema4 = (int*) (exState -> s_a1);
	(*sema4)--;
	if(*sema4<ZERO){
		copyState(exState, &(currentProc->p_s));/*17.11moveState*/
		insertBlocked(sema4, currentProc);
		softBlockCnt++;
		scheduler();
	}
	switchContext(exState);
}

/*sys4 - Verhogen
*Perfroms a V operation on a specified synchronization semaphore in the
	$a1 reg of the old state. If the semaphore is greater than zero, 
	a new processes is unblocked and placed in the ready queue. 
	No matter what, there is a switchContect to return to current process.
*/
HIDDEN void VERHOGEN1(state_PTR exState){
	int* sema4 = (int*) (exState->s_a1);
	(*sema4)++;
	pcb_PTR p;
	if((*sema4) <= ZERO){
		p = removeBlocked(sema4);
		if(p != NULL){
			/*softBlockCnt--;*/
			insertProcQ(&readyQueue, p);
		}
	}
	switchContext(exState);/*return to current proccess
}

/*sys5 - Wait for IO Device
*This service performs a P operation on the semaphore requested 
	by a device indicated by the values in a1, a2, and optionally a3.......................
*/
HIDDEN void WAIT_FOR_IO_DEVICE(state_PTR exState){
	copyState(exState, &(currentProc -> p_s));
	int lineN = exState -> s_a1;
	int devN = exState -> s_a2;
	int wait = exState-> s_a3;

	/*wait addresses whether we need the offset of 8(devperint)*/
	int device = (((lineN - DEVICEOFFSET + wait) * DEVPERINT) + devN);
	(deviceSema4s[device])--;
	
	softBlockCnt++;
	
	insertBlocked(&(deviceSema4s[device]), currentProc);
	currentProc = NULL;
	scheduler();
}

/*sys6 - Get CPU Time
*This service causes the processor time used by the 
	requesting process to be placed/returned in the 
	caller’s v0. 
*/
HIDDEN void GET_CPU_TIME(state_PTR exState){
	/*copy old state to current process*/
	copyState(exState, &(currentProc->p_s));/*17.11moveState*/
	cpu_t calcTime;
	STCK(calcTime);
	/* current time - sTOD */ /* getTimer from r129 */
	currentProc -> p_s.s_v0 = currentProc -> p_time = currentProc -> p_time + (calcTime-sTOD);
	STCK(sTOD);
	switchContext(&currentProc->p_s);
}

/*sys7 - Wait for Clock
*This instruction performs a P operation on the 
	pseudo-clock timer semaphore. This semaphore 
	is V’ed every 100 milliseconds automatically. 
	Then, a new job is retrieved. 
*/
HIDDEN void WAIT_FOR_CLOCK(state_PTR exState){
		copyState(exState, &(currentProc -> p_s));/*17.11*/
		deviceSema4s[MAXDEVCNT-1]--;
		if(deviceSema4s[MAXDEVCNT-1] < ZERO){
			softBlockCnt++;
			insertBlocked(&(deviceSema4s[MAXDEVCNT-1]), currentProc);
			currentProc = NULL;/*17.11*/
			scheduler();/*17.11*/
		}
		switchContext(&currentProc->p_s);
}

/*sys8 - Get Support Data
*This service requests a pointer to the Current Process's
	Support Structure. Hence, this service returns the
	value of p_supportStruct was provided for the Current
	Proccess when it was created, return null.
*/
HIDDEN void GET_SUPPORT_DATA(state_PTR exState){
		copyState(exState, &(currentProc->p_s));/*17.11moveState*/
		currentProc -> p_s.s_v0 = (int)(currentProc -> p_supportStruct);
		switchContext(&(currentProc -> p_s));
}

/*PassUpOrDie
*A syscall 5 (specify state exceptions vector) helper that will
	check if a new exception vector has been set up for that particular
	exception. If so, that processes is "passed up" to the appropriate 
	handler, copys into the processor's state the old exception, and 
	performs a context switch. Otherwise, the process dies.............................
*/
void passUpOrDie(state_t *exState, int exType){
	if(currentProc -> p_supportStruct != NULL){
		/*pass up*/
		copyState(exState, &(currentProc -> p_supportStruct -> sup_exceptState[exType]));/*17.11moveState*/
		LDCXT(currentProc -> p_supportStruct -> sup_exceptContext[exType].c_stackPtr, 
			currentProc -> p_supportStruct -> sup_exceptContext[exType].c_status, 
			currentProc -> p_supportStruct -> sup_exceptContext[exType].c_pc);
	}else{
		/*die*/
		TERMPROC(currentProc);
		scheduler();
	}
}

/*SYS
*The handler for syscalls 1-8 when the user is
 	in kernel mode. If the user is not in kernel mode,
 	PassUpOrDie is called.
*/
void SYS(){/*unsigned int num, unsigned int arg1, unsigned int arg2, unsigned int arg3*/
	/*get info from BIOSDATABAGE*/
	
	state_PTR exState;
	exState = (state_PTR) BIOSDATAPAGE;
	
	exState -> s_pc = exState -> s_t9 = exState -> s_pc + 4;
	
	/*int sysNum = ;*//*&UM 0x0...2*/
	if(((exState -> s_status) & KERNALMODE) != ALLBITSOFF){
		passUpOrDie(exState, GENERALEXCEPT);
	}
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
			passUpOrDie(exState, GENERALEXCEPT);/*generalException*/
			break;}
	}
}

