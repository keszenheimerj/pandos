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


/*
*****************global variables*****************
*/

pcb_t* readyQueue = mkEmptyQ();
pcb_t* currentProc = NULL; /*scaler to the running Proc*/
int processCnt = 0;
int softBlockCnt = 0;
int deviceSema4s[42]; /*42 | 49; =0??*/

/*main*/
/*ref
	tlb_refil handler
	genException handler
*/
main{
	initPcbs();
	initASL();
	init global Variables
	
	/*init 4 words in BIOS pg*/
	xxx -> tlb_refill_handler = (memaddr) uTLB_RefillHandler;
	tlb_refill_stackPtr
	xxx -> exception_handler = (memaddr) fooBar;
	exception_stackPtr
		
	allocPcb()
	create 1 process
	p = allocPcb();
	yyy -> p_s -> s_pc = (memaddr) test;
	process Cnt ++;
	insertProcQ(&readyQueue, p; #statis is ready
	scheduler();#dequeue remove PRocQ
}
