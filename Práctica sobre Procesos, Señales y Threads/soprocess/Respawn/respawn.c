#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)
{
   pid_t pid;
   int valor;

   pid = fork();
   switch(pid)
   {
      case -1:   /* error del fork() */
         exit(-1);
      case 0:    /* proceso hijo */
         if (execvp(argv[1], &argv[1]) < 0)
         perror("exec");
      default:   /* padre */
         while (wait(&valor) != pid);

         if (valor == 0)
            printf("El mandato se ejecuto de formal normal\n");
         else
         {
            if (WIFEXITED(valor))
                 printf("El hijo termino normalmente y su valor devuelto fue %d\n", WEXITSTATUS(valor));

            if (WIFSIGNALED(valor))
                 printf("El hijo termino al recibir la señal %d\n", WTERMSIG(valor));
         }
    }
	return 0;
}
