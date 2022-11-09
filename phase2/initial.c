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
/*#include "../phase2/exceptions.c" 
#include "../phase2/interrupts.c"*/
/*#include "../phase2/p2test.c"*/
/* #include "../phase2/scheduler.c" */
#include "/usr/include/umps3/umps/libumps.h"

extern void test();
extern void passUpOrDie(state_t *exState, int exType);


/*
*****************global variables*****************
*/

extern void intHandler();
extern unsigned int SYS(unsigned int number, unsigned int arg1, unsigned int arg2, unsigned int arg3);
extern void scheduler();
extern void prepForSwitch();

pcb_PTR readyQueue;
pcb_PTR currentProc = NULL; 	/*scaler to the running Proc*/
int processCnt = 0;		/*int indicating the started but not terminated processes*/
int softBlockCnt = 0;		/*a process can either be ready, running, blocked(waiting) state and this int is the number of started, but not terminated processes*/
int deviceSema4s[MAXDEVCNT]; /*42 | 49; =0??*/
cpu_t startTime;
/*
************end global variables**************
*/


void uTLB_RefillHandler2() {
	setENTRYHI(0x80000000);
	setENTRYLO(0x00000000);
	TLBWR();
	LDST ((state_PTR) 0x0FFFF000);
}/*exists in p2test*/

void genExceptionHandler(){
	/*save state*/
	state_PTR previousStatePTR = (state_PTR) (BIOSDATAPAGE);
	/*make ptr to from bios*/
	int causeNum = (previousStatePTR -> s_cause & Cause) >> CAUSESHIFT;
	/*do bitwise stuff*/
	if(causeNum == 0){
		/*pass proccessing to nucleus dev interupt handler*/
		intHandler();
	}else if(causeNum == 8){
		SYS(previousStatePTR -> s_a0, previousStatePTR -> s_a1, previousStatePTR -> s_a2, previousStatePTR -> s_a3);/*previousStatePTR could be passed but unneccessary */
	}else{
		/*tlb execption, pass proccessing to tlb-exception handler*/
		if(causeNum < 4){
			passUpOrDie((state_PTR) BIOSDATAPAGE, 0); /*PGF*/
		}else{
			passUpOrDie((state_PTR) BIOSDATAPAGE, 1); /*general*/
		}
		/*programTrap*/
	}
}

/*main*/
int main(){
	readyQueue = mkEmptyProcQ();
	int i = 0;
	while(i < MAXDEVCNT){
		deviceSema4s[i] = 0;
		i++;
	}

	initPcbs();
	initASL();
	
	
	
	/*init 4 words in BIOS pg*/
	passupvector_PTR	passV = (passupvector_t *) PASSUPVECTOR;
	passV -> tlb_refill_handler = (memaddr) uTLB_RefillHandler2;
	passV -> tlb_refill_stackPtr = RAMTOP;
	passV -> exception_handler =  (memaddr) genExceptionHandler;
	passV -> exception_stackPtr = RAMTOP; 
	
	LDIT(INTERVALTMR); /*loading the interval timer with 100 milisec*/
	pcb_PTR p = allocPcb();
	/*
	sp = RAMTOP
	pc is set to address of test*/
	p -> p_s.s_sp = RAMTOP;	/*set to ram top which is installed ram size + ram base address*/
	p -> p_s.s_pc = p -> p_s.s_t9 = (memaddr) test;
	/*process tree fields to NULL*/
	
	
	/*done*/
	
	p -> p_time = 0;
	p -> p_semAdd = NULL;
	p -> p_supportStruct = NULL;
	/*turn kernal mode on?*/
	/*init interupts as enabled
	procLocal timer enabled
	kernel mode on*/
	p -> p_s.s_status = ALLBITSOFF | TEON | IMON | IEPON;

	processCnt ++;
	insertProcQ(&(readyQueue), p); /*statis is ready*/
	scheduler();			/*dequeue remove PRocQ*/
	return 0;
}

