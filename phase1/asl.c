/*H***************************************************************************************************
* FILENAME :	asl.c
*
* DESCRIPTION :	
*	The Active Semaphore list management of structures: semd_t.
*
* PUBLIC FUNCTIONS : 
*	semd_t	searchAdd()			- Loop return parent of node if there or parent of node if not there.
*	semd_t	search()			- ...
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

/*2.4*/
HIDDEN	semd_t *semdActive_h, *semdFree_h;

semd_t*	searchAdd(int* checkVal){
	/*loop return parent of node if there
	or parent of node if not there*/
	semd_t *temp = semdActive_h;
	while((temp -> s_next -> s_semAdd) < checkVal){
		temp = temp -> s_next;
	}
	return temp;
}


semd_t* search(semd_t *child){
	
	return search(child);
}

int 	insertBlocked(int *semAdd, pcb_t *p){
	/* Insert the pcb pointed to by p at the tail of the process queue as-
	sociated with the semaphore whose physical address is semAdd and
	set the semaphore address of p to semAdd. If the semaphore is cur-
	rently not active (i.e. there is no descriptor for it in the ASL), allocate
	a new descriptor from the semdFree list, insert it in the ASL (at the
	appropriate position), initialize all of the fields (i.e. set s semAdd
	to semAdd, and s procq to mkEmptyProcQ()), and proceed as
	above. If a new semaphore descriptor needs to be allocated and the
	semdFree list is empty, return TRUE. In all other cases return FALSE.
	*/
	/*
	search active semd list for node with semd
		found
			insertProcQ(p), tailpointer found in semd
		notfound
			allocate a new semd from free list and init its fields and put a value in semAdd, init tailpointer with make empty proc and insert new node into active list
			have to find right location for insert because it is sorted then perform same opperation as if it was found*/
	
	semd_t *insertP = searchAdd(semAdd); 	/*find parent of semAdd. temp = parent of semAdd*/
	if((insertP -> s_next -> s_semAdd) == semAdd){ 	/*is semAdd already in the active list? If Found*/
		insertProcQ(&(insertP -> s_next) -> s_procQ, p); 
	}else{	
		if(semdFree_h == NULL){
			return 1;
		}					/*semAdd is not in the active list. Not Found*/
		semd_t *copy = semdFree_h;
		semdFree_h = semdFree_h -> s_next;
		
		/*init fields*/
		
		copy -> s_next = insertP -> s_next;
		(insertP) -> s_next = copy;		
		copy -> s_semAdd = semAdd;
		copy -> s_procQ = mkEmptyProcQ();
		insertProcQ(&copy -> s_procQ, p);
	}
	return 0;
}

pcb_t 	*removeBlocked(int *semAdd){
	/* Search the ASL for a descriptor of this semaphore. If none is
	found, return NULL; otherwise, remove the first (i.e. head) pcb from
	the process queue of the found semaphore descriptor and return a
	pointer to it. If the process queue for this semaphore becomes empty
	(emptyProcQ(s procq) is TRUE), remove the semaphore de-
	scriptor from the ASL and return it to the semdFree list. */
	/*removeProcQ(){
		search active sem list for matching semAdd
		case
			not found
				errorcase
			found
				perform a remove procQ on processQ associated with node just found in active sem list
				this is value returned
				process queue is not empty after removal ---done
				process queue is empty--deallocate and take node out of active list and put back on free list
	}*/
	semd_t *parent = searchAdd(semAdd);
	semd_t *semAsemd = parent -> s_next;
	if(semAsemd -> s_semAdd != semAdd){
		/*error case*/
		return NULL;
	}
	pcb_t *outP = removeProcQ(&(semAsemd) -> s_procQ);
	if(emptyProcQ(semAsemd -> s_procQ)){/*if list is empty we have to move semd to free list*/
		/*remove from active*/
		parent -> s_next = parent -> s_next -> s_next;
		/*add to free*/
		semAsemd -> s_next = semdFree_h;
		semdFree_h = semAsemd;
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
	semd_t *parent = search(semAdd);
	semd_t *semAsemd = parent -> next;
	if(semAsemd -> s_semAdd != semAdd){
		/*error case*/
		return NULL;
	}
	pcb_t *outP = outProcQ(semAsemd -> s_procQ);
	if(emptyProcQ(semAsemd -> s_procQ)){/*if list is empty we have to move semd to free list*/
		/*remove from active*/
		parent -> s_next = parent -> s_next -> s_next;
		/*add to free*/
		semAsemd -> semdFree_h;
		semdFree_h = semdAsemd;
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
	semd_t temp = search(semAdd);
	if(temp -> s_next == semAdd && s_procQ == NULL){
		return (headProcQ(temp -> s_procQ));
	}

	return NULL;
}
void 	initASL(){
	/* Initialize the semdFree list to contain all the elements of the array
	static semd t semdTable[MAXPROC]
	This method will be only called once during data structure initializa-
	tion. */
	static semd_t semdPool[MAXPROC+2];
	semdFree_h = semdPool[1];
	for(int i = 1; i <= MAXPROC; i++){
		semdPool[i] -> s_procQ = mkEmptyProcQ();
		if(i==MAXPROC){
			semdPool[i] -> s_next= NULL;
		}else{
			semdPool[i] -> s_next = semdPool[i+1];
		}
	}
	/*set up active*/
	semdActive_h = semdPool[0];
	semdACtive ->semdAdd = 0;
	semdActive_h -> s_next = semdPool[21];
	semdActive_h -> s_next -> semdAdd = INF;
}
