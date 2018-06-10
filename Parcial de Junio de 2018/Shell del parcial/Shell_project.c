/**
UNIX Shell Project.

Sistemas Operativos.
Grados I. Informatica, Computadores & Software.
Dept. Arquitectura de Computadores - UMA.

Codigo adaptado de "Fundamentos de Sistemas Operativos", Silberschatz et al.

Para compilar y ejecutar el programa:
	$ gcc Shell_project.c job_control.c -o Shell
	$ ./Shell

	Escribir "^D" (CTRL+D) para finalizar.
*/

#include "job_control.h"	// RECORDAR: Compilar con el modulo "job_control.c".
#include "string.h"         // Importar para poder usar la funcion "strcmp()".

#define MAX_LINE 256	    // 256 caracteres por linea (por comando), deberia ser suficiente.

#define NEGRO    "\x1b[0m"
#define ROJO     "\x1b[31;1;1m"
#define VERDE    "\x1b[32;1;1m"
#define AZUL     "\x1b[34;1;1m"
#define CIAN     "\x1b[36;1;1m"
#define AMARILLO "\x1b[33;1;1m"
#define PURPURA  "\x1b[35;1;1m"

job* tareas;    // Debe declararse global para que pueda usarse por el manejador.
int contador_sig = 0;    // Contador para los procesos terminados con SIGNALED.

// -----------------------------------------------------------------
///                          MANEJADOR
// -----------------------------------------------------------------

void manejador(int signal_id){      /// "signal(SEÑAL, manejador)" espera un "void manejador(int señal)".
    int pid_wait;                   // PID del proceso en espera.
    int status;                     // Estado devuelto por "wait()".
    int info;                       // Informacion procesada por "analyze_status()".
    enum status status_res;         // Estado procesado por "analyze_status()".
    job* trabajos = tareas->next;   // Puntero a "tareas" (el primer nodo contiene informacion, no es una tarea).

    printf(PURPURA"\nOuch!\n"NEGRO);    /// (Obligatorio) Ejercicio 1.

    block_SIGCHLD();                // Se accede a la lista de tareas.

    while(trabajos != NULL) {
        pid_wait = waitpid(trabajos->pgid, &status, WUNTRACED | WNOHANG);   // Comprobar para cada proceso.

        status_res = analyze_status(status, &info);     // Estado procesado (Exited, Signaled o Suspended).

        if(pid_wait == trabajos->pgid) {    // Si el PID devuelto es el mismo, quiere decir que algo le paso al proceso.
            if(status_res != SUSPENDED) {       // El proceso ha terminado o ha enviado una señal.

                /// (Obligatorio) Ejercicio 3.
                if(status_res == SIGNALED){     // Si terminó con una señal, se incrementa el contador.
                    contador_sig++;
                }

                printf(AZUL"\nSegundo Plano (finalizó)\n    pid: %i, comando: %s.\n"NEGRO,
                       trabajos->pgid, trabajos->command);

                job* auxiliar = trabajos->next; // Guardar temporalmente el puntero al siguiente proceso de la lista.
                delete_job(tareas, trabajos);   // Elimina el proceso "trabajos" de la lista "tareas".
                trabajos = auxiliar;            // Actualizar el trabajo (tarea) al siguiente, usando "auxiliar".

            }else{  // status_res == SUSPENDED.
                printf(AZUL"\nSegundo Plano (se suspendió)\n    pid: %i, comando: %s.\n"NEGRO,
                       trabajos->pgid, trabajos->command);

                trabajos->state = STOPPED;  // Cambiar estado del proceso a STOPPED.

                printf(PURPURA"\n¡Quiero aprobar!\n"NEGRO);     /// (Obligatorio) Ejercicio 2.

                trabajos = trabajos->next;  // Actualizar el trabajo (tarea) al siguiente.
            }

        }else{
            trabajos = trabajos->next;  // Actualizar el trabajo (tarea) al siguiente.
        }
    }

    unblock_SIGCHLD();      // Se sale de la lista de tareas.
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
    tareas = new_list("Lista de Tareas");   // Inicializacion de la lista de tareas.

    ignore_terminal_signals();      // Ignorar señales del Terminal.
    signal(SIGCHLD, manejador);     // Manejar señales SIGCHLD con el "manejador".

    while (1) {        // Termina normalmente dentro de "get_command()" tras escribir "^D" ("Ctrl + D").
        printf("COMANDO -> ");  // Prompt (cabecera).
        fflush(stdout);

        get_command(inputBuffer, MAX_LINE, args, &background);    // Obtener siguiente comando.

        if (args[0] == NULL) continue;  // Si se introduce un comando "vacio" se vuelve a pedir.

        /// Comandos Internos ("cd", "jobs", "fg", "bg" y "sig").
        if(strcmp(args[0], "cd") == 0) {        // "strcmp" para comparar Strings.
            if(args[1] != NULL) {
                if(chdir(args[1]) == -1) {      // Cambiar directorio a "args[1]" si lo encuentra, si falla devuelve "-1".
                    printf(ROJO"\nNo se puede cambiar a la ruta '%s'.\n\n"NEGRO, args[1]);
                }

            }else{
                chdir(getenv("HOME"));  // "getenv" para convertir un String en ruta.
            }

            continue;   // La Shell debe recibir otro comando tras finalizar la ejecucion de un comando interno.
        }

        if(strcmp(args[0], "jobs") == 0) {      // "strcmp" para comparar Strings.
            if(list_size(tareas) != 0) {
                print_job_list(tareas);         // Si la lista no esta vacia (0 elementos), la imprime.

            }else{
                printf(VERDE"\nLista vacía\n    No hay procesos que mostrar.\n\n"NEGRO); // Si no, indica que esta vacia.
            }

            continue;   // La Shell debe recibir otro comando tras finalizar la ejecucion de un comando interno.
        }

        if(strcmp(args[0], "fg") == 0) {
            job* auxiliar;              // Tratar el proceso (nodo de la lista).
            int id;                     // Posicion del proceso en la lista.

            if(args[1] != NULL) {   // Se introdujo "fg id".
                id = atoi(args[1]); // Usar "atoi()" para convertir "args[1]" (char) en "int".

            }else{                  // Se introdujo "fg   ".
                id = 1;             // Por defecto, si no se indica el proceso, traera a primer plano el proceso 1.
            }

            auxiliar = get_item_bypos(tareas, id);  // Se obtiene el proceso indicado por el "id" en la lista "tareas".

            if(auxiliar == NULL){
                printf(ROJO"\nLista vacía\n    No hay procesos que pasar a Primer Plano.\n\n"NEGRO);

            }else{
                if(auxiliar->state != FOREGROUND) {   // Actuar solo cuando la tarea esta en segundo plano o suspendida.
                    auxiliar->state = FOREGROUND;       // Cambiar de BACKGROUND a FOREGROUND para ponerlo en primer plano.

                    printf(AMARILLO"\nPasó a Primer Plano\n    [%i] pid: %i, comando: %s.\n\n"NEGRO,
                           id, auxiliar->pgid, auxiliar->command);

                    set_terminal(auxiliar->pgid);       // Asigna el Terminal al proceso indicado.
                    killpg(auxiliar->pgid, SIGCONT);    // Envia una señal al grupo de procesos para que estos continuen.
                    waitpid(auxiliar->pgid, &status, WUNTRACED);
                    set_terminal(getpid());             // Asigna el Terminal al proceso en ejecucion.

                    status_res = analyze_status(status, &info);     // Saber el estado para saber si finalizo la ejecucion.

                    if (status_res != SUSPENDED) {      // Si el estado no es SUSPENDED, el proceso termino su ejecucion.

                        /// (Obligatorio) Ejercicio 3.
                        if(status_res == SIGNALED){     // Si terminó con una señal, se incrementa el contador.
                    		contador_sig++;
                		}

                        printf(AMARILLO"\nPrimer Plano (finalizó)\n    [%i] pid: %i, comando: %s.\n\n"NEGRO,
                               id, auxiliar->pgid, auxiliar->command);

                        delete_job(tareas, auxiliar);   // Se elimina el proceso de la lista "tareas".

                    } else {                            // Si el estado es SUSPENDED, el proceso esta detenido.
                        printf(AMARILLO"\nPrimer Plano (se suspendió)\n    [%i] pid: %i, comando: %s.\n\n"NEGRO,
                               id, auxiliar->pgid, auxiliar->command);

                        auxiliar->state = STOPPED;      // Se indica que el proceso esta STOPPED (detenido) en su estado.
                    }

                }else{
                    printf(AMARILLO"El proceso '%s' (pid: %i) no estaba en Segundo Plano o Suspendido."NEGRO,
                            auxiliar->command, auxiliar->pgid);
                }
            }

            continue;   // La Shell debe recibir otro comando tras finalizar la ejecucion de un comando interno.
        }

        if(strcmp(args[0], "bg") == 0) {
            job* auxiliar;              // Tratar el proceso (nodo de la lista).
            int id;                     // Posicion del proceso en la lista.

            if(args[1] != NULL) {   // Se introdujo "bg id".
                id = atoi(args[1]); // Usar "atoi()" para convertir "args[1]" (char) en "int".

            }else{                  // Se introdujo "bg   ".
                id = 1;             // Por defecto, si no se indica el proceso, traera a primer plano el proceso 1.
            }

            auxiliar = get_item_bypos(tareas, id);  // Se obtiene el proceso indicado por el "id" en la lista "tareas".

            if(auxiliar == NULL) {
                printf(ROJO"\nLista vacía\n    No hay procesos que pasar a Segundo Plano.\n\n"NEGRO);

            }else{
                auxiliar->state = BACKGROUND;       // Cambiar de FOREGROUND a BACKGROUND para ponerlo en segundo plano.

                killpg(auxiliar->pgid, SIGCONT);    // Envia una señal al grupo de procesos para que estos continuen.

                printf(AMARILLO"\nPasó a Segundo Plano (ejecutándose...)\n    [%i] pid: %i, comando: %s.\n\n"NEGRO,
                       id, auxiliar->pgid, auxiliar->command);

            }

            continue;   // La Shell debe recibir otro comando tras finalizar la ejecucion de un comando interno.
        }

        if(strcmp(args[0], "sig") == 0) {
            if(args[1] == NULL) {   // Se introdujo "sig".
                printf(PURPURA"\nEl número de tareas que terminaron por recibir una señal es %i.\n\n"NEGRO,
                contador_sig);

            }else{                  // Se introdujo "sig <algo>".
                printf(ROJO"\nEl comando 'sig' no requiere argumentos.\n\n"NEGRO);
            }

            continue;   // La Shell debe recibir otro comando tras finalizar la ejecucion de un comando interno.
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

            printf(ROJO"\nComando '%s' no encontrado.\n\n"NEGRO, args[0]);

            exit(EXIT_FAILURE);

        } else {    // Proceso PADRE.
            new_process_group(pid_fork);    // Crear un grupo de procesos para el proceso padre.

            /// (3)  Si "background == 0", el proceso padre espera, sino continua.
            if (background == 0) {          // Proceso en FOREGROUND.
                set_terminal(pid_fork);                     // Asigna el Terminal al proceso hijo.
                pid_wait = waitpid(pid_fork, &status, WUNTRACED);
                set_terminal(getpid());                     // Asigna el Terminal al proceso en ejecucion.
                status_res = analyze_status(status, &info); // Estado procesado (Exited, Signaled o Suspended).

                if(status_res == SUSPENDED) {   // Si la tarea en primer plano se suspende, debe almacenarse detenida.
                    job *proceso = new_job(pid_fork, args[0], STOPPED);  // Crear tarea con estado STOPPED (detenida).
                    block_SIGCHLD();            // Bloquear señal SIGCHLD para acceder a una estructura de datos (lista).
                    add_job(tareas, proceso);   // Añadir la tarea "proceso" a la lista "tareas".
                    unblock_SIGCHLD();          // Desbloquear señal SIGCHLD al salir de una estructura de datos (lista).
                }

                /// (Obligatorio) Ejercicio 3.
                if(status_res == SIGNALED){     // Si terminó con una señal, se incrementa el contador.
                    contador_sig++;
                }

                /// (4) La Shell muestra un mensaje del estado del comando ejecutado.
                //if (info != 1) {
                printf("\nPrimer Plano\n    pid: %i, comando: %s, %s, info: %i.\n\n",
                       pid_fork, args[0], status_strings[status_res], info);
                //}

            } else {    // Proceso en BACKGROUND.
                job* proceso = new_job(pid_fork, args[0], BACKGROUND);  // Crear tarea con estado BACKGROUND.
                block_SIGCHLD();            // Bloquear señal SIGCHLD para acceder a una estructura de datos (lista).
                add_job(tareas, proceso);   // Añadir la tarea "proceso" a la lista "tareas".
                unblock_SIGCHLD();          // Desbloquear señal SIGCHLD al salir de una estructura de datos (lista).

                /// (4)  La Shell muestra un mensaje del estado del comando ejecutado.
                //if (info != 1) {
                printf("\nSegundo Plano    (ejecutándose...)\n    pid: %i, comando: %s.\n\n", pid_fork, args[0]);
                //}
            }

            /// (5)  El bucle vuelve a la funcion "get_commnad()".
        }

        // Fin del "while()".
    }
}
