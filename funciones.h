//Librerias a utilizar//
#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdbool.h>
//Se utiliza stdbool para el parseo de argumentos ()



//****************************Comienzo y desglose de datos y funciones******************************//

//****************************variables estaticas (no cambiara en el tiempo)//
#define PROCESOS 100
//****************************Fin variables estaticas***********************************************//

//Datos globales al igual que el laboratorio 1 pero ahora cada uno utiza su propia variable//
extern int id_proceso;
extern int num_procesos;
extern int token_global;
extern int valor_decremento;
extern int pipes[PROCESOS][2];
//Fin variables globales externas//



//Descripcion: Funcion que permite inicio de pipes
//Dom: num_procesos X pipes
//Rec: void

void iniciar_pipes(void);


//Descripcion: Funcion que aplica el decremento al token
//Dom: token X M
//Rec: numero con el decremento aplicado

int aplicar_decremento(int token, int M);


//Descripcion: Funcion que permite el parseo de argumentos ingresados
//Dom: argc X argv X num_procesos X token X M X Debug
//Rec: (True si se realizo operacion, false en caso que no)


bool parseo_argumentos(int argc, char *argv[]);


//Descripcion: Funcion que permite la configuracion de pipes (redireccion)
//Dom: id_proceso
//Rec: void

void configurar_pipes(int id_proceso);

//Descripcion: Funcion para procesar el anillo procesos
//Dom: id_proceso
//Rec: void

void anillo_procesos(int id_proceso);


//Descripcion: Funcion para generar los hijos con fork y redirigirlo a los pipes
//Dom: void
//Rec: void

void procesos_hijos(void);


//Descripcion: Funcion para comenzar con el juego
//Dom: void
//Rec: void

void iniciar_juego(void);

//Descripcion: Funcion que permite esperar por el proceso
//Dom: void
//Rec: void

void esperar_procesos(void);


#endif 