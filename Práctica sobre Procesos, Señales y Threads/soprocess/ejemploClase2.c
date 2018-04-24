#include <stdlib.h>		// Libreia estandar
#include <stdio.h>

int main(int argc, char *argv[], char *env[]){
	
	int i = 0;
	for(i; i < argc; i++){
		printf("Argumento %i : %argv[i]\n", i, argv[i]);
	}
	
	int j = 0;
	for(j; env[j] != NULL; j++){
		printf("Var %d : %s\n", j, env[j]);
	}
}
