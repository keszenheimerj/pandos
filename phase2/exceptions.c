
/*sys3*/
HIDDEN void proberen(sem_PTR sema4){
	sema4--;
	if(sema4<0){
		insertBlocked(&sema4, currentProc);
		scheduler();
	}else{
		LDST(sema4);
	}

/*sys4*/
HIDDEN void verhogen((sem_PTR sema4){
	sema4++;
	if(sema4 <= 0){
		p = removeBlocked(&sema4);
		insertPRocQ(&readyQueue, p);
	}
	LDST(sema4);/*return to current proccess*/
}

/*sys8*/
HIDDEN void waitForIoDevice(special sema4){
	/*find which device
	test value
		insertBlocked
		scheduler();
		*/
}
