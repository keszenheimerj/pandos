/*sys1*/
HIDDEN void CREATEPROCESS(statePTR callerstate){
	state_PTR newState = s_a1;
	pcb_t tim = allocPCB();
	if(tim == NULL){
		s_v0 = -1;
		LDST();
	}
}

/*sys2*/
HIDDEN void TERMINATEPROCESS(){
	
}

/*sys3*/
HIDDEN void PROBEREN(sem_PTR sema4){
	sema4--;
	if(sema4<0){
		insertBlocked(&sema4, currentProc);
		scheduler();
	}else{
		LDST(sema4);
	}

/*sys4*/
HIDDEN void VERHOGEN((sem_PTR sema4){
	sema4++;
	if(sema4 <= 0){
		p = removeBlocked(&sema4);
		insertPRocQ(&readyQueue, p);
	}
	LDST(sema4);/*return to current proccess*/
}

/*sys6*/
HIDDEN void GETCPUTIME(){
	/*Bookeeping and management
		look for a call called "CPU Time" maybe...
		Write down time of day clock
		Do subtraction to figure out how much time was used
		Return time
		How much is there plus how much current time slice.
		use register s_v0
		*/
}

/*sys8*/
HIDDEN void WAITFORIODEVICE(special sema4){
	/*find which device
	test value
		insertBlocked
		scheduler();
		*/
}
