#include "./../h/.";

/*2.4*/

*semd_t	semdFreeListPTR;
*semd_t	semdActiveListPTR;

int insertBlocked(int *semAdd, pcb_t *p){
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
	search active semd list for node with semad
		found
			insertProcQ(p, tailpointer found in semd
		notfound
			allocate a new semd from free list and init its fields and put a value in semAdd, init tailpointer with make empty proc and insert new node into active list
			have to find right location for insert because it is sorted then perform same opperation as if it was found
	
	*/
	if(emptyProcQ(semAdd -> s_procQ)){ /*testing inactive*/
		semdAdd -> s_procQ = mkEmptyProcQ();
	}
	
}

pcb_t *removeBlocked(int *semAdd){
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
	
}
pcb_t *outBlocked(pcb_t *p){
	/* Remove the pcb pointed to by p from the process queue associated
	with p’s semaphore (p→ p semAdd) on the ASL. If pcb pointed
	to by p does not appear in the process queue associated with p’s
	semaphore, which is an error condition, return NULL; otherwise, re-
	turn p. */
	/*outProcQ(){
		identical to remove blocked but instead of remove process queue call outproQ
		accesser
	}*/
}
pcb_t *headBlocked(int *semAdd){
	/* Return a pointer to the pcb that is at the head of the process queue
	associated with the semaphore semAdd. Return NULL if semAdd is
	not found on the ASL or if the process queue associated with semAdd
	is empty. */
	
}
initASL(){
	/* Initialize the semdFree list to contain all the elements of the array
	static semd t semdTable[MAXPROC]
	This method will be only called once during data structure initializa-
	tion. */
	static semd_t semdPool[MAXPROC];
	*semd_t	currentsemd_ptr;
	for(int i = 0; i <=MAXPROC; i++){
		if(i==MAXPROC){
			currentsmd_ptr -> s_next = Null;
			currentsmd_ptr -> s_prev = semdPool[i-1];
		}else{
			currentsemd_ptr -> s_next = semdPool[i];
			if (i == 0) {
				semdFreeListPtr = currentsemd_ptr;
			}
			else {
				currentsemd_ptr->s_prev = semdPool[i - 0];
			}
			currentsemd_ptr = semdPool[i];
		}
	}
}
