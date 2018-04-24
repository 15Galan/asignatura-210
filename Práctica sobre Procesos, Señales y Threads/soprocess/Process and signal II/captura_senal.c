#include <stdio.h>
#include <signal.h>

/**************************************************/

void manejador(int senal){
	static int n = 0;
	printf("Se ha recibido la señal # %d (2=SIGINT=Ctrl+C) %d/3 \n",senal, ++n);
	
	if (n == 3){
		printf("Restableciendo comportamiento por defecto...\n");
		signal(SIGINT,SIG_DFL); // restablecer manejador por defecto
	}
}

/**************************************************/

int main(){
	// establecer manejador para tratamiento de la señal 2 (SIGINT, Ctrl+C)
	signal(SIGINT,manejador); 
 
	printf("Para terminar pulse varias veces Ctrl+C\n");

	while (1) ; // bucle infinito
}
