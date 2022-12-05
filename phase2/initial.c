/*H***************************************************************************************************
* FILENAME :	initial.c
*
* DESCRIPTION :	
*	Initializes the main() function and runs the bootcode for Pandos OS.
*
* PUBLIC FUNCTIONS : 
*	genExceptionHandler;	called when a general exception occurs and uses the BIOS data page to find cause and call either:
*					SYS():cause 8, interruptHandler():cause 0, passUpOrDie(..): cause else
*	
* AUTHORS :	James Keszenheimer, Evan Hanson		START DATE : 31 Aug 22
*
*H*/

#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/types.h"
#include "../h/const.h"
#include "/usr/include/umps3/umps/libumps.h"

/* ---------Global Functions----------- */
extern void test();
extern void passUpOrDie(state_t *exState, int exType);
/* ------------------------------------ */

/* ---------Global Variables----------- */
extern void intHandler();
extern unsigned int SYS();
extern void scheduler();
extern void prepForSwitch();
extern void uTLB_RefillHandler();

pcb_PTR readyQueue;		/*pointer to queue of pcbs that are waiting*/
pcb_PTR currentProc; 			/*pointer to the running Proc*/
int processCnt;					/*int indicating the started but not terminated processes*/
int softBlockCnt;				/*a process can either be ready, running, blocked(waiting) state 
									and this int is the number of started, but not terminated processes*/
int deviceSema4s[MAXDEVCNT]; 	/*a sema4 for each device so 49; last is sudo clock*/
cpu_t sTOD;
int causeNum;
/* ------------------------------------ */

/*genExceptionHandler
*Handles exceptions.
*/
void genExceptionHandler(){
	/*save state*/
	state_PTR previousStatePTR = (state_PTR) (BIOSDATAPAGE);
	/*make ptr to from bios*/
	causeNum = (int) (((previousStatePTR -> s_cause) & genExMask) >> 2);/* & used to grab just the five bits desired*/
	/*do bitwise stuff*/
	if(causeNum == ZERO){
		/*pass proccessing to nucleus dev interupt handler*/
		intHandler();
	}
	if(causeNum == 8){
		SYS();
	}else{
		/*tlb execption, pass proccessing to tlb-exception handler*/
		if(causeNum < 4){
			passUpOrDie((state_PTR) BIOSDATAPAGE, PGFAULTEXCEPT);
		}else{
			passUpOrDie((state_PTR) BIOSDATAPAGE, GENERALEXCEPT); 
		}
	}
}

/*Main
*Runs the boot code for the OS. it will initalize pcbs and will initialize 
	the active semaphore list. It will also populate low areas of memory
	as well as setting the ground work for context switching. Finally, it 
	will call the scheduler.
*/
int main(){
	initPcbs();
	initASL();
	
	devregarea_PTR devBus = (devregarea_PTR) RAMBASEADDR;
	int RAMTOP = (devBus -> rambase + devBus -> ramsize);
	
	/*init 4 words in BIOS pg*/
	passupvector_PTR passV = (passupvector_t *) PASSUPVECTOR;
	passV -> tlb_refill_handler = (memaddr) uTLB_RefillHandler;
	passV -> tlb_refill_stackPtr = EXENTRY;
	passV -> exception_handler =  (memaddr) genExceptionHandler;
	passV -> exception_stackPtr = EXENTRY; 
	
	readyQueue = mkEmptyProcQ();
	currentProc = NULL;
	processCnt = ZERO;
	softBlockCnt = ZERO;
	int i = ZERO;
	while(i < MAXDEVCNT){
		deviceSema4s[i] = ZERO;
		i++;
	}
	
	pcb_PTR p = allocPcb();
	if(p == NULL){
		PANIC();
	}
	p -> p_s.s_sp = RAMTOP;	/*set to ram top which is installed ram size + ram base address*/
	p -> p_s.s_pc = p -> p_s.s_t9 = (memaddr) test;
	/*process tree fields to NULL*/
	
	p -> p_time = ZERO;
	p -> p_semAdd = NULL;
	p -> p_supportStruct = NULL;
	
	/*init interupts as enabled
	procLocal timer enabled
	kernel mode on*/
	p -> p_s.s_status = ALLBITSOFF | TEON | IMON | IEPON;
	
	insertProcQ(&readyQueue, p);
	processCnt++;
	LDIT(IO); /*loading the interval timer with 100 milisec*/
	p = NULL;/*testing theory*/
	STCK(sTOD);
	scheduler();
	
	return ZERO;
}

