/*H***************************************************************************************************
* FILENAME :	asl.c
*
* DESCRIPTION :	
*	The Active Semaphore list management of structures: semd_t.
*
* PUBLIC FUNCTIONS : 
*	semd_t	searchAdd()			- Loop return parent of node if there or parent of node if not there.
*	semd_t 	allocSemd()			- Allocates a semd_t from the semd_t free list and returns a pointer to it.
* 	void	freeSemd() 			- Takes a semd_t and points it to the semd_t free list. 
*	int 	insertBlocked()		- Insert the pcb pointed to by p at the tail of the process queue as-
*								sociated with the semaphore whose physical address is semAdd and
*								set the semaphore address of p to semAdd.
*	pcb_t 	removeBlocked()		- Search the ASL for a descriptor of this semaphore. If none is
*								found, return NULL; otherwise, remove the first (i.e. head) pcb from
*								the process queue of the found semaphore descriptor and return a
*								pointer to it.
* 	pcb_t 	outBlocked()		- Remove the pcb pointed to by p from the process queue associated
*								with p’s semaphore (p→ p semAdd) on the ASL. If pcb pointed
*								to by p does not appear in the process queue associated with p’s
*								semaphore, which is an error condition, return NULL; otherwise, re-
*								turn p.
* 	pcb_t 	headBlocked()		- Return a pointer to the pcb that is at the head of the process queue
*								associated with the semaphore semAdd. Return NULL if semAdd is
*								not found on the ASL or if the process queue associated with semAdd
*								is empty.
*	void	initASL()			- Initialize the semdFree list to contain all the elements of the array
*								static semd t semdTable[MAXPROC].
* 	
* AUTHORS :	James Keszenheimer, Evan Hanson		START DATE : 5 Sept 22
*
*H*/
/*#include "./pcb.c"*/
#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/const.h"

HIDDEN	semd_t *semdActive_h, *semdFree_h;

/*Functions*/
/*
* Function: Loop return parent of node if 
* there or parent of node if not there.
*/
semd_t*		searchAdd(int* checkVal){
	semd_t *temp = semdActive_h;
	while((temp -> s_next -> s_semAdd) < checkVal){
		temp = temp -> s_next;
	}
	return temp;
}

/*
* Function: Allocates a semd_t from the semd_t
* free list and returns a pointer to it.
*/
semd_t* 	allocSemd(int *semAdd) {
	semd_t *asemd = semdActive_h;
	while((asemd -> s_next -> s_semAdd) < semAdd){
		asemd = asemd -> s_next;
	}
	semd_t *rout = asemd -> s_next;
	asemd -> s_next = asemd -> s_next -> s_next;
	return (rout);
}

/*
* Function: Takes a semd_t and points it to
* the semd_t free list. 
*/
void 	freeSemd(semd_t *s) {
	s -> s_next = semdFree_h;
	semdFree_h = s;
}

/*
* Function: Insert the pcb pointed to by p at the tail of the process queue
* associated with the semaphore whose physical address is semAdd and
* set the semaphore address of p to semAdd. If the semaphore is
* currently not active (i.e. there is no descriptor for it in the ASL), allocate
* a new descriptor from the semdFree list, insert it in the ASL (at the
* appropriate position), initialize all of the fields (i.e. set s semAdd
* to semAdd, and s procq to mkEmptyProcQ()), and proceed as
* above. If a new semaphore descriptor needs to be allocated and the
* semdFree list is empty, return TRUE. In all other cases return FALSE.
*/
int 	insertBlocked(int *semAdd, pcb_t *p){
	semd_t *insertP = searchAdd(semAdd); 					/*Find parent of semAdd. insertP = parent of semAdd*/
	if(insertP -> s_next -> s_semAdd == semAdd){ 			/*Is semAdd already in the active list? If Found*/
		pcb_PTR *tproc = &(insertP -> s_next -> s_procQ);
		insertProcQ(tproc, p); 
		p -> p_semAdd = semAdd;
		return 0;
	}else{													
		if(semdFree_h == NULL){
			return 1;
		}
		semd_t *tempF = semdFree_h;
		semdFree_h = semdFree_h -> s_next;
		
		tempF -> s_next = insertP -> s_next;
		insertP -> s_next = tempF;
		tempF -> s_semAdd = semAdd;
		tempF -> s_procQ = mkEmptyProcQ();
		insertProcQ(&(tempF -> s_procQ), p);
		p -> p_semAdd = semAdd;															
	}
	return 0;												/*semAdd is not in the active list. Not Found*/
}

/*
* Function: Search the ASL for a descriptor of this semaphore. If none is
* found, return NULL; otherwise, remove the first (i.e. head) pcb from
* the process queue of the found semaphore descriptor and return a
* pointer to it. If the process queue for this semaphore becomes empty
* (emptyProcQ(s procq) is TRUE), remove the semaphore de-
* scriptor from the ASL and return it to the semdFree list.
*/
pcb_t 	*removeBlocked(int *semAdd){
	semd_t *insertP = searchAdd(semAdd);
	if(insertP -> s_next -> s_semAdd != semAdd){
		return NULL;
	}
	pcb_t *outP = removeProcQ(&(insertP -> s_next -> s_procQ));
	if(emptyProcQ(insertP -> s_next -> s_procQ)){
		
		freeSemd(allocSemd(semAdd));
	}
	return outP;
}

/*
* Function: Remove the pcb pointed to by p from the process queue associated
* with p’s semaphore (p → p semAdd) on the ASL. If pcb pointed
* to by p does not appear in the process queue associated with p’s
* semaphore, which is an error condition, return NULL; otherwise,
* return p.
*/
pcb_t 	*outBlocked(pcb_t *p){
	semd_t *insertP = searchAdd(p -> p_semAdd);
	if(insertP -> s_next -> s_semAdd != p -> p_semAdd){
		return NULL;
	}
	pcb_t *outP = outProcQ(&(insertP -> s_next -> s_procQ), p);
	if(outP == NULL){
		return NULL;
	}
	if(emptyProcQ(insertP -> s_next -> s_procQ)){
		freeSemd(allocSemd(p -> p_semAdd));
	}	
	return outP;
}

/*
* Function: Return a pointer to the pcb that is at the head of the process queue
* associated with the semaphore semAdd. Return NULL if semAdd is
* not found on the ASL or if the process queue associated with semAdd
* is empty.
*/
pcb_t 	*headBlocked(int *semAdd){
	semd_t *temp = searchAdd(semAdd);
	if(temp -> s_next -> s_semAdd != semAdd){
		return NULL;
	}
	return (headProcQ(temp ->s_next -> s_procQ));
}

/*
* Function: Initialize the semdFree list to contain all the 
* elements of the array static semd_t semdPool[MAXPROC].
* This function will be only called once during data structure 
* initialization. 
*/
void 	initASL(){
	semdFree_h = NULL;
	semdActive_h = NULL;

	static semd_t semdPool[MAXPROC+2];
	int i = 1;
	for(i = 1; i <= MAXPROC; i++){
		freeSemd(&(semdPool[i]));
	}

	semdPool[0].s_semAdd = 0;
	semdPool[21].s_semAdd = (int *)0x7fffffff;

	/*set up active*/
	semdActive_h = (&semdPool[0]);
	semdActive_h -> s_procQ = mkEmptyProcQ();
	semdActive_h -> s_next = &semdPool[21];
	semdActive_h -> s_next -> s_procQ = mkEmptyProcQ();
	semdActive_h -> s_next -> s_next = NULL;
	return;
}
