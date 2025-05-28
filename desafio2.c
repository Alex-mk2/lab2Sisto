#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>   

int main(int argc, char *argv[]) {
    srand(time(NULL));

    //Se llama a la funcion parseo de argumentos
    bool es_hijo = parseo_argumentos(argc, argv);
    if(es_hijo){
        anillo_procesos(id_proceso);
        return 0;
    }
    iniciar_pipes();
    procesos_hijos();
    iniciar_juego();
    esperar_procesos();
    return 0;
}


