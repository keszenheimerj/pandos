#include "./../pcb.h";

/*2.4*/
HIDDEN	semd_t *semdActive_h, *semdFree_h;

*semd_h	semdFreeListPTR;
*semd_h	semdActiveListPTR; /*active sorted in acending order with 2 dummys on either side*/

semd_t	search(semd_t *child){
	/*loop return parent of node if there
	or parent of node if not there
	*/
	temp = *semdActive_h;
	while(temp -> s_semAdd < child -> s_semAdd){
		temp = temp -> s_next;
	}
	return temp -> s_prev;
}


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
	search active semd list for node with semd
		found
			insertProcQ(p, tailpointer found in semd
		notfound
			allocate a new semd from free list and init its fields and put a value in semAdd, init tailpointer with make empty proc and insert new node into active list
			have to find right location for insert because it is sorted then perform same opperation as if it was found
	
	*/
	semd_t temp = search(semAdd); 	/*find parent of semAdd. temp = parent of semAdd*/
	if(temp -> s_next == semAdd){ 	/*is semAdd already in the active list?*/
		return NULL; -------
	}else{
		semd_t copy = semdFree_h;
		semdFree_h = semdFree_h -> p_next;
		copy -> p_next = NULL
		copy -> p_prev = NULL

		s_semAdd -> s_next = *temp -> s_next 	/*semAdd's next becomes it's parent's next*/
		s_semAdd -> s_prev = *temp 				/*semAdd's prev points to it's parent*/
		*temp -> s_next = s_semAdd 				/*parent's next points to semAdd*/
	}




	if(emptyProcQ(semAdd -> s_procQ)){ /*testing inactive*/
		semdAdd -> s_procQ = mkEmptyProcQ();
	}




	if(emptyProc(*tp){
		*(tp) = p;
		p -> p_next = p;
		p -> p_prev = p;
		return;
	/* n queue case*/
		p  -> p_prev = tp;
		p  -> p_next = tp -> p_next;
		tp -> p_next = p;
		*(tp) = p;
		return;



	
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

	/*use search, which gives you the head and then you test of you have that child*/
	/*if childed seached is not there, search will give you the parent, so you will have to 
	check the parents next to see if the semAdd is actually there, if it is not there, return NULL.*/

	/*You need to get the head of s_procQ.*/

	/*return (headProc(search(child) -> s_ProcQ))*/

	/* one more test, if s_procQ is empty, return NULL*/
	semd_t temp = search(semAdd);
	if(temp -> s_next == semAdd){
		if(s_procQ == NULL){
		return (headProcQ(search(semAdd) -> s_procQ))
		}
	}else{
		return NULL;
	}
}
initASL(){
	/* Initialize the semdFree list to contain all the elements of the array
	static semd t semdTable[MAXPROC]
	This method will be only called once during data structure initializa-
	tion. */
	static semd_t semdPool[MAXPROC+2];
	for(int i = 1; i <= MAXPROC; i++){
		if(i == 1;){
			semdFreeListPTR = semdPool[i];
		}
		semdPool[i] -> s_procQ = mkEmptyProcQ();
		if(i==1){
			semdPool[i] -> s_prev = Null;
		}else{
			semdPool[i] -> s_prev = semdPool[i-1];
		}
		if(i==MAXPROC){
			semdPool[i] -> s_next= Null;
		}else{
			semdPool[i] -> s_next = semdPool[i+1];
		}
	}
	/*set up active*/
	semdActive_h = semdPool[0];
	semdACtive ->semdAdd =0;
	semdActive_h -> s_next = semdPool[21];
	semdActive_h -> s_next -> semdAdd = INF;
}
