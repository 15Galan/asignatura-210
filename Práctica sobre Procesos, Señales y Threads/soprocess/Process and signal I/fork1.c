#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
   pid_t pid_devuelto;
   int i;
   int n = 10;

   for (i = 0; i < n; i++)
   {
      pid_devuelto = fork();
      if (pid_devuelto != 0)
         break;
   } 
   printf("El padre del proceso %d es %d\n", getpid(), getppid());
return 0;
}
