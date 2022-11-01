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

#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/types.h"
#include "../h/const.h"
/* #include "../phase2/exceptions.c" */
/* #include "../phase2/scheduler.c" */
/* #include "/usr/include/umps3/umps/libumps.h" */

cpu_t interruptStartTime;
int IntLineNo = 0;

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

void nonTimerI(int devNo){
	int devP = (intDevNo-3) * DEVPERINT + devNo;
	
	int devAddrBase = LOWMEM + ((IntLineNo - 3) * 0x80) + (devNo * 0x10); /*r28*/

	device_t_PTR device = (device_PTR) devAddrBase;
	
	int status;
	
	if(intLineNo < 7){
		status = device -> d_status;
		device->t_recv_command = ACK;
	}
		devP = devP + DEVPERINT;
		
	}


	/*do the V*/
	int semad_PTR = &sema4Dev[device];
	semad_PTR++;

	if(semadPTR >= 0){
		pcb_PTR p = removerBlocked(&semad_PTR);

		if(p != NULL){
			p -> p_s.s_v0 = status;
			softBlockCnt--;
			insertProcQ(&readyQueue, p);
		}
	}
	prepToSwitch();
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

void pltI(state_PTR eState){/*process local timer interrupt*/
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
		LDIT(QUANTUM);
		moveState(eState, curreentProc -> p_s);
		insertProcQ(&readyQueue, currentProc);
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
	state_PTR exState = (state_PTR) BIOSDATAPAGE;
	int ip = ((exState -> s_cause & IPMASK) >> IPSHIFT);
	/*find line number */
	if(ip & LINEONEON){
		/*in progress*/
		pltI(exState);
		prepToSwitch();
	}else if(ip & LINETWOON){
		LDIT(
		STCK(interruptStartTime);
		pcb_PTR p = removeBlocked(sem/);
		sem = 0;
		prep
		prepToSwitch();
	}
	unsigned int lines[5] = {LINETHREEON, LINEFOURON, LINEFIVEON, LINESIXON, LINESEVENON};
	for(int i = 3; (i < 8 && IntLineNo == 0); i++){
		if(ip & lines[i]){
			IntLineNo = i;
		}
	}

	devregarea_t * ram = (devregarea_t *) RAMBASEADDR;
	device_PTR dev = ram -> interrupts_dev[IntLineNo-3]; /*devBits*/
	
	/*locate device number */
	int intDevNo = -1;
	
	for(int i = 0; (i < 8 && intDevNo == -1); i++){
		if(dev -> d_status & i+1){
			devNo = i;
		}
	}
	if(intLineNo >= 3){
		nonTimerI(devNo);
	}
}
