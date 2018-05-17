/**
UNIX Shell Project.

Sistemas Operativos.
Grados I. Informatica, Computadores & Software.
Dept. Arquitectura de Computadores - UMA.

Codigo adaptado de "Fundamentos de Sistemas Operativos", Silberschatz et al.

Para compilar y ejecutar el programa:
	$ gcc Shell_project.c job_control.c -o Shell
	$ ./Shell

	(escribir "^D" para finalizar)
*/

#include "job_control.h"	// RECORDAR: Compilar con el modulo "job_control.c".
#include "string.h"     // Importar para poder usar la funcion "strcmp()".

#define MAX_LINE 256	// 256 caracteres por linea (por comando), deberia ser suficiente.

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------

int main(void) {
    char inputBuffer[MAX_LINE];     // Buffer para el comando entrante.
    int background;                 // Igual a 1 si el comando acaba con '&'.
    char *args[MAX_LINE / 2];       // Linea de comando de maximo 128 (de 256) argumentos.

    // Variables utiles:
    int pid_fork, pid_wait;     // pid para procesos creados y en espera.
    int status;                 // Estado devuelto por "wait()".
    enum status status_res;     // Estado procesado por "analyze_status()".
    int info;                   // Info procesada por "analyze_status()".

    // Variables utiles (mias):
    int estado;

    ignore_terminal_signals();  // Ignorar señales del Terminal.

    while (1) {        // Termina normalmente dentro de "get_command()" tras escribir "^D" ("Ctrl + D").
        printf("COMMAND -> ");  // Prompt (cabecera).
        fflush(stdout);

        get_command(inputBuffer, MAX_LINE, args, &background);    // Obtener siguiente comando.

        if (args[0] == NULL) continue;  // Si se introduce un comando "vacio" se vuelve a pedir.

        if(strcmp(args[0], "cd") == 0) {     // "strcmp" para comparar Strings.
            if(args[1] != NULL) {
                chdir(args[1]);     // Cambiar directorio a "args[1]".

            }else{
                chdir(getenv("HOME"));  // "getenv" para convertir "HOME" en la ruta a "/home".
            }

            estado = analyze_status(status,&info);

            if (info != 1){
                printf("\nForeground    pid: %i, command: %s, %s, info: %i.\n\n",
                       getpid(), args[0], status_strings[estado], info);
            }

            continue;
        }

        /// PASOS:
        /// (1)  Crear un proceso hijo usando "fork()".
        pid_fork = fork();

        if (pid_fork == -1) {   // Necesario comprobar que el hijo se cree correctamente.
            printf("Fallo en fork().\n\n");
        }

        /// (2)  EL proceso hijo invoca "execvp()".
        if (pid_fork == 0) {    // Proceso HIJO.
            new_process_group(getpid());    // Crear un grupo de procesos para el proceso hijo.

            restore_terminal_signals();     // Restaurar señales del Terminal.

            execvp(args[0], args);  // Ejecutar comando introducido o devuelve error.

            printf("ERROR: Command '%s' not found.\n\n", args[0]);

            exit(EXIT_FAILURE);

        } else {    // Proceso PADRE.
            new_process_group(pid_fork);    // Crear un grupo de procesos para el proceso padre.

            /// (3)  Si "background == 0", el proceso padre espera, sino continua.
            if (background == 0) {          // Proceso en FOREGROUND.
                set_terminal(pid_fork);     // Asigna la Terminal al proceso hijo.

                pid_wait = waitpid(pid_fork, &status, WUNTRACED);

                set_terminal(getpid());     // Asigna la Terminal al proceso en ejecucion.

                estado = analyze_status(status, &info);

                /// (4) La Shell muestra un mensaje del estado del comando ejecutado.
                if (info != 1) {
                    printf("\nForeground    pid: %i, command: %s, %s, info: %i.\n\n",
                           pid_fork, args[0], status_strings[estado], info);
                }

            } else {    // Proceso en BACKGROUND.
                /// (4)  La Shell muestra un mensaje del estado del comando ejecutado.
                if (info != 1) {
                    printf("\nBackground    job running... pid: %i, %s.\n\n", pid_fork, args[0]);
                }
            }

            /// (5)  El bucle vuelve a la funcion "get_commnad()".
        }

        // Fin del "while()".
    }
}
