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
#include "../h/initial.h"
#include "../h/types.h"
#include "../h/const.h"
#include "/usr/include/umps3/umps/libumps.h"

extern void test();


/*
*****************global variables*****************
*/
pcb_t* readyQueue = mkEmptyQ();
pcb_t* currentProc = NULL; 	/*scaler to the running Proc*/
int processCnt = 0;		/*int indicating the started but not terminated processes*/
int softBlockCnt = 0;		/*a process can either be ready, running, blocked(waiting) state and this int is the number of started, but not terminated processes*/
int deviceSema4s[49] = 0; /*42 | 49; =0??*/
/*
************end global variables**************
*/


void uTLB_RefillHandler () {
	setENTRYHI(0x80000000);
	setENTRYLO(0x00000000);
	TLBWR();
	LDST ((state_PTR) 0x0FFFF000);
}

void genExceptionHanler(){
	/*save state*/
	state_PTR peviousStatePTR = state_PTR;
	/*make ptr to from bios*/
	int causeNum = (previousStatePTR -> s_cause && CauseExcCode) >> CAUSESHIFT;
	/*do bitwise stuff*/
	if(causeNum == 0){
		/*pass proccessing to nucleus dev interupt handler*/
		
	}else if(causeNum <= 3){
		/*tlb execption, pass proccessing to tlb-exception handler*/
		
	}else if(causeNum == 8){
		sysCall(previousStatePTR);
	}else{
		/*programTrap*/
	}
}

/*main*/
main{
	initPcbs();
	initASL();
	
	/*init 4 words in BIOS pg*/
	passupvector	passV -> tlb_refill_handler = (memaddr) uTLB_RefillHandler;
	passV -> tlb_refill_stackPtr = RAMTOP;
	passV -> exception_handler =  (memaddr) genExceptionHanler();
	passV -> exception_stackPtr = RAMTOP; 
	
	LDIT = .1 /*loading the interval timer with 100 milisec*/
	pcb_t p = allocPcb();
	/*
	sp = RAMTOP
	pc is set to address of test*/
	s_sp = RAMTOP;	/*set to ram top which is installed ram size + ram base address*/
	p -> p_s.s_pc = s_t9 = (memaddr) test;
	/*process tree fields to NULL*/
	
	
	/*done*/
	
	p -> p_time = 0;
	p -> p_semAdd = NULL;
	p -> p_supportStruct = NULL;
	/*turn kernal mode on?*/
	/*init interupts as enabled
	procLocal timer enabled
	kernel mode on*/
	p -> p_s.s_status = ALLOFF | TEBITON | IMON | IEPBIT;

	processCnt ++;
	insertProcQ(&readyQueue, p); /*statis is ready*/
	scheduler();			/*dequeue remove PRocQ*/
}


