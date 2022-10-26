/*H***************************************************************************************************
* FILENAME :	interrupts.c
*
* DESCRIPTION :	
*	example
*
* PUBLIC FUNCTIONS : 
*	
* AUTHORS :	James Keszenheimer, Evan Hanson		START DATE : 31 Aug 22
*
*H*/

#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/scheduler.h"
#include "../h/exceptions.h"
#include "../h/initial.h"

cpu_t interruptStartTime;

HIDDEN int getDevice(int line){
	
	return device;
}

void interruptLineHandler(int line){
	/* non timer interrupt */
	if(line >2){
	
	}
}

/*returns index of highest priority device*/
int getIndexHPriority(int line){
	for(int i = 0; i < 8; i++){
		if(devAddrBase + 
	}
}

void nonTimerI(int line){
	
	int DevNo = s_a2;
	
	int devAddrBase = LOWMEM + ((IntLineNo - 3) * 0x80) + (DevNo * 0x10); /*r28*/
	/* Non-timer interrupt: device interrupt */
	
			/* calculate address of device's device register */
			
			/* save off status code of device's device register */
			
			/* acknowledge the outstanding interrupt */
				/* write acklg command code into the interrupting device's device register*/
				/* change the device's device register back to it's standard value */
			
			/* perform a V operation on the Nuclkeus maintained semaphore associated wit hthis subdevice */
				/* unblock the process (pcb) removeBlocked, initiating the I/) operation */
				/* then requested to wait for its completion via a sys5 operation */
				
				/*V opperation can return NULL if there is no PROCESS CONTROL BLOCK PCB
				if(V return == NULL){
					LDST(..);
					return;
				}*/
				/*if scheduler executed a wait instruction LDST return */
				
				
			/* place stored status code in the unblocked pcb's v0 register */
			
			/* insert the newly unblocked pcb onto the Ready Queue, changing the state from blocked to ready */
			
			/* return control to the current process */
			LDST(/*saved exception state (located at the start of the BIOS Data Page */);
}

void pltI(){/*process local timer interrupt*/
	/* Process Local Timer Interrupts (PLT) */
			/* syscall2 (terminating) cause or exception without having set a support structure address */
			
			/* transition from running to blocked state; 
			then execute 
				a sys3,
				sys5,
				or sys7 */
			
			/* Be interrupted by a PLT Interrupt */
				/*Current process has used up its time quantum/slice without complting it CPU Burst, change from running to ready state */
				
			/*Ach the PLT interrupt by loading the timer with a new value */
			
			/* copy the processor state at the time of exception; located at the start of the BIOS data pg
				copy prrocesser state at the time of the exception into the Current processess's pcb (P_s) */
			
			/* place current process on the ready queue , tranisitioning current process from running state to the ready state */
			
			/* call the scheduler */
			scheduluer();
}

void intTimerI(){

}

int getLine(unsigned int cause){
	/* declare the array of possible line numbers */
    unsigned int lineNumbers[SEMDEVICE] = {4, 5, 6, 7, 8};
    /* declare the array of possible device numbers */
    unsigned int devices[SEMDEVICE] = {DISKINT, FLASHINT, NETWINT, PRNTINT, TERMINT};
    int i;
    /* what was our line number? */
    int found = 0;
    /* loop through each possible device */
    for (i = 0; i < SEMDEVICE; i++) {
        if(((cause) & (lineNumbers[i])) != 0) {
            /* match the line number with the device */
            found = devices[i];
        }
    }
    /* we found it */
    return found;


	/* (state_t)BIOSDATAPAGE s_cause
	for(int i = 0; i < 8; i++){
		if( (LOWMEM + (i  * 0x80))  ){
			
		}
	} */
}

void interruptHandler(){
	int IntLineNo = s_a1;
	if(if IntLineNo == 1){
		pltI(IntLineNo);
	}else if(IntLineNo == 2){
		intTimerI();
	}else if(IntLineNo > 2 && IntLineNo < 8){ 
		nonTimerI();
	}else{
		PANIC();
	}
}
