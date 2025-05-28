#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <sys/wait.h>
#include <time.h>


//************************************Desglose funciones y variables globales**********************************//


//Procesamiento de los datos declarados en funciones h y mismos datos utilizados del lab1//
int id_proceso = -1;
int num_procesos = 0;
int token_global = 0;
int valor_decremento = 0;
int pipes[PROCESOS][2];
//Fin procesamiento de datos...



//Descripcion: Funcion para iniciar con los pipes
//Dom: num_procesos X pipes
//Rec: void

void iniciar_pipes(void){
    int i;
    //Inicio de pipes, y si es menor tira un error
    for(i = 0; i < num_procesos;i++){
        if(pipe(pipes[i]) < 0){
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }
}



//Descripcion: Funcion que aplica el decremento al token
//Dom: token X M
//Rec: numero con el decremento aplicado

int aplicar_decremento(int token, int M){
    int dec = rand() % M; 
    return token - dec;
}



//Descripcion: Funcion que permite el parseo de argumentos ingresados
//Dom: argc X argv X num_procesos X token X M X debug
//Rec: (True si se realizo operacion, false en caso que no)


bool parseo_argumentos(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "p:t:M:h:")) != -1) {
        switch (opt) {
            case 'p':
                num_procesos = atoi(optarg);
                break;
            case 't':
                token_global = atoi(optarg);
                break;
            case 'M':
                valor_decremento = atoi(optarg);
                break;
            case 'h':
                id_proceso = atoi(optarg);
                break;
            default:
                fprintf(stderr,
                    "Uso: %s -p <procesos> -t <token> -M <maxDecr> [-h <id>]\n",
                    argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (num_procesos <= 0 || token_global < 0 || valor_decremento < 0) {
        fprintf(stderr, "Parámetros inválidos\n");
        exit(EXIT_FAILURE);
    }
    return (id_proceso >= 0);
}

//Descripcion: Funcion que permite la configuracion de pipes (redireccion)
//Dom: id_proceso
//Rec: void

void configurar_pipes(int id_proceso){
    int entrada = (id_proceso - 1 + num_procesos) % num_procesos;

    //Se utiliza dup2 para duplicar entrada
    dup2(pipes[entrada][0], STDIN_FILENO);

    //Ahora dup2 para la salida
    dup2(pipes[id_proceso][1], STDOUT_FILENO);

    //Se cierran los pipes en donde fueron duplicados
    int i;
    for(i = 0; i < num_procesos;i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

//Descripcion: Funcion para procesar el anillo procesos
//Dom: id_proceso
//Rec: void

void anillo_procesos(int id_proceso) {
    int procesos_vivos = num_procesos;
    char buffer[64];

    while (1) {
        if(read(STDIN_FILENO, buffer, sizeof(buffer)) <= 0)
            exit(EXIT_FAILURE);

        //Cuando el proceso se elimina o muere
        if(buffer[0] == 'm'){
            //Se escribe en la entrada el proceso a eliminar
            read(STDIN_FILENO, buffer, sizeof(buffer));
            procesos_vivos = atoi(buffer);

            //Si queda solo un proceso con vida
            if(procesos_vivos == 1) {
                fprintf(stderr, "Proceso %d es el ganador\n", id_proceso);  
                exit(EXIT_SUCCESS);
            }

            //Se vuelve a leer desde la entrada 
            read(STDIN_FILENO, buffer, sizeof(buffer));
            int nueva_lectura = atoi(buffer);

            //Se duplica esta entrada a traves de pipes
            dup2(pipes[nueva_lectura][0], STDIN_FILENO);


            //Se escribe los procesos vivos
            snprintf(buffer, sizeof(buffer), "%d", procesos_vivos);
            write(STDOUT_FILENO, buffer, strlen(buffer) + 1);
            sleep(1);

            //Se escribe el token restante
            snprintf(buffer, sizeof(buffer), "%d", token_global);
            write(STDOUT_FILENO, buffer, strlen(buffer) + 1);
            continue;
        }
        procesos_vivos = atoi(buffer);
        read(STDIN_FILENO, buffer, sizeof(buffer));
        int token = atoi(buffer);

        //Se escribe el token que queda y la cantidad de procesos vivos que quedan
        fprintf(stderr, "Proceso %d; Token recibido: %d\n", id_proceso, token);  
        token = aplicar_decremento(token, valor_decremento);
        fprintf(stderr, "Proceso %d; Token resultante: %d\n", id_proceso, token); 


        //Si el token es menor que 0, se elimina el proceso
        if(token < 0) {
            fprintf(stderr, "Proceso %d eliminado\n", id_proceso);
            sleep(1);  

            //Se esribe el proceso que fue eliminado              
            write(STDOUT_FILENO, "m", 2);
            snprintf(buffer, sizeof(buffer), "%d", procesos_vivos - 1);
            write(STDOUT_FILENO, buffer, strlen(buffer) + 1);
            sleep(1);

            //Se escribe una nueva salida con la actualizacion del proceso
            int nueva_salida = (id_proceso - 1 + num_procesos) % num_procesos;
            snprintf(buffer, sizeof(buffer), "%d", nueva_salida);
            write(STDOUT_FILENO, buffer, strlen(buffer) + 1);
            sleep(1);
            exit(EXIT_SUCCESS);
        }

        //Se escribe tanto procesos vivos como el token restante 
        snprintf(buffer, sizeof(buffer), "%d", procesos_vivos);
        write(STDOUT_FILENO, buffer, strlen(buffer) + 1);
        sleep(1);


        snprintf(buffer, sizeof(buffer), "%d", token);
        write(STDOUT_FILENO, buffer, strlen(buffer) + 1);
        sleep(1);
    }
}


//Descripcion: Funcion para generar los hijos con fork y redirigirlo a los pipes
//Dom: void
//Rec: void

void procesos_hijos(void){
    for(int i = 0; i < num_procesos; i++){
        pid_t pid = fork();
        //Error de fork al momento de crear un proceso
        if(pid < 0){
            perror("fork"); exit(EXIT_FAILURE);
        }
        if(pid == 0){
            //Se llama a las funciones configurar pipes y anillo procesos creadas
            configurar_pipes(i);
            anillo_procesos(i);
            exit(EXIT_SUCCESS);
        }
    }
}




//Descripcion: Funcion para comenzar con el juego
//Dom: void
//Rec: void

void iniciar_juego(void){
    //Se inicia un buffer de 32 espacios
    char buffer[32];
    //Proceso lider 
    int leader = (num_procesos - 1 + num_procesos) % num_procesos;
    snprintf(buffer, sizeof(buffer), "%d", num_procesos);
    //Se escribe el resultado en los pipes
    write(pipes[leader][1], buffer, strlen(buffer) + 1);
    sleep(1);
    snprintf(buffer, sizeof(buffer), "%d", token_global);
    write(pipes[leader][1], buffer, strlen(buffer) + 1);
}

//Descripcion: Funcion que permite esperar por el proceso
//Dom: void
//Rec: void

void esperar_procesos(void){
    //Se realiza la espera de los procesos 
    while(wait(NULL) > 0){

    }
}