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

    char *pvalue = NULL;
    int p;
    while ((p = getopt (argc, argv, "n:")) != -1)
        switch (p)
        {
        case 'n':
            pvalue = optarg;
            break;
        case '?':
            if (optopt == 'n')
                fprintf (stderr, "Opción -%c requiere un argumento.\n", optopt);
            else if (isprint (optopt))
                fprintf (stderr, "Opción desconocida '-%c'.\n", optopt);
            else
                fprintf (stderr,
                         "Opción desconocida '\\x%x'.\n",
                         optopt);
            return 1;
        default:
            abort ();
    }

    for (int index = optind; index < argc; index++)
        printf ("El argumento no es una opción válida %s\n", argv[index]);
    
    children = atoi(pvalue);

    // Processes
    pid_t pid;
    pid_t *pidChildProcesses = (pid_t *) malloc((children-1) * sizeof(pid_t));
    pid_t *aux = pidChildProcesses;

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
    int j = 0;
    while (i < children)
    {
        pid = fork();
        if (pid == 0)
        {
            if(i == children -1){
                while(j < 3){
                    // Se cierra write de la tuberia actual
                    close(tuberia[(i*2 + 1)]);
                    // Se lee de la tuberia actual
                    read(tuberia[i*2], &input, sizeof(char));

                    printf("—-> Soy el proceso %d con PID %d  y recibí el testigo '%c', el cual tendré por 5 segundos\n", i, getpid(), input);

                    sleep(5); // Holding

                    // Cerrando read de siguientetuberia
                    close(tuberia[0]);

                    // Escribiendo en siguiente tuberia
                    write(tuberia[1], &input, sizeof(char));

                    printf("<—- Soy el proceso %d con PID %d y acabo de enviar el testigo '%c'\n", i, getpid(), input);
                    j++;
                }

            } else
            {
                while(j < 3){
                    // Se cierra write de la tuberia actual
                    close(tuberia[(i*2 + 1)]);
                    // Se lee de la tuberia actual
                    read(tuberia[i*2], &input, sizeof(char));

                    printf("—-> Soy el proceso %d con PID %d  y recibí el testigo '%c', el cual tendré por 5 segundos\n", i, getpid(), input);

                    sleep(5); // Holding

                    // Cerrando read de siguientetuberia
                    close(tuberia[((i*2) + 2)]);

                    // Escribiendo en siguiente tuberia
                    write(tuberia[(i*2) + 3], &input, sizeof(char));

                    printf("<—- Soy el proceso %d con PID %d y acabo de enviar el testigo '%c'\n", i, getpid(), input);
                    j++;
                }
            }
            exit(0);
        }
        else if (pid == -1)
        {
            // Error al crear hijo
            printf("Hubo un error al crear proceso hijo. Numero de procesos hijos creados hasta ahora: %d\n", i + 1);
            break;
        }
        else
        {
            // Si es el proceso padre escribe la primera vez el testigo
            if (i == 1)
            {
                // Guardando los pids de child processes para esperarlos despues
                *aux = pid;
            }
        }
        i++;
        aux++;
    }


    printf("Escribiendo testigo:\n");
    write(tuberia[1], &testigo, sizeof(char));

    //Lectura y escritura del padre
    while(j < 3){
        // Se cierra write de la tuberia actual
        close(tuberia[1]);
        // Se lee de la tuberia actual
        read(tuberia[0], &input, sizeof(char));

        printf("—-> Soy el proceso 0 con PID %d  y recibí el testigo '%c', el cual tendré por 5 segundos\n", getpid(), input);

        sleep(5); // Holding

        // Cerrando read de siguientetuberia
        close(tuberia[2]);

        // Escribiendo en siguiente tuberia
        write(tuberia[3], &input, sizeof(char));

        printf("<—- Soy el proceso 0 con PID %d y acabo de enviar el testigo '%c'\n", getpid(), input);
        j++;
    }

    // Proceso padre espera a todos sus hijos a que terminen
    aux = pidChildProcesses;
    pid_t *final = pidChildProcesses + (children-1);
    for (; aux < final; ++aux)
    {
        waitpid(*aux, NULL, 0);
    }
    
    free(pidChildProcesses);

    return 0;
}
