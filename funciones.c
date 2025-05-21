#include "funciones.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


//************************************Desglose funciones**********************************//







//Descripcion: Funcion que permite configurar el anillo de procesos con pipes
//Dom: id_proceso X num_procesos X pipes
//Rec: void

void configurar_anillo_proc(int id_proceso, int num_procesos, int pipes[][2]){
    int entrada = (id_proceso + num_procesos - 1) % num_procesos;
    int salida = id_proceso;

    //Manejo con dup2 para redirecciones tanto entrada y salida de archivos
    dup2(pipes[entrada][0], STDIN_FILENO);
    dup2(pipes[salida][1], STDOUT_FILENO);

    //Ahora se cierran los pipes
    int i;
    for(i = 0; i < num_procesos;i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

}


//Descripcion: Funcion para iniciar con los pipes
//Dom: num_procesos X pipes
//Rec: void

int iniciar_pipes(int num_procesos, int pipes[][2]){
    int i, j;
    for(i = 0; i < num_procesos; i++){
        if(pipe(pipes[i]) == -1){
            perror("Error creacion de pipe");
            for(j = 0; j < i; j++){
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return -1;
        }
    }
    return 0;
}

//Descripcion: Funcion auxiliar que permite obtener el decremento
//Dom: M
//Rec: numero decrementado

int decremento(int M){
    int dec = rand() % M;
    return dec;
}

//Descripcion: Funcion que aplica el decremento al token
//Dom: token X M
//Rec: numero con el decremento aplicado

int aplicar_decremento(int token, int M){
    int dec = decremento(M);
    return (token - dec);
}



//Descripcion: Funcion que permite el parseo de argumentos ingresados
//Dom: argc X argv X num_procesos X token X M X debug
//Rec: (True si se realizo operacion, false en caso que no)


bool parseo_argumentos(int argc, char *argv[],int *num_procesos, int * token, int *M, bool *debug_flag) {
    int opt;
    *num_procesos = 4;
    *token         = 10;
    *M             = 10;
    *debug_flag    = false;

    while ((opt = getopt(argc, argv, "p:t:M:Dh")) != -1) {
        switch (opt) {
            case 'p':
                *num_procesos = atoi(optarg);
                break;
            case 't':
                *token = atoi(optarg);
                break;
            case 'M':
                *M = atoi(optarg);
                break;
            case 'D':
                *debug_flag = true;
                break;
            case 'h':
                printf("Uso: %s -p <n_procesos> -t <token_inicial> -M <max_decremento> [-D] [-h]\n",
                       argv[0]);
                return false;
            default:
                return false;
        }
    }

    if (*num_procesos <= 0) {
        fprintf(stderr, "Error: -p debe ser un entero > 0\n");
        return false;
    }
    if (*token < 0) {
        fprintf(stderr, "Error: -t debe ser un entero >= 0\n");
        return false;
    }
    if (*M <= 0) {
        fprintf(stderr, "Error: -M debe ser un entero > 0\n");
        return false;
    }
    return true;
}


//Descripcion: Funcion que permite notificar la eliminacion de un proceso
//Dom: id_proceso X padre
//Rec: void

void notifica_eliminacion(int id_proceso, int log_padre) {
    if (write(log_padre, &id_proceso, sizeof(int)) == -1) {
        perror("Error al notificar eliminacion");
        exit(EXIT_FAILURE);
    }
}


//Descripcion: Funcion auxiliar para procesar y imprimir token
//Dom: token X id_proceso X token_resultante
//Rec: void

void imprimir_token(int id_proceso, int token, int token_resultante){
    printf("Proceso %d; Token recibido: %d; Token resultante: %d\n",
           id_proceso, token, token_resultante);
    fflush(stdout);
}


//Descripcion: Funcion que permite el inicio de ronda de procesos (papa caliente)
//Dom: id_proceso_lider X token_inicial X num_procesos X pipes
//Rec: void

void iniciar_ronda(int id_proceso_lider, int token_inicial, int num_procesos, int pipes[][2]){
    //Escritura archivo de salida
    if(write(pipes[id_proceso_lider][1], &token_inicial, sizeof(int)) == -1){
        perror("Error al iniciar ronda");
    }
    printf("Líder %d inicia nueva ronda con token %d\n",
           id_proceso_lider, token_inicial);
    fflush(stdout);
}


//Descripcion: Funcion que permite coordinar el juego
//Dom: num_procesos x pipes X log X token_inicial
//Rec: void

void coordinar_juego(int num_procesos,
                     int pipes[][2],
                     int log_fd,
                     int token_inicial){

    bool alive[num_procesos];
    memset(alive, true, sizeof(alive));
    int vivos = num_procesos;

    iniciar_ronda(0, token_inicial, num_procesos, pipes);

    while (vivos > 1) {
        int eliminado;
        if (read(log_fd, &eliminado, sizeof(eliminado)) <= 0) {
            perror("parent_listen: read log_fd");
            break;
        }

        alive[eliminado] = false;
        vivos--;
        int candidato = (eliminado + 1) % num_procesos;
        while (!alive[candidato])
            candidato = (candidato + 1) % num_procesos;

        //Reinicio token
        iniciar_ronda(candidato, token_inicial, num_procesos, pipes);
    }

    //Cerrar los pipes
    for (int i = 0; i < num_procesos; ++i) {
        close(pipes[i][1]);
    }


    for (int i = 0; i < num_procesos; ++i) {
        if (alive[i]) {
            printf("Proceso %d es el ganador\n", i);
            break;
        }
    }
}



//Descripcion: Funcion que permite la creacion del anillo de procesos (fork)
//Dom: num_procesos X pipes
//Rec: numero

int iniciar_anillo_proc(int num_procesos, int pipes[][2]){
    int i;
    for(i = 0; i < num_procesos;i++){
        pid_t pid = fork();
        if(pid < 0){
            perror("Error al crear al hijo");
            exit(EXIT_FAILURE);
        }
        if(pid == 0){ //Se crea el hijo
            return i;
        }
    }
    return -1;
}

//Descripcion: Funcion que permite crear anillo procesos
//Dom: id_proceso X token_inicial X M X num_procesos X pipes X D
//Rec: void

void anillo_proc(int id_procesos,
                 int token_inicial,
                 int M,
                 int num_procesos,
                 int pipes[][2],
                 bool D,
                 int log_fd) {
    configurar_anillo_proc(id_procesos, num_procesos, pipes);
    srand(time(NULL) + id_procesos);

    int token;
    while (1) {
        ssize_t n = read(STDIN_FILENO, &token, sizeof(token));
        if (n < 0) {
            perror("Error al leer token");
            exit(EXIT_FAILURE);
        }
        if (token < 0) {
            write(STDOUT_FILENO, &token, sizeof(token));
            exit(EXIT_SUCCESS);
        }

        int original = token;
        token = aplicar_decremento(token, M);
        if(D) imprimir_token(id_procesos, original, token);

        if(token <= 0) {
            notifica_eliminacion(id_procesos, log_fd);
            exit(EXIT_SUCCESS);
        } else{

            if(write(STDOUT_FILENO, &token, sizeof(token)) == -1) {
                perror("Error escritura token");
                exit(EXIT_FAILURE);
            }
        }
    }
}