#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    /* ignorando SIGTSTP y SIGINT*/ 
    signal(SIGTSTP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    
    printf("Ignorando señales CTRL+C y CTRL+Z ...\n");
    sleep(10);
    printf("\nVolviendo señales a su comportamiento por defecto ...\n");
    
    /* restaurando su comportamiento por defecto */
    signal(SIGTSTP, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    
    sleep(10);
    
    return 0;
}
