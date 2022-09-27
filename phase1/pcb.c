/*H***************************************************************************************************
* FILENAME :	pcb.c
*
* DESCRIPTION :	
*	PcbQueues file management of queues of structures: pcb’s.
*
* PUBLIC FUNCTIONS : 
*	pcb_t	mkEmptyProcQ()		- This method is used to initialize a variable to be tail pointer to 
* 								a process queue. Return a pointer to the tail of an empty process 
* 			 					queue; i.e. NULL.
*	int	 	emptyProcQ()		- This method is used to initialize a variable to be tail pointer to 
* 								a process queue. Return a pointer to the tail of an empty process 
* 								queue; i.e. NULL.
*	void	freePcb()			- Insert element pointed to by p onto the pcbFree list.
*	pcb_t	allocPcb()			- Return Null if pcbFree list is empty or remove an element from the 
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

/*Functions*/
/*
* Function: This method is used to initialize a variable to be 
* tail pointer to a process queue. Return a pointer to the tail 
* of an empty process queue; i.e. NULL.
*/
pcb_t	*mkEmptyProcQ(){
	return NULL;
}

/*
* Function: Return TRUE if the queue whose tail is pointed 
* to by tp is empty. Return FALSE otherwise.
*/
int		emptyProcQ(pcb_t *tp){
	return (tp == NULL);
}

/*
* Function: Insert a pcb p onto the head of the pcbFree list.
*/
void	freePcb(pcb_t *p){	
	p -> p_prev = pcbFree_h;
	pcbFree_h = p;
}

/*
* Function: Initialize the pcbFree list.
*/
void	initPcbs(){	
	static	pcb_t	pool[MAXPROC];
	pcbFree_h = mkEmptyProcQ();
	int i=0;
	while(i< MAXPROC){
		freePcb(&(pool[i]));
		i++;
	}
}

/*
* Function: Return NULL if pcbFree list is empty or remove an 
* element from the list, provide initial values for ALL of 
* the pcbs field and return *pointer to removed element.
*/
pcb_t	*allocPcb(){		
	/*verify tere is a pcb to pull from the free list*/
	if(pcbFree_h == NULL){
		return NULL;
	}
	pcb_PTR	allocPCB = pcbFree_h;
	pcbFree_h = pcbFree_h -> p_prev;
	/*init all values to null*/
	allocPCB -> p_prev = NULL;
	allocPCB -> p_next = NULL;
	allocPCB -> p_prnt = NULL;
	allocPCB -> p_child = NULL;
	allocPCB -> p_sib = NULL;
	allocPCB -> p_sib_next = NULL;
	allocPCB -> p_semAdd = NULL;
	
	return allocPCB;
}

/*
* Function: Insert the pcb pointed to by p into the process queue 
* whose tail-pointer is pointed to by tp. Note the double 
* indirection through tp to allow for the possible updating of 
* the tail pointer as well.
*/
void 	insertProcQ(pcb_PTR *tp, pcb_t *p){
	/*empty queue case*/
	if(emptyProcQ(*tp)){
		p -> p_next = p -> p_prev = p;
		(*tp) = p;
		return;
	}
	pcb_PTR temp = *tp;
	temp->p_next -> p_prev = p;
	p->p_next = temp ->p_next;
	p->p_prev = temp;
	temp->p_next = p;
	(*tp) = p;
	return;
}

/*
* Function: Return a pointer to the first pcb from the process 
* queue whose tail is pointed to by tp. Do not remove this 
* pcbfrom the process queue. Return NULL if the process queue 
* is empty.
*/
pcb_t	*headProcQ(pcb_t *tp){
	if(emptyProcQ(tp)){
		return NULL;
	}
	return ((tp) -> p_next);	/*next is the head*/
}

/*
* Function: Remove the first (i.e. head) element from the 
* process queue whose tail-pointer is pointed to by tp. 
* Return NULL if the process queue was initially empty; 
* otherwise return the pointer to the removed element. 
* Update the process queue’s tail pointer if necessary.
*/
pcb_t 	*removeProcQ(pcb_PTR *tp){
	/*empty queue case*/
	pcb_PTR temp = *tp;
	if(emptyProcQ(*tp)){
		return NULL;
	}
	if(temp -> p_prev == (*tp)){
		*tp = NULL;
		return temp;
	}
	/*save head*/
	pcb_PTR head = headProcQ(temp);
	(head -> p_next) -> p_prev = *(tp);
	temp -> p_next = head -> p_next;
	
	return head;
}

/*
* Function: Remove the pcb pointed to by p from the process 
* queue whose tail pointer is pointed to by tp. Update the 
* process queue’s tail pointer if necessary. If the desired 
* entry is not in the indicated queue (an error condition), 
* return NULL; otherwise, return p. Note that p can point
* to any element of the process queue.
*/
pcb_t 	*outProcQ(pcb_PTR *tp, pcb_t *p){
	pcb_PTR prevProcQ = (*tp) ->p_prev;;
	/*verify processQ is not empty*/
	if(emptyProcQ(*tp)){
		return NULL;
	}
	/*verify the same process isnt removed twice, if processQ is only the tp remove it, updating tp when necessary*/
	if(*tp == headProcQ(*tp)){
		removeProcQ(tp);
	}
	if(*tp == p){
		pcb_PTR headP = headProcQ(*tp);
		prevProcQ -> p_prev -> p_prev = headP;
		headP -> p_prev = prevProcQ -> p_prev;
		return p;
	}
	/*find p in processQ*/
	/*allow the tail to be checked once and exit loop if hit again; hit again should never happen*/
	int hitTail = 0;
	while(hitTail < 2){
		if(prevProcQ -> p_prev == *tp){
			hitTail = hitTail + 1;
		}
		prevProcQ = prevProcQ -> p_prev;
		if(prevProcQ -> p_next -> p_prev == p){
			prevProcQ -> p_prev -> p_next = prevProcQ -> p_next;
			prevProcQ -> p_next -> p_prev = prevProcQ -> p_prev;
			return p;
		}
	}
	return NULL;
}

/*
* Function: Return TRUE if the pcb pointed to by p has no children. 
* Return FALSE otherwise. 
*/
int	emptyChild(pcb_t *p){
	/* */
	return (p -> p_child == NULL);
}

/*
* Function: Make the pcb pointed to by p a child of the pcb 
* pointed to by prnt.
*/
void	insertChild(pcb_t *prnt, pcb_t *p){
	/*prnt -> p_child = p;*/
	if(prnt -> p_child != NULL){
		p -> p_sib = prnt -> p_child;
	}
	p -> p_prnt = prnt;
	prnt -> p_child = p;
}

/*
* Function: Make the first child of the pcb pointed to by p 
* no longer a child of p. Return NULL if initially there were
* no children of p. Otherwise, return a pointer to this 
* removed first child pcb. 
*/
pcb_t	*removeChild(pcb_t *p){
	if(emptyChild(p)){
		return NULL;
	}
	pcb_PTR youngest = p -> p_child;
	if(youngest -> p_sib == youngest){ /*there is no sibling, only child.*/
		p -> p_child = NULL;
	}else{/*there is a sibiling*/
		p -> p_child = youngest -> p_sib;
	}
	youngest -> p_prnt = youngest -> p_sib = NULL;
	return youngest;
}

/*
* Function: Make the pcb pointed to by p no longer the child of its parent. If
	the pcb pointed to by p has no parent, return NULL; otherwise, return
	p. Note that the element pointed to by p need not be the first child of
	its parent.
*/
pcb_t	*outChild(pcb_t *p){
	/*test if parent exists*/
	if(p -> p_prnt == NULL){
		return NULL;
	}
	/*see if child is the first in the list of sibilings*/
	if(p -> p_prnt -> p_child == p){
		return removeChild(p);
	}
	pcb_PTR prev = p -> p_prnt -> p_child;
	/*loop through sibilings looking for p in which case remove*/
	while(prev -> p_sib != NULL){
		if(prev -> p_sib == p){
			prev -> p_sib = p -> p_sib;
			p -> p_prnt = NULL;
			return p;
		}
		prev = prev -> p_sib;
	}
	return NULL;
}
