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
#include "/usr/include/umps3/umps/libumps.h"

/* ---------Global Variables----------- */
extern pcb_PTR currentProc;
extern pcb_PTR readyQueue;
extern int processCnt;
extern int softBlockCnt;
extern int deviceSema4s[MAXDEVCNT];
extern cpu_t sTOD;

extern pcb_PTR removeBlocked(int *semAdd);
extern void prepForSwitch();
extern void switchContext(state_PTR s);
void moveState(state_PTR source, state_PTR destination);
void copyState(state_PTR source, state_PTR destination);
void scheduler();
/* ------------------------------------ */
cpu_t interruptStop;
int intLineN;
int intDevN;
int ip;
int check = ZERO;
int status;
int devP;
int *semC;

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

void deviceInterrupt(int dev, int intDevN, int intLineN){
	/*delcare variables*/
	/* devaddrBase */

	devP = (intLineN-DEVICEOFFSET) * DEVPERINT + intDevN;
	
	int devAddrBase = 0x10000054 + ((intLineN - DEVICEOFFSET) * 0x80) + (intDevN * 0x10); /*r28*/ /* first part is magic should be LOWMEM but this creates issues */

	device_PTR device = (device_PTR) devAddrBase;
	
	status = ZERO;
	check = ZERO;
	if(intLineN != 7){
		status = device -> d_status;
		device->d_command = ACK;
		check = 1;
	}else{/*device is is 7*/
		if(device -> t_transm_command & TRANSON){
			status = device ->t_transm_status;
			device -> t_transm_command = ACK;
			device->d_command = ACK;
			check = 2;
			/*status = device -> d_status;
			device->d_command = ACK;*/
		}else{
			status = device -> t_recv_status;
			device -> t_recv_command = ACK;
			devP = devP + DEVPERINT;
			check = 3;
			/*status = device -> d_status;
			device->d_command = ACK;*/
		}
		
	}


	/*do the V*/
	/*int *sem = &deviceSema4s[devP];
	(*sem)++;
	semC = sem;*/
	deviceSema4s[devP]++;
	
	
	if((deviceSema4s[devP]) <= ZERO){/**sem  >=*/
		/*int* s = (sem);*/
		pcb_PTR p = removeBlocked(&(deviceSema4s[devP]));
		softBlockCnt--;
		/*if(p != NULL){
			STCK(interruptStop);
			p -> p_time = (p -> p_time) + (interruptStop - sTOD);
			p -> p_s.s_v0 = status;
			softBlockCnt--;
			insertProcQ(&readyQueue, p);
		}*/
		STCK(interruptStop);
		p -> p_time += (interruptStop - sTOD);
		p -> p_s.s_v0 = status;
		
		insertProcQ(&readyQueue, p);
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
	/*LDST((state_PTR) BIOSDATAPAGE);saved exception state (located at the start of the BIOS Data Page */
}

void pltInt(state_PTR eState){/*process local timer interrupt*/
	/*Current process has used up its time quantum/slice without compltIntng it CPU Burst, change from running to ready state */
		
	/*Ack the PLT interrupt by loading the timer with a new value */
	setTIMER(IO);
	if(currentProc != NULL){
		/* copy the processor state at the time of exception; located at the start of the BIOS data pg
			copy prrocesser state at the time of the exception into the Current processess's pcb (P_s) */
		moveState(eState, &(currentProc -> p_s));/*17.11moveState*/
		/* place current process on the ready queue , tranisitioning current process from running state to the ready state */
		insertProcQ(&readyQueue, currentProc);
	}
	scheduler();
}

void intervalTimerInterrupt(){
	LDIT(IO);
	STCK(interruptStop);
	int *clockS = &deviceSema4s[MAXDEVCNT-1];
	pcb_PTR p = (removeBlocked(clockS));/*store process */
	while(p != NULL){
		p->p_time += (interruptStop - sTOD);
		insertProcQ(&readyQueue, p);
		p = removeBlocked(clockS);
		softBlockCnt--;
	}
	*clockS = ZERO;
	prepForSwitch();	
	
}


int getLine(state_PTR exState){
	/*find where an interrupt is pending*/
	ip = ((exState -> s_cause))>>IPSHIFT;
	if(ip & LINEZEROON){
		return 0;
	}else if((ip & LINEONEON) != ZERO){
		return 1;
	}else if((ip & LINETWOON) != ZERO){
		return 2;
	}
	
	unsigned int lines[8] = {LINEZEROON, LINEONEON, LINETWOON, LINETHREEON, LINEFOURON, LINEFIVEON, LINESIXON, LINESEVENON};
	int i = DEVICEOFFSET;
	intLineN = ZERO;
	while((i < 8 && intLineN == ZERO)){
		if((ip & lines[(i)]) != ZERO){
			intLineN = i;
		}
		i++;
	}
	return intLineN;
}

int getDevLine(int dev){
	unsigned int devlines[8] = {DEVLINEZEROON, DEVLINEONEON, DEVLINETWOON, DEVLINETHREEON, DEVLINEFOURON, DEVLINEFIVEON, DEVLINESIXON, DEVINESEVENON};
	/*locate device number */
	intDevN = -1;
	int i = ZERO;
	while((i < 8 && intDevN == -1)){
		if((dev & devlines[(i)]) != ZERO){
			intDevN = i;
		}
		i++;
	}
	return intDevN;
}
/*
*the interrupt handler is called by the generalExceptionHandler anytime an interrupt line is on somewhere
*The 0th line should never be on for our uniprocessor environment, but another line should always be on when this function is called
*we will find the line
*if a device line then find which device
*ensure the line is acknowledged
*/
void intHandler(){
	/*find exception state*/
	state_PTR exState = (state_PTR) BIOSDATAPAGE;
	
	/*find line number */
	int line = getLine(exState);
	
	/*decide what to de based on line number*/
	if(line == 0){
		/*intended for uniprocessor environments only*/
		PANIC();
	}else if(line == 1){
		/*process local timer interrupt*/
		pltInt(exState);
	}else if(line == 2){/*pg 34*/
		/*intervalTimerInterrupt*/
		LDIT(IO);
		STCK(interruptStop);
		int *clockS = &deviceSema4s[MAXDEVCNT-1];
		pcb_PTR p = (removeBlocked(clockS));/*store process */
		while(p != NULL){
			p->p_time += (interruptStop - sTOD);
			insertProcQ(&readyQueue, p);
			p = removeBlocked(clockS);
			softBlockCnt--;
		}
		*clockS = ZERO;
		prepForSwitch();
	}
	
	devregarea_t * ram = (devregarea_t *) RAMBASEADDR;
	int dev = ram -> interrupt_dev[intLineN-DEVICEOFFSET]; /*devBits*/
	int intDevN = getDevLine(dev);
	
	if(intLineN >= DEVICEOFFSET){
		deviceInterrupt(dev, intDevN, line);
	}
}
