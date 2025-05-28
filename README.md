Laboratorio 2 - Sistemas Operativos 1-2025
Nombre: Alex Mellado Gamboa

----------------------------------------
INSTRUCCIONES DE COMPILACIÓN
----------------------------------------

Para compilar el programa, se debe ejecutar el comando:

    make

Esto generará el archivo ejecutable llamado 'desafio2'.


----------------------------------------
Para lograr su ejecución basta con ingresar el comando 
    
    make run

----------------------------------------
INSTRUCCIONES DE EJECUCIÓN
----------------------------------------

Formato de ejecución:

    ./desafio2 -t <token> -M <resta> -p <procesos>

Donde:

    -p : número de procesos a crear
    -t : token (positivo)
    -M : valor aleatorio a restar al token

Ejemplo de ejecución:

    ./desafio1 -t 10 -M 10 -p 4


----------------------------------------
NOTAS ADICIONALES
----------------------------------------
-Como inconveniente, existe el problema que se realiza la ejecución del código pero se "pega" al momento de eliminar un proceso (se elimina, pero después no continua), por lo cual, no coincide con el resultado esperado.

-Cada función cuenta con su descripción de lo que realiza, el dominio que recibe, y el retorno de la misma.

