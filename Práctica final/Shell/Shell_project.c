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
#include "string.h"         // Importar para poder usar la funcion "strcmp()".

#define MAX_LINE 256	    // 256 caracteres por linea (por comando), deberia ser suficiente.


job* tareas;    // Debe declararse global para que pueda usarse por el manejador.

// -----------------------------------------------------------------
///                          MANEJADOR
// -----------------------------------------------------------------

void manejador(int signal_id){      /// "signal(SEÑAL, manejador)" espera un "void manejador(int señal)".
    int pid_wait;                   // PID del proceso en espera.
    int status;                     // Estado devuelto por "wait()".
    int info;                       // Informacion procesada por "analyze_status()".
    enum status status_res;         // Estado procesado por "analyze_status()".
    job* trabajos = tareas->next;   // Puntero a "tareas" (el primer nodo contiene informacion, no es una tarea).

    while(trabajos != NULL) {
        pid_wait = waitpid(trabajos->pgid, &status, WUNTRACED | WNOHANG);   // Comprobar para cada proceso.

        status_res = analyze_status(status, &info);     // Estado procesado (Exited, Signaled o Suspended).

        if(pid_wait == trabajos->pgid) {
            if(status_res == EXITED || status_res == SIGNALED) {    // El proceso ha terminado o envio una señal.
                printf("\nEl proceso '%s' (pid: %i) ha finalizado.\n", trabajos->command, trabajos->pgid);

                job* auxiliar = trabajos->next; // Guardar temporalmente el puntero al siguiente proceso de la lista.
                delete_job(tareas, trabajos);   // Elimina el proceso "trabajos" de la lista "tareas".
                trabajos = auxiliar;            // Elimina el trabajo haciendo que apunte a donde auxiliar.

            }else{  // SUSPENDED.
                printf("\nEl proceso '%s' (pid: %i) se ha suspendido.\n", trabajos->command, trabajos->pgid);

                trabajos->state = STOPPED;  // Cambiar estado del proceso a STOPPED.
                trabajos = trabajos->next;  // Actualizar el trabajo (tarea) al siguiente.
            }

        }else{
            trabajos = trabajos->next;  // Actualizar el trabajo (tarea) al siguiente.
        }
    }
}


// ----------------------------------------------------------------
///                              MAIN
// ----------------------------------------------------------------

int main(void) {
    char inputBuffer[MAX_LINE];     // Buffer para el comando entrante.
    int background;                 // Igual a 1 si el comando acaba con '&'.
    char *args[MAX_LINE / 2];       // Linea de comando de maximo 128 (de 256) argumentos.

    /// Variables utiles:
    int pid_fork, pid_wait;     // PID para procesos creados (hijos) y en espera.
    int status;                 // Estado devuelto por "wait()".
    enum status status_res;     // Estado procesado por "analyze_status()".
    int info;                   // Informacion procesada por "analyze_status()".

    /// Variables utiles (mias):
    tareas = new_list("Lista de Tareas");

    ignore_terminal_signals();          // Ignorar señales del Terminal.
    signal(SIGCHLD, manejador);         // Manejar señales SIGCHLD con el "manejador".

    while (1) {        // Termina normalmente dentro de "get_command()" tras escribir "^D" ("Ctrl + D").
        printf("COMANDO -> ");  // Prompt (cabecera).
        fflush(stdout);

        get_command(inputBuffer, MAX_LINE, args, &background);    // Obtener siguiente comando.

        if (args[0] == NULL) continue;  // Si se introduce un comando "vacio" se vuelve a pedir.

        /// Comandos Internos ("cd", "jobs", "bg" y "fg").
        if(strcmp(args[0], "cd") == 0) {     // "strcmp" para comparar Strings.
            if(args[1] != NULL) {
                chdir(args[1]);         // Cambiar directorio a "args[1]".

            }else{
                chdir(getenv("HOME"));  // "getenv" para convertir un String en ruta.
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
            execvp(args[0], args);          // Ejecutar comando introducido o devuelve error.

            printf("ERROR: Comando '%s' no encontrado.\n\n", args[0]);

            exit(EXIT_FAILURE);     /// DUDA: ¿Por qué "EXIT_FAILURE"?

        } else {    // Proceso PADRE.
            new_process_group(pid_fork);    // Crear un grupo de procesos para el proceso padre.

            /// (3)  Si "background == 0", el proceso padre espera, sino continua.
            if (background == 0) {          // Proceso en FOREGROUND.
                set_terminal(pid_fork);                     // Asigna la Terminal al proceso hijo.
                pid_wait = waitpid(pid_fork, &status, WUNTRACED);
                set_terminal(getpid());                     // Asigna la Terminal al proceso en ejecucion.
                status_res = analyze_status(status, &info); // Estado procesado (Exited, Signaled o Suspended).

                job* proceso = new_job(pid_fork, args[0], FOREGROUND);  // Crear tarea.
                block_SIGCHLD();            // Bloquear señal SIGCHLD para acceder a una estructura de datos (lista).
                add_job(tareas, proceso);   // Añadir la tarea "proceso" a la lista "tareas".
                unblock_SIGCHLD();          // Desbloquear señal SIGCHLD al salir de una estructura de datos (lista).

                /// (4) La Shell muestra un mensaje del estado del comando ejecutado.
                if (info != 1) {
                    printf("\nPrimer Plano    pid: %i, comando: %s, %s, info: %i.\n\n",
                           pid_fork, args[0], status_strings[status_res], info);
                }

            } else {    // Proceso en BACKGROUND.
                job* proceso = new_job(pid_fork, args[0], BACKGROUND);  // Crear tarea.
                block_SIGCHLD();            // Bloquear señal SIGCHLD para acceder a una estructura de datos (lista).
                add_job(tareas, proceso);   // Añadir la tarea "proceso" a la lista "tareas".
                unblock_SIGCHLD();          // Desbloquear señal SIGCHLD al salir de una estructura de datos (lista).

                /// (4)  La Shell muestra un mensaje del estado del comando ejecutado.
                if (info != 1) {
                    printf("\nSegundo Plano    ejecutándose... pid: %i, comando: %s.\n\n", pid_fork, args[0]);
                }
            }

            /// (5)  El bucle vuelve a la funcion "get_commnad()".
        }

        // Fin del "while()".
    }
}
