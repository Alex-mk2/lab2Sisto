#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


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
    int id = iniciar_anillo_proc(num_procesos, pipes);
    if (id == -1) {
        iniciar_ronda(0, token_inicial, num_procesos, pipes);
        leer_ganador(num_procesos, pipes, token_inicial);
        for (int i = 0; i < num_procesos; ++i) {
            wait(NULL);
        }
    } else {
        anillo_proc(id,
                    token_inicial,
                    M,
                    num_procesos,
                    pipes,
                    debug_flag);
    }
    return EXIT_SUCCESS;
}




