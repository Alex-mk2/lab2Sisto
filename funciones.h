//Librerias a utilizar//
#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

//****************************Comienzo y desglose de funciones******************************//



//Descripcion: Funcion que permite configurar el anillo de procesos con pipes
//Dom: id_proceso X num_procesos X pipes
//Rec: void

void configurar_anillo_proc(int id_proceso, int num_procesos, int pipes[][2]);


//Descripcion: Funcion que permite inicio de pipes
//Dom: num_procesos X pipes
//Rec: void

int iniciar_pipes(int num_procesos, int pipes[][2]);


//Descripcion: Funcion auxiliar que permite obtener el decremento
//Dom: M
//Rec: numero decrementado

int decremento(int M);


//Descripcion: Funcion que aplica el decremento al token
//Dom: token X M
//Rec: numero con el decremento aplicado

int aplicar_decremento(int token, int M);


//Descripcion: Funcion que permite el parseo de argumentos ingresados
//Dom: argc X argv X num_procesos X token X M X Debug
//Rec: (True si se realizo operacion, false en caso que no)


bool parseo_argumentos(int argc, char *argv[],int *num_procesos,int *token, int *M, bool *debug_flag);

//Descripcion: Funcion que permite notificar la eliminacion de un proceso
//Dom: id_proceso X num_procesos X pipes
//Rec: void

void notifica_eliminacion(int id_proceso, int log_padre);


//Descripcion: Funcion auxiliar para procesar y imprimir token
//Dom: token X id_proceso X token_resultante
//Rec: void

void imprimir_token(int id_proceso, int token, int token_resultante);


//Descripcion: Funcion que permite el inicio de ronda de procesos (papa caliente)
//Dom: id_proceso_lider X token_inicial X num_procesos X pipes
//Rec: void

void iniciar_ronda(int id_proceso_lider, int token_inicial, int num_procesos, int pipes[][2]);


//Descripcion: Funcion que permite coordinar el juego
//Dom: num_procesos x pipes X log X token_inicial
//Rec: void

void coordinar_juego(int num_procesos,
                     int pipes[][2],
                     int log_fd,
                     int token_inicial);

//Descripcion: Funcion que permite la creacion del anillo de procesos (fork)
//Dom: num_procesos X pipes
//Rec: numero

int iniciar_anillo_proc(int num_procesos, int pipes[][2]);

//Descripcion: Funcion que permite crear anillo procesos
//Dom: id_proceso X token_inicial X M X num_procesos X pipes X D
//Rec: void

void anillo_proc(int id_procesos,
                 int token_inicial,
                 int M,
                 int num_procesos,
                 int pipes[][2],
                 bool D,
                 int log_fd); 



#endif 