/*H***************************************************************************************************
* FILENAME :	pcb.c
*
* DESCRIPTION :	
*	PcbQueues file management of queues of structures: pcb’s.
*
* PUBLIC FUNCTIONS : 
*	void	freePcb(pcb_t *p)	- Insert element pointed to by p onto the pcbFree list.
*	pcb_t	*allocPcb()			- Return Null if pcbFree list is empty or remove an element from the 
*								list, provide initial values for ALL of the pcbs field and return 
*								pointer to removed element.
*	void	initPcbs()			- Initialize the pcbFree list.
*	int 	emptyChild()		- Return TRUE if the pcb pointed to by p has no children. Return
*								FALSE otherwise.
*	void 	insertChild()		- Make the pcb pointed to by p a child of the pcb pointed to by prnt.
*	pcb_t 	removeChild()		- Make the first child of the pcb pointed to by p no longer a child of
*								p. Return NULL if initially there were no children of p. Otherwise,
*								return a pointer to this removed first child pcb.
*	pcb_t 	outChild()			- Make the pcb pointed to by p no longer the child of its parent. If
*								the pcb pointed to by p has no parent, return NULL; otherwise, return
*								p. Note that the element pointed to by p need not be the first child of
*								its parent.
* AUTHORS :	James Keszenheimer, Evan Hanson		START DATE : 31 Aug 22
*
*H*/

#include "../h/pcb.h"
#include "../h/const.h"
#include "../h/types.h"

HIDDEN pcb_t	*pcbFree_h;

pcb_t	*mkEmptyProcQ(){
	/* This method is used to initialize a variable to be tail pointer to a
	process queue.
	Return a pointer to the tail of an empty process queue; i.e. NULL. */
	return NULL;
}

int		emptyProcQ(pcb_t *tp){
	/* Return TRUE if the queue whose tail is pointed to by tp is empty.
	Return FALSE otherwise. */
	return (tp == NULL);
}

int		singleProcQ(pcb_t *tp){
	/* Return TRUE if the queue has one element.
	Return FALSE otherwise. */
	return (tp->p_next == tp);
}

void	freePcb(pcb_t *p){	
	/*inset element pointed to by p onto the pcbFree list*/
	/*if(pcbFree_h != NULL){
		p -> p_next = pcbFree_h -> p_next;
		p -> p_prev = pcbFree_h;
		pcbFree_h -> p_next -> p_prev = p;
		pcbFree_h -> p_next = p;
	}
	pcbFree_h = p;*/
	/*pcbFree_h ->p_prev -> p_next = p;
	p -> p_prev = pcbFree_h -> p_prev;
	p -> p_next = pcbFree_h;
	pcbFree_h -> p_prev = p;
	(*pcbFree_h) = *p;*/
	p -> p_prev = pcbFree_h;
	pcbFree_h = p;
}

void	initPcbs(){	
	/*initialize the pcbFree list*/
	static	pcb_t	pool[MAXPROC];
	pcbFree_h = mkEmptyProcQ();
	int i=0;
	while(i< MAXPROC){
		freePcb(&(pool[i]));
		i++;
	}
}

pcb_t	*allocPcb(){		
	/*return NULL if pcbFree list is empty or remove an element from the 
	* list, provide initial values for ALL of the pcbs field and return 
	*pointer to removed element
	*/
	if(pcbFree_h == NULL){
		return NULL;
	}
	
	
	pcb_PTR	allocPCB = pcbFree_h;
	
	pcbFree_h = pcbFree_h -> p_prev;
	allocPCB -> p_prev = NULL;
	allocPCB -> p_next = NULL;
	allocPCB -> p_prnt = NULL;
	allocPCB -> p_child = NULL;
	allocPCB -> p_sib = NULL;
	allocPCB -> p_sib_next = NULL;
	allocPCB -> p_semAdd = NULL;
	/*pcbFree_h -> p_next = pcbFree_h -> p_prev;
	pcbFree_h -> p_prev -> p_next = pcbFree_h -> p_next;
	(*pcbFree_h) = *pcbFree_h -> p_next;
	allocPCB -> p_next = allocPCB -> p_prev = NULL;*/
	/*
	pcbFree_h = (pcbFree_h -> p_next);
	pcbFree_h -> p_prev = allocPCB -> p_next;
	allocPCB -> p_next -> p_prev = pcbFree_h;*/
	/*if(singleProcQ(pcbFree_h)){
		pcbFree_h = NULL;
	}*/
	return allocPCB;
}

void 	insertProcQ(pcb_PTR *tp, pcb_t *p){
	/* Insert the pcb pointed to by p into the process queue whose tail-
	pointer is pointed to by tp. Note the double indirection through tp
	to allow for the possible updating of the tail pointer as well. */
	/* empty queue case*/
	if(emptyProcQ(*tp)){
		
		p -> p_next = p -> p_prev = p;
		(*tp) = p;
		return;
	}
	pcb_PTR temp = *tp;
	(*tp) = p;
	temp->p_next -> p_prev = p;
	p->p_next = temp ->p_next;
	p->p_prev = temp;
	temp->p_next = p;
	/* n queue case*/
	/*p  -> p_prev = temp;
	p  -> p_next = temp -> p_next;
	temp -> p_next = p;
	p -> p_next -> p_prev = p;*/
	
	return;
}




pcb_t	*headProcQ(pcb_PTR *tp){
	/* Return a pointer to the first pcb from the process queue whose tail
	is pointed to by tp. Do not remove this pcbfrom the process queue.
	Return NULL if the process queue is empty. */
	if(emptyProcQ(*tp)){
		return NULL;
	}
	return ((*tp) -> p_next);	/*next is the head*/
}







pcb_t 	*removeProcQ(pcb_PTR *tp){
	/* Remove the first (i.e. head) element from the process queue whose
	tail-pointer is pointed to by tp. Return NULL if the process queue
	was initially empty; otherwise return the pointer to the removed ele-
	ment. Update the process queue’s tail pointer if necessary. */
	/* empty queue case*/
	pcb_PTR temp = *tp;
	if(emptyProcQ(*tp)){
		return NULL;
	}
	if(temp -> p_prev == (*tp)){
		*tp = NULL;
		return temp;
	}
	/*save head*/
	pcb_PTR head = headProcQ(&temp);
	(head -> p_next) -> p_prev = *(tp);
	temp -> p_next = head -> p_next;
	
	/*head -> p_next = head -> p_prev = NULL;*/
	/*if head was the last element we make the tailPointer null*/
	
	return head;
}

pcb_t 	*outProcQ(pcb_PTR *tp, pcb_t *p){
	/* Remove the pcb pointed to by p from the process queue whose tail-
	pointer is pointed to by tp. Update the process queue’s tail pointer if
	necessary. If the desired entry is not in the indicated queue (an error
	condition), return NULL; otherwise, return p. Note that p can point
	to any element of the process queue. */
	pcb_PTR curN = (*tp) -> p_prev;
	
	while(*tp != curN && curN != p){
		curN = curN -> p_prev;
	}
	if(curN ==p ){
		if(*(tp) == p){
			*tp = (*tp) -> p_prev;
		}
		p -> p_next -> p_prev = p -> p_prev;
		p -> p_prev -> p_next = p -> p_next;
		/*p -> p_next = p -> p_prev = NULL;*/
		return p;
	}else{
		return NULL;
	}
	
}

/******************START*OF*GLOBAL*FUNCTIONS*************************************/
/*pcb_PTR	freePCBList[MAXPROC];*/



				


int		emptyChild(pcb_t *p){
	/* Return TRUE if the pcb pointed to by p has no children. Return
	FALSE otherwise. */
	return (p -> p_child == NULL);
}
	
void	insertChild(pcb_t *prnt, pcb_t *p){
	/* Make the pcb pointed to by p a child of the pcb pointed to by prnt.*/
	prnt -> p_child = p;
}

pcb_t	*removeChild(pcb_t *p){
	/* Make the first child of the pcb pointed to by p no longer a child of
	p. Return NULL if initially there were no children of p. Otherwise,
	return a pointer to this removed first child pcb. */
	if(emptyChild(p)){
		return NULL;
	}
	pcb_t *youngest = p -> p_child;
	if(youngest -> p_sib == youngest){ /*there is no sibling, only child.*/
		p -> p_child = NULL;
	}else{/*there is a sibiling*/
		p -> p_child = youngest -> p_sib;
		youngest -> p_sib_next -> p_sib = youngest -> p_sib;
		youngest -> p_sib -> p_sib_next = youngest -> p_sib_next;
	}
	youngest -> p_prnt = youngest -> p_sib = youngest -> p_sib_next = NULL;
	return youngest;
}

pcb_t	*outChild(pcb_t *p){
	/* Make the pcb pointed to by p no longer the child of its parent. If
	the pcb pointed to by p has no parent, return NULL; otherwise, return
	p. Note that the element pointed to by p need not be the first child of
	its parent. */
	if(p -> p_prnt == NULL){
		return NULL;
	}
	if(p -> p_prnt -> p_child == p){
		return removeChild(p);
	}
	if(p -> p_sib == p){
		p -> p_prnt -> p_child = NULL;
		return p;
	}
	p -> p_prnt -> p_child = p -> p_sib;
	p -> p_sib_next -> p_sib = p -> p_sib;
	p -> p_sib -> p_sib_next = p -> p_sib_next;
	return p;
	
}


/*******************END*OF*GLOBAL*FUNCTIONS*************************************/

