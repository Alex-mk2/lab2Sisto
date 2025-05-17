#include "funciones.h"
#include <bits/getopt_core.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



//Descripcion: Funcion para procesar anillo de procesos
//Dom: id_proceso X token X M X num_procesos X pipe_in X pipe_out
//Rec: void

void anillo_procesos(int id_proceso, int M, int num_procesos, int pipe_in, int pipe_out){
    srand(time(NULL) + id_proceso);

    //Se crea para conocer el token actual del proceso
    int token_actual;

    //Se utiliza un entero con signo para saber el valor del token y procesarlo con los pipes
    ssize_t paridad_token;

    //Se ejecuta un ciclo "True"...
    while(1){
        //Se lee el token de entrada
        paridad_token = read(pipe_in, &token_actual, sizeof(int));

        if(paridad_token == 0){
            close(pipe_in);
            exit(0);
        }

        if(paridad_token < 0){
            perror("Error lectura pipe in");
            exit(EXIT_FAILURE);
        }

        int decremento = rand() % M;
        int token_recibido = token_actual;
        token_actual -= decremento;


        printf("Proceso %d; Token recibido: %d; Token resultante: %d\n", id_proceso, token_recibido, token_actual);

        //Limpieza archivo salida
        fflush(stdout);

        //Ahora si el token es negativo

        if(token_actual < 0){
            //Notificar eliminacion (se debe incorporar funcion...)
            if (write(pipe_out, &token_actual, sizeof(int)) != sizeof(int)) {
                perror("Error en la escritura del pipe de salida");
                exit(EXIT_FAILURE);
                
            }
            exit(id_proceso);
        }

        if (write(pipe_out, &token_actual, sizeof(int)) != sizeof(int)) {
            perror("Error en la escritura del pipe de salida");
            exit(EXIT_FAILURE);
        }

        sleep(1);
    }
}


//Descripcion: Funcion para iniciar con los pipes
//Dom: num_procesos X pipes
//Rec: void

void iniciar_pipes(int num_procesos, int pipes[][2]){
    int i;
    for(i = 0; i < num_procesos; i++){
        if(pipe(pipes[i]) == -1){
            perror("Error creacion de pipe");
            exit(EXIT_FAILURE);
        }
    }
}

//Descripcion: Funcion que permite seleccionar al lider
//Dom: num_procesos X pipes
//Rec: void

void lider(int num_procesos, int pipes[][2]){
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

//Descripcion: Funcion que permite la eleccion de un lider
//Dom: num_procesos X procesos_vivos
//Rec: proceso_lider

int elegir_lider(int num_procesos, int procesos_vivos[]){
    int maximo = procesos_vivos[0];

    int i;
    for(i = 0; i < num_procesos;i++){
        if(procesos_vivos[i] > maximo){
            maximo = procesos_vivos[i];
        }
    }
    return maximo;
}

//Descripcion: Funcion que permite el manejo de fork a traves de pipes
//Dom: num_procesos X pipes X token X M
//Rec: void

void iniciar_fork_pipes(int num_procesos, int pipes[][2], int token, int M){
    int i, j;
    for(i = 0; i < num_procesos; i++){
        //Se inicia fork
        pid_t pid = fork();
        if(pid < 0){
            perror("Error al crear proceso hijo");
            exit(EXIT_FAILURE);
        }

        if(pid == 0){
            //Se crea proceso hijo
            for(j = 0; j < num_procesos; j++){
                if(j != i) close(pipes[j][0]);
                if(j != (i + 1) % num_procesos) close(pipes[j][1]);
            }

            //Se redirigen tanto a salida como entrada a esos pipes
            dup2(pipes[i][0], STDIN_FILENO);
            dup2(pipes[(i + 1) % num_procesos][1], STDOUT_FILENO);

            //En los mismos pipes redirigidos, se cierran
            close(pipes[i][0]);
            close(pipes[(i + 1) % num_procesos][1]);

            //Se ejecuta la funcion anillo_procesos
            anillo_procesos(i, M, num_procesos, STDIN_FILENO, STDOUT_FILENO);
            exit(0);
        }
    }
}

//Descripcion: Funcion que permite anunciar el proceso eliminado
//Dom: id_proceso X num_procesos X pipes X procesos_vivos
//Rec: void

void anunciar_eliminado(int id_proceso, int num_procesos, int pipes[][2], int procesos_vivos[]){
    printf("Proceso %d eliminado: ", id_proceso);
    procesos_vivos[id_proceso] = 0; //Un proceso menos

    //Cierre de pipes...
    close(pipes[id_proceso][1]);

    int i;

    //Se recorren los procesos que aun siguen con vida
    for(i = id_proceso; i < num_procesos -1; i++){
        pipes[i][0] = pipes[i + 1][0];
        pipes[i][1] = pipes[i + 1][1];
    }

}