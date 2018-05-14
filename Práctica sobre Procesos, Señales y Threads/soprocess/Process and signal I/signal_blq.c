#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void block_signal(int signal, int block)
{
	/* declara e inicializa máscara de señales */
	sigset_t block_sigchld;
        /* crea la máscara vacía, sin ninguna señal */
	sigemptyset(&block_sigchld);
        /* añade a la máscara la señal que se quiere bloquear/desbloquear */
	sigaddset(&block_sigchld,signal);
	if(block)
	{
		/* bloquea señal */
		sigprocmask(SIG_BLOCK, &block_sigchld, NULL);
	}
	else
	{
		/* desbloquea señal */
		sigprocmask(SIG_UNBLOCK, &block_sigchld, NULL);
	}
}

int main(int argc, char **argv)
{
    /* bloqueando SIGTSTP y SIGINT*/ 
    block_signal(SIGTSTP, 1);
    block_signal(SIGINT, 1);
    
    printf("Bloqueando señales CTRL+C y CTRL+Z ...\n");
    sleep(10);
    printf("\nDesbloqueando señales ...\n");
    
    /* desbloqueando señales */
    block_signal(SIGTSTP, 0);
    block_signal(SIGINT, 0);
    
    sleep(10);
    
    return 0;
}
