#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include <sys/wait.h>
#include <time.h>


//************************************Desglose funciones**********************************//


//Procesamiento de los datos declarados en funciones h//
int id_proceso = -1;
int num_procesos = 0;
int token_global = 0;
int valor_decremento = 0;
int pipes[PROCESOS][2];




//Descripcion: Funcion para iniciar con los pipes
//Dom: num_procesos X pipes
//Rec: void

void iniciar_pipes(void){
    int i;
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

int aplicar_decremento(int token, int M) {
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

void anillo_procesos(int id) {
    int vivos = num_procesos;
    char buf[64];

    while (1) {
        if(read(STDIN_FILENO, buf, sizeof(buf)) <= 0)
            exit(EXIT_FAILURE);

        if(buf[0] == 'm') {
            read(STDIN_FILENO, buf, sizeof(buf));
            vivos = atoi(buf);

            if (vivos == 1) {
                fprintf(stderr, "Proceso %d es el ganador\n", id);  
                exit(EXIT_SUCCESS);
            }

            read(STDIN_FILENO, buf, sizeof(buf));
            int nueva = atoi(buf);
            dup2(pipes[nueva][0], STDIN_FILENO);

            snprintf(buf, sizeof(buf), "%d", vivos);
            write(STDOUT_FILENO, buf, strlen(buf) + 1);
            sleep(1);

            snprintf(buf, sizeof(buf), "%d", token_global);
            write(STDOUT_FILENO, buf, strlen(buf) + 1);
            continue;
        }
        vivos = atoi(buf);
        read(STDIN_FILENO, buf, sizeof(buf));
        int token = atoi(buf);

        fprintf(stderr, "Proceso %d; Token recibido: %d\n", id, token);  
        token = aplicar_decremento(token, valor_decremento);
        fprintf(stderr, "Proceso %d; Token resultante: %d\n", id, token); 

        if (token < 0) {
            fprintf(stderr, "Proceso %d eliminado\n", id);               
            write(STDOUT_FILENO, "m", 2);
            snprintf(buf, sizeof(buf), "%d", vivos - 1);
            write(STDOUT_FILENO, buf, strlen(buf) + 1);
            sleep(1);

            int nueva = (id - 1 + num_procesos) % num_procesos;
            snprintf(buf, sizeof(buf), "%d", nueva);
            write(STDOUT_FILENO, buf, strlen(buf) + 1);
            sleep(1);
            exit(EXIT_SUCCESS);
        }
        snprintf(buf, sizeof(buf), "%d", vivos);
        write(STDOUT_FILENO, buf, strlen(buf) + 1);
        sleep(1);
        snprintf(buf, sizeof(buf), "%d", token);
        write(STDOUT_FILENO, buf, strlen(buf) + 1);
    }
}


//Descripcion: Funcion para generar los hijos con fork y redirigirlo a los pipes
//Dom: void
//Rec: void

void procesos_hijos(void){
    for(int i = 0; i < num_procesos; i++){
        pid_t pid = fork();
        if(pid < 0){
            perror("fork"); exit(EXIT_FAILURE);
        }
        if(pid == 0){
            configurar_pipes(i);
            anillo_procesos(i);
            exit(EXIT_SUCCESS);
        }
    }
}




//Descripcion: Funcion para comenzar con el juego
//Dom: void
//Rec: void

void iniciar_juego(void) {
    char buf[32];
    int leader = (num_procesos - 1 + num_procesos) % num_procesos;
    snprintf(buf, sizeof(buf), "%d", num_procesos);
    write(pipes[leader][1], buf, strlen(buf) + 1);
    sleep(1);
    snprintf(buf, sizeof(buf), "%d", token_global);
    write(pipes[leader][1], buf, strlen(buf) + 1);
}

//Descripcion: Funcion que permite esperar por el proceso
//Dom: void
//Rec: void

void esperar_procesos(void){
    int i;
    while(wait(NULL) > 0){
        
    }
}


