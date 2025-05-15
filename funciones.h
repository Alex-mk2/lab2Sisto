//Librerias a utilizar//
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>




//Comienzo y desglose de funciones//

//Descripcion: Funcion que permite procesar anillo de procesos
//Dom: id_proceso X token X M X num_procesos X pipe_in X pipe_out
//Rec: void

void anillo_procesos(int id_proceso, int token, int M, int num_procesos, int pipe_in, int pipe_out);


//Descripcion: Funcion que permite inicio de pipes
//Dom: num_procesos X pipes
//Rec: void

void iniciar_pipes(int num_procesos, int pipes[]);


//Descripcion: Funcion para crear fork con pipes
//Dom: num_procesos X pipes
//Rec: void

void iniciar_fork_pipes(int num_procesos, int pipes[]);


//Descripcion: Funcion para iniciar el lider de los procesos
//Dom: num_procesos X pipes
//Rec: void

void lider(int num_procesos, int pipes[]);
