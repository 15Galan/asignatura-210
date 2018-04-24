#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>

/**************************************************/

int main(){ 
	int pid_fork, pid_wait, status;

	pid_fork=fork(); // creamos un proceso hijo
  
	if(pid_fork<0) {
		perror("fork");
		geexit(-1);
	}

	if(pid_fork==0){ // proceso hijo
		int i=0;
		
		printf("Proceso hijo pid %d: ejecutando...\n",getpid());
		
		while(1) {
			sleep(1); printf("hijo: %d seg\n",++i); } // bucle infinito
		}
		
	else{ // padre
		sleep(5); // pausa de 5 segundos

		printf("\nProceso padre pid %d: mandando señal SIGINT\n",getpid());
		kill(pid_fork,SIGINT);

		pid_wait=wait(&status);  // esperando finalización del hijo

		// interpretacion de la finalizacion del hijo
		if(WIFEXITED(status)){
			printf("El proceso hijo pid %d termino con estato %d\n",pid_wait,WEXITSTATUS(status));
		}
		
		if(WIFSIGNALED(status)){
			printf("El proceso hijo pid %d termino por senal %d\n",pid_wait,WTERMSIG(status));
		}
	}
	
	exit(0);
}
