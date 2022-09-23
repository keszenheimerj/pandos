/*H***************************************************************************************************
* FILENAME :	asl.c
*
* DESCRIPTION :	
*	The Active Semaphore list management of structures: semd_t.
*
* PUBLIC FUNCTIONS : 
*	semd_t	searchAdd()			- Loop return parent of node if there or parent of node if not there.
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

/*2.4*/
HIDDEN	semd_t *semdActive_h, *semdFree_h;

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
* Function: allocates a semd_t from the semd_t
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
* Function: takes a semd_t and points it to
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

pcb_t 	*outBlocked(pcb_t *p){
	/* Remove the pcb pointed to by p from the process queue associated
	with p’s semaphore (p→ p semAdd) on the ASL. If pcb pointed
	to by p does not appear in the process queue associated with p’s
	semaphore, which is an error condition, return NULL; otherwise, re-
	turn p. */
	/*outProcQ(){
		identical to remove blocked but instead of remove process queue call outproQ
		accesser
	}*/
	/*semd_t *parent = searchAdd(p-> p_semAdd);
	semd_t *semAsemd = parent -> s_next;
	if(semAsemd -> s_semAdd != p -> p_semAdd){
		/*error case*/
		/*return NULL;
	/*}
	pcb_t *outP = outProcQ(&semAsemd -> s_procQ, p);
	if(emptyProcQ(semAsemd -> s_procQ)){/*if list is empty we have to move semd to free list*/
		/*remove from active*/
	/*parent -> s_next = parent -> s_next -> s_next;
		/*add to free*/
		/*semAsemd = semdFree_h;
		semdFree_h = semAsemd;
	}*/
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
pcb_t 	*headBlocked(int *semAdd){
	/* Return a pointer to the pcb that is at the head of the process queue
	associated with the semaphore semAdd. Return NULL if semAdd is
	not found on the ASL or if the process queue associated with semAdd
	is empty. */

	/*use search, which gives you the head and then you test of you have that child*/
	/*if childed seached is not there, search will give you the parent, so you will have to 
	check the parents next to see if the semAdd is actually there, if it is not there, return NULL.*/
	/*You need to get the head of s_procQ.
	return (headProc(search(child) -> s_ProcQ))
	one more test, if s_procQ is empty, return NULL*/
	semd_t *temp = searchAdd(semAdd);
	if(temp -> s_next -> s_semAdd != semAdd){
		return NULL;
	}
	return (headProcQ(temp ->s_next -> s_procQ));
	
}


void 	initASL(){
	/* Initialize the semdFree list to contain all the elements of the array
	static semd t semdTable[MAXPROC]
	This method will be only called once during data structure initializa-
	tion. */
	semdFree_h = NULL;
	semdActive_h = NULL;

	static semd_t semdPool[MAXPROC+2];
	int i = 1;
	/*semdFree_h = &semdPool[1];
	semdFree_h -> s_next = NULL;*/
	
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
