#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <wait.h>

/**************************************************/
void manejador_hijo(int senal){
	printf("El hijo ha recibido la sennal # %d y hace exit() \n",senal);
	exit(2);
}

int main(){
	int pid_fork, pid_wait, status;
	int i = 0;

	pid_fork=fork();	// creamos un proceso hijo.
	
	if(pid_fork<0){
		perror("fork");
		exit(-1);
	}

	if(pid_fork == 0){	// proceso hijo.
		signal(SIGINT,manejador_hijo); 
  
		printf("Proceso hijo pid %d: ejecutando...\n",getpid());
		
		while(i < 10){	// bucle de retardo.
			sleep(1); printf("hijo: %d seg\n",++i);
		}
	
	}else{	// padre.

		sleep(5);	// pausa de 5 segundos.
		
		printf("\nProceso padre pid %d: mandando sennal SIGINT\n",getpid());
		kill(pid_fork,SIGINT);
		
		while(i < 20){	// bucle de retardo.
			sleep(1); printf("padre: %d seg\n",++i);
		}
		
		exit(0);
	}
}
