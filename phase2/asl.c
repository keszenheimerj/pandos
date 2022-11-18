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
#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/const.h"

int nullHit;
int semTest;

HIDDEN	semd_t *semdActive_h, *semdFree_h;

/*Functions*/
/*
* Function: Loop return parent of node if 
* there or parent of node if not there.
*/
semd_t*		searchAdd(int* checkVal){
	semd_t *loopingSem = semdActive_h;
	
	while((loopingSem -> s_next -> s_semAdd) < checkVal){
		loopingSem = loopingSem -> s_next;
	}
	return loopingSem;
}

semd_t	*search(int *semAdd){
	semd_t *temp = semdActive_h -> s_next;
	semd_t *prnt = semdActive_h;
	while(temp -> s_semAdd <= semAdd){
		if(temp->s_semAdd == semAdd){
			return prnt;
		}
		prnt = temp;
		temp = temp -> s_next;
	}
	return prnt;
}
/*
* Function: Allocates a semd_t from the semd_t
* free list and returns a pointer to it.
*/
semd_t* 	allocSemd(int *semAdd) {
	semd_t *loopingSem = semdActive_h;
	while((loopingSem -> s_next -> s_semAdd) < semAdd){
		loopingSem = loopingSem -> s_next;
	}
	semd_t *semmToAlloc = loopingSem -> s_next;
	loopingSem -> s_next = loopingSem -> s_next -> s_next;
	return (semmToAlloc);
}

/*
* Function: Takes a semd_t and points it to
* the semd_t free list. 
*/
void 	freeSemd(semd_t *free) {
	free -> s_next = semdFree_h;
	semdFree_h = free;
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
	semd_t *insertP = search(semAdd); 					/*Find parent of semAdd. insertP = parent of semAdd*/
	if(insertP -> s_next -> s_semAdd == semAdd){ 			/*Is semAdd already in the active list? If Found*/
		pcb_PTR *tproc = &(insertP -> s_next -> s_procQ);
		insertProcQ(tproc, p); 
		p -> p_semAdd = semAdd;
		return 0;
	}else{													
		if(semdFree_h == NULL){
			return 1;
		}
		semd_t *storeFree = semdFree_h;
		semdFree_h = semdFree_h -> s_next;
		
		storeFree -> s_next = insertP -> s_next;
		insertP -> s_next = storeFree;
		storeFree -> s_semAdd = semAdd;
		storeFree -> s_procQ = mkEmptyProcQ();
		insertProcQ(&(storeFree -> s_procQ), p);
		p -> p_semAdd = semAdd;															
	}
	return 0;												
	/*semAdd is not in the active list. Not Found*/
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
	nullHit = 0;
	semd_t *prnt = searchAdd(semAdd);
	semd_t *child = prnt -> s_next;/*error line */
	
	if(child -> s_semAdd != semAdd){
		nullHit = 1;/*this method doesn't work!!!!*/
		return NULL;
	}
	pcb_t* outP = removeProcQ(&(child -> s_procQ));
	semd_t *off;
	if(emptyProcQ(child -> s_procQ)){
		off = child;
		prnt -> s_next = child -> s_next;
		
	}
	freeSemd(off);
	outP -> p_semAdd = NULL;
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
	semd_t *child = searchAdd(p -> p_semAdd) -> s_next;
	if(child -> s_semAdd != p -> p_semAdd){
		return NULL;
	}
	pcb_t *outP = outProcQ(&(child -> s_procQ), p);
	/*check null condition*/
	if(outP != NULL && emptyProcQ(child -> s_procQ)){
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
	semd_t *parent = searchAdd(semAdd);
	if(parent -> s_next -> s_semAdd != semAdd){
		return NULL;
	}
	return (headProcQ(parent ->s_next -> s_procQ));
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
	semdPool[21].s_semAdd = (int *)INF; /*INF = 0x7fffffff;*/

	/*set up active*/
	semdActive_h = (&semdPool[0]);
	semdActive_h -> s_procQ = mkEmptyProcQ();
	semdActive_h -> s_next = &semdPool[21];
	semdActive_h -> s_next -> s_procQ = mkEmptyProcQ();
	semdActive_h -> s_next -> s_next = NULL;
	return;
}
