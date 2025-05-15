#include "funciones.h"
#include <bits/getopt_core.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



//Descripcion: Funcion para procesar anillo de procesos
//Dom: id_proceso X token X M X num_procesos X pipe_in X pipe_out
//Rec: void

void anillo_procesos(int id_proceso, int token, int M, int num_procesos, int pipe_in, int pipe_out){
    srand(time(NULL) + id_proceso);

    //Mientras el token sea mayor a 0 (es decir, todos los procesos siguen vivos)
    while(token >= 0){
        //Se decrementa y esto se aplica al token
        int decremento = rand() % M;
        token -= decremento;
        printf("Proceso %d; Token recibido: %d; Token resultante: %d\n", id_proceso, token + decremento, token);

        //Si el token es menor a 0 (en este caso un proceso se elimina)
        if(token < 0){
            //Cerrar el pipe 
            close(pipe_out);
            exit(0);
        }

        //Se escribe el resultado a traves del pipe
        write(pipe_out, &token, sizeof(int));
        sleep(1);
    }
}


//Descripcion: Funcion para iniciar con los pipes
//Dom: num_procesos X pipes
//Rec: void

void iniciar_pipes(int num_procesos, int pipes[]){
    int i;
    for(i = 0; i < num_procesos; i++){
        if(pipe(pipes + 2 * i) == -1){
            perror("Error en la creacion del pipe");
            exit(EXIT_FAILURE);
        }
    }
}

//Descripcion: Funcion que permite seleccionar al lider
//Dom: num_procesos X pipes
//Rec: void

void lider(int num_procesos, int pipes[]){
    int nuevo_lider = num_procesos - 1;
    printf("El nuevo lider es: (Proceso %d\n)", nuevo_lider);
}

//Descripcion: Funcion que permite el parseo de argumentos ingresados
//Dom: argc X argv X num_procesos X token X M
//Rec: void

void parseo_argumentos(int argc, char * argv[], int * num_procesos, int * token, int * M){
    int opt;
    *num_procesos = 4;
    *token = 10;
    *M = 10;
    
    while ((opt = getopt(argc, argv, "t:M:p:")) != -1){
        switch (opt) {
            case 't':
                *token = atoi(optarg);
                break;
            case 'M':
                *M = atoi(optarg);
                break;
            case 'p':
                *num_procesos = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Uso: %s -t token -M decremento -p num_procesos\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}
