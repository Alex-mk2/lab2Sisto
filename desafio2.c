#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int num_procesos, token_inicial, M;
    bool debug_flag;

    if (!parseo_argumentos(argc, argv,
                           &num_procesos,
                           &token_inicial,
                           &M,
                           &debug_flag)) {
        fprintf(stderr,
                "Uso: %s -p <num_procesos> -t <token_inicial> -M <max_decremento> [-D] [-h]\n",
                argv[0]);
        exit(EXIT_FAILURE);
    }
    int pipes[num_procesos][2];
    if (iniciar_pipes(num_procesos, pipes) < 0) {
        fprintf(stderr, "Error al inicializar los pipes\n");
        exit(EXIT_FAILURE);
    }
    int log_pipe[2];
    if (pipe(log_pipe) == -1) {
        perror("Error al crear log_pipe");
        exit(EXIT_FAILURE);
    }
    int id = iniciar_anillo_proc(num_procesos, pipes);

    if (id == -1){
        close(log_pipe[1]);

        //Inicio ronda
        iniciar_ronda(0, token_inicial, num_procesos, pipes);
        coordinar_juego(num_procesos,
                        pipes,
                        log_pipe[0],
                        token_inicial);

        //Cerrar pipes
        close(log_pipe[0]);
        for (int i = 0; i < num_procesos; ++i) {
            wait(NULL);
        }
    } else {
        close(log_pipe[0]);
        anillo_proc(id,token_inicial,M,num_procesos,pipes,debug_flag, log_pipe[1]);
        
    }

    return EXIT_SUCCESS;
}
