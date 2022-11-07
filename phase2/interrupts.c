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
/*#include "../h/scheduler.h"*/
/* #include "../phase2/exceptions.c" */
/* #include "../phase2/scheduler.c" */
#include "/usr/include/umps3/umps/libumps.h"

/* ---------Global Variables----------- */
extern pcb_PTR currentProc;
extern pcb_PTR readyQueue;
extern int processCnt;
extern int softBlockCnt;
extern int deviceSema4s[MAXDEVCNT];

extern pcb_PTR removeBlocked(int *semAdd);
extern void prepForSwitch();
void moveState(state_PTR source, state_PTR destination);
void scheduler();
/* ------------------------------------ */
cpu_t interruptStart;
cpu_t interruptStop;
int intLineN = 0;
int intDevN = 0;

/*HIDDEN int getDevice(int line){
	
	return device;
}*/

/*void interruptLineHandler(int line){
	non timer interrupt 
	if(line >2){
	
	}
}*/

/*returns index of highest priority device*/
/*int getIndexHPriority(int line){
	for(int i = 0; i < 8; i++){
		if(devAddrBase + ){

		}
	}
}*/

void nonTimerInt(int devNo){
	/*delcare variables*/
	/* devaddrBase */

	int devP = (intDevN-3) * DEVPERINT + devNo;
	
	int devAddrBase = 0x10000054 + ((intLineN - 3) * 0x80) + (devNo * 0x10); /*r28*/ /* first part is magic should be LOWMEM but this creates issues */

	device_PTR device = (device_PTR) devAddrBase;
	
	int status;
	
	if(intLineN < 7){
		status = device -> d_status;
		device->t_recv_command = ACK;
	}
		devP = devP + DEVPERINT;


	/*do the V*/
	int semad_PTR = deviceSema4s[devP];
	semad_PTR++;

	if(semad_PTR >= 0){
		int* s = &semad_PTR;
		pcb_PTR p = removeBlocked((s));

		if(p != NULL){
			p -> p_s.s_v0 = status;
			softBlockCnt--;
			insertProcQ(&readyQueue, p);
		}
	}
	prepForSwitch();
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
	LDST((state_PTR) BIOSDATAPAGE);/*saved exception state (located at the start of the BIOS Data Page */
}

void pltInt(state_PTR eState){/*process local timer interrupt*/
	/* Process Local Timer Interrupts (PLT) */
			/* syscall2 (terminating) cause or exception without having set a support structure address */
	
			/* transition from running to blocked state; 
			then execute 
				a sys3,
				sys5,
				or sys7 */
			
			/* Be interrupted by a PLT Interrupt */
				/*Current process has used up its time quantum/slice without compltIntng it CPU Burst, change from running to ready state */
				
			/*Ach the PLT interrupt by loading the timer with a new value */
			
			/* copy the processor state at the time of exception; located at the start of the BIOS data pg
				copy prrocesser state at the time of the exception into the Current processess's pcb (P_s) */
			
			/* place current process on the ready queue , tranisitioning current process from running state to the ready state */
			
			/* call the scheduler */
		LDIT(QUANTUM);
		moveState(eState, &(currentProc -> p_s));
		insertProcQ(&readyQueue, currentProc);
		scheduler();
}

/*void intTimerInt(){

}*/

int getLineN(unsigned int cause){
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

void intHandler(){
	state_PTR exState = (state_PTR) BIOSDATAPAGE;
	int ip = ((exState -> s_cause & IPMASK) >> IPSHIFT);
	/*find line number */
	if(ip & LINEONEON){
		/*in progress*/
		pltInt(exState);
		prepForSwitch();
	}else if(ip & LINETWOON){
		LDIT(interruptStart);
		STCK(interruptStart);
		int *clockS = &deviceSema4s[MAXDEVCNT-1];
		pcb_PTR p = (removeBlocked(clockS));/*store process */
		p = p; /*remove error, using variable*/
		deviceSema4s[MAXDEVCNT-1] = 0;
		prepForSwitch();
	}
	
	unsigned int lines[5] = {LINETHREEON, LINEFOURON, LINEFIVEON, LINESIXON, LINESEVENON};
	int i = 3;
	while((i < 8 && intLineN == 0)){
		if(ip & lines[i]){
			intLineN = i;
		}
		i++;
	}

	devregarea_t * ram = (devregarea_t *) RAMBASEADDR;
	int dev = ram -> interrupt_dev[intLineN-3]; /*devBits*/
	
	/*locate device number */
	intDevN = -1;
	i = 0;
	while((i < 8 && intDevN == -1)){
		if(dev & (i+1)){
			intDevN = i;
		}
		i++;
	}
	if(intLineN >= 3){
		nonTimerInt(intDevN);
	}
}
