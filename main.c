/*
 *  Created by Saul Montes De Oca
 *  15/09/2020
 *
 *  N Processes Token Ring Network
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char const *argv[])
{
    int children = 0;
    if(argc > 1)
    {
        children = atoi(argv[1]);
    } else
    {
        // No number of child processes
        printf("Please enter number of children processes to exeute next time.\n");
        return -1;
    }

    // Processes
    pid_t pid;

    // Se declara un array de tuberias, cada dos espacios representa una
    // tuberia y es por donde va a ir viajando el testigo
    int tuberia[2*children];

    // De esta forma se hacen pipes los espacios
    for (int i = 0; i < children; i++) {
        pipe(&tuberia[2*i]);
    } 

    // Generando T (testigo)
    char testigo = 'T';
    // Declarando char input
    char input;

    int i = 1;
    while (i <= children)
    {
        pid = fork();

        if (pid == 0)
        {
            // Se cierra write de la tuberia actual
            close(tuberia[(i*2 - 1)]);
            // Se lee de la tuberia actual
            read(tuberia[i*2 - 2], &input, sizeof(char));

            printf("—-> Soy el proceso %d con PID %d  y recibí el testigo '%c', el cual tendré por 5 segundos\n", i, getpid(), input);

            sleep(5); // Holding

            // Cerrando read de siguientetuberia
            close(tuberia[(i*2)]);

            // Escribiendo en siguiente tuberia
            write(tuberia[i*2 + 1], &input, sizeof(char));

            printf("<—- Soy el proceso %d con PID %d y acabo de enviar el testigo '%c'\n", i, getpid(), input);
        }
        else if (pid == -1)
        {
            // Error al crear hijo
            printf("Hubo un error al crear proceso hijo. Numero de procesos hijos creados hasta ahora: %d\n", i + 1);
            break;
        }
        else
        {
            // Si es el primer proceso, el padre de todos
            if (i == 1)
            {
                // Cerramos read de la primera tuberia
                close(tuberia[0]);
                // Escribimos en la primera tuberia
                write(tuberia[1], &testigo, sizeof(char));
                printf("Soy el proceso padre con PID = %d y envío '%c'\n", pid, testigo);
            }
            // cada hijo al tener otro hijo hace break y no seguira en el loop
            break;
        }
        i++;
    }


    // Cada padre espera a su hijo
    waitpid(pid, NULL, 0);


    return 0;
}
