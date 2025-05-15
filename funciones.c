#include "funciones.h"



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
    }
}