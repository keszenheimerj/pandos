/*H***************************************************************************************************
* FILENAME :	initial.c
*
* DESCRIPTION :	
*	PcbQueues file management of queues of structures: pcb’s.
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
#include "/usr/include/umps3/umps/libumps.h"

/*
*****************global functions*******************
*/
extern void test();
extern void passUpOrDie(state_t *exState, int exType);
/*
**************end global functions*******************
*/

/*
*****************global variables*****************
*/

extern void intHandler();
extern unsigned int SYS();
extern void scheduler();
extern void prepForSwitch();
extern void uTLB_RefillHandler();



pcb_PTR readyQueue;
pcb_PTR currentProc; 		/*scaler to the running Proc*/
int processCnt;			/*int indicating the started but not terminated processes*/
int softBlockCnt;		/*a process can either be ready, running, blocked(waiting) state and this int is the number of started, but not terminated processes*/
int deviceSema4s[MAXDEVCNT]; 	/*49; last is sudo clock*/
cpu_t sTOD;
int causeNum;
/*
************end global variables**************
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

/*main*/
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

