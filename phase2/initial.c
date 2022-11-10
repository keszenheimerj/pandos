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
extern unsigned int SYS();
extern void scheduler();
extern void prepForSwitch();
extern void uTLB_RefillHandler();

pcb_PTR readyQueue;
pcb_PTR currentProc; 	/*scaler to the running Proc*/
int processCnt;		/*int indicating the started but not terminated processes*/
int softBlockCnt;		/*a process can either be ready, running, blocked(waiting) state and this int is the number of started, but not terminated processes*/
int deviceSema4s[MAXDEVCNT]; /*42 | 49; =0??*/
cpu_t startTime;
/*
************end global variables**************
*/


/*void uTLB_RefillHandler2() {
	setENTRYHI(0x80000000);
	setENTRYLO(0x00000000);
	TLBWR();
	LDST ((state_PTR) 0x0FFFF000);
}exists in p2test*/

void genExceptionHandler(){
	/*save state*/
	state_PTR previousStatePTR = (state_PTR) (BIOSDATAPAGE);
	/*make ptr to from bios*/
	int causeNum = ((previousStatePTR -> s_cause) >> 2);/* & Cause*/
	/*do bitwise stuff*/
	if(causeNum == 0){
		/*pass proccessing to nucleus dev interupt handler*/
		intHandler();
	}else if(causeNum == 8){
		SYS();/*previousStatePTR could be passed but unneccessary previousStatePTR -> s_a0, previousStatePTR -> s_a1, previousStatePTR -> s_a2, previousStatePTR -> s_a3*/
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
	processCnt = 0;
	softBlockCnt = 0;
	int i = 0;
	while(i < MAXDEVCNT){
		deviceSema4s[i] = 0;
		i++;
	}

	
	
	
	pcb_PTR p = allocPcb();/*reached*/
	if(p == NULL){
		PANIC();
	}
	/*
	sp = RAMTOP
	pc is set to address of test*/
	p -> p_s.s_sp = RAMTOP;	/*set to ram top which is installed ram size + ram base address*/
	p -> p_s.s_pc = p -> p_s.s_t9 = (memaddr) test;
	/*process tree fields to NULL*/
	
	
	/*done*/
	
	/*p -> p_time = 0;
	p -> p_semAdd = NULL;*/
	p -> p_supportStruct = NULL;
	/*turn kernal mode on?*/
	/*init interupts as enabled
	procLocal timer enabled
	kernel mode on*/
	p -> p_s.s_status = ALLBITSOFF | TEON | IMON | IEPON;

	
	insertProcQ(&readyQueue, p); /*statis is ready*/
	processCnt = processCnt + 1;
	LDIT(IO); /*loading the interval timer with 100 milisec*/
	p = NULL;/*testing theory*/
	STCK(startTime);
	scheduler();/*gets called*/			/*dequeue remove PRocQ*/
	
	return 0;
}

