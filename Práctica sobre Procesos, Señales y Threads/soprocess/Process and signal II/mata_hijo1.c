#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>

/**************************************************/

void manejadorHijo(int senal){
	printf("Hijo: He recibido la señal #%d.\n", senal);
	
	sleep(2);
	
	exit(10);
}

int main(){ 
	int pid_fork, pid_wait, status;

	pid_fork = fork();	// creamos un proceso hijo
  
	if(pid_fork < 0){
		perror("fork");
		exit(-1);
	}

	if(pid_fork == 0){	// proceso hijo
		int i = 0;
		
		printf("Proceso HIJO (pid %d): ejecutando...\n",getpid());
		
		while(1){	// bucle infinito
			
			signal(SIGINT, manejadorHijo);
			
			sleep(1);
			printf("Hijo: %d seg.\n",++i);
		}
	
	}else{ // padre
		sleep(5);	// pausa de 5 segundos

		printf("\nProceso PADRE (pid %d): mandando señal SIGINT (#2).\n",getpid());
		kill(pid_fork,SIGINT);

		pid_wait = wait(&status);  // esperando finalización del hijo

		// interpretacion de la finalizacion del hijo
		if(WIFEXITED(status)){
			printf("El proceso HIJO (pid %d) terminó con estado %d.\n",pid_wait,WEXITSTATUS(status));
		}
		
		if(WIFSIGNALED(status)){
			printf("El proceso HIJO (pid %d) terminó por senal %d.\n",pid_wait,WTERMSIG(status));
		}
	}
	
	exit(0);
}
