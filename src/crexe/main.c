#include "../process/process.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/shm.h>



int max_pids; //maxima cantidad de procesos concurrentes
Program** programs_list; // Array de programas
int N; // Cantidad total de programas (i.e len de programs_list)
int* count; // Cantidad de programas ejecutados
pid_t parent_pid;

// Funcion que maneja la señal de interrupcion
void end_all_process(int sig){
  pid_t pid = getpid();
  if(pid != parent_pid){
    printf("Murio\n");
    //Desactivo el seteo del tiempo
    signal (SIGCHLD, SIG_IGN);
    //Desactivo la alarma
    signal (SIGALRM, SIG_IGN);

    kill(pid, SIGKILL);
  }
}

//Funcion que maneja la alarma
void end_process(int sig){
  //Desactivo la alarma
  signal (SIGALRM, SIG_IGN);

  //Desactivo interrupcion
  signal (SIGKILL, SIG_IGN);

  //Mato al proceso
  pid_t end_pid = getpid();
  kill(end_pid, SIGKILL);

  //Reactivo interrupcion
  signal (SIGINT, &end_all_process);
}

void set_end_time_program(int sig){
  time_t end = time(NULL);
  pid_t pid;
  int status;

  while ((pid = waitpid(-1, &status, WNOHANG)) != -1){
    if(pid != 0 && pid!=parent_pid){
      for(int i=0; i<N;i++){
        if(programs_list[i]->status == INPROGRESS && programs_list[i]->process_pid == pid){
          printf("entramos aqui\n");
          programs_list[i]->status = COMPLETE;
          programs_list[i]->end_time = end;
          break;
        }
      }
    }
  }

}

/** Esta función es lo que se llama al ejecutar tu programa */
int main(int argc, char *argv[]){ 

  parent_pid = getpid();

  //Cantidad maxima de pids concurrentes
  max_pids = atoi(argv[3]);

  //
  N = 4; ////*** CAMBIAR  **///

  // Map space
  // Obtenido de link: https://stackoverflow.com/questions/26161486/creating-multiple-children-of-a-process-and-maintaining-a-shared-array-of-all-th
  programs_list = mmap(0, N*sizeof(Program**), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  memset((void **)programs_list, 0, N*sizeof(Program**));

  count = mmap(0, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  memset((void *)count, 0, sizeof(int));


  //Leo el archivo y obtengo la lista de programas
  programs_list = read_file(argv[1]);

  signal(SIGINT, &end_all_process);

  // Capturamos la señal de termino del hijo
  signal(SIGCHLD, set_end_time_program);

  pid_t current_pid; 

  //Mientras no hayamos corrido todos los programas, creamos procesos
  while(count[0]<N){ 
    //Creamos los hijos
    for (int amount=0; amount<max_pids; amount++) {
      current_pid = fork();  
      
      // Si el proceso es exitoso, terminamos su ciclo
      if (current_pid==0) {
        break;
      }
    }

    //Comportamiento del hijo
    if (current_pid==0) {
      
      //Si todavia quedan programas por correr
      if(count[0]<N){
        for(int i=0; i<N;i++){
          // Si el programa no esta siendo ejecutado
          if(programs_list[i]->status == INCOMPLETE){
            printf("Me voy a ejecutar\n");
            programs_list[i]->status = INPROGRESS;
            count[0]++;
            programs_list[i]->process_pid = getpid();

            //Preparamos el array de argumentos terminados en NULL
            char** args =(char**)calloc(programs_list[i]->n_arg + 1, sizeof(char*));
            for(int aux=0; aux<programs_list[i]->n_arg; aux++){
              args[aux] = strdup(programs_list[i]->arg[aux]);
            }
            args[programs_list[i]->n_arg] = NULL;
            
            // Capturamos la señal de alarma
            signal (SIGALRM, end_process);

            // Establecemos una alarma para parar la ejecucion
            if(argc >= 5 && strcmp(argv[4],"-t") != 0){
              int max_time = atoi(argv[4]);
              alarm(max_time);
            }

            // Seteamos el tiempo de inicio
            programs_list[i]->start_time = time(NULL);

            // Ejecutamos el programa
            execvp(programs_list[i]->name,args);
          }
        }
        // Si no hay ningun programa por correr, terminamos
        exit(0);
      }
    }

    // Esperamos a que todos los hijos ejecuten
    else{
      wait(NULL);
    }
  }

  // El programa original escribe el archivo de salida
  if(getpid() == parent_pid){
    // El programa principal escribe el archivo de salida
    FILE* output = fopen(argv[2], "w");

    for(int i=0; i<N; i++){
      Program* program = programs_list[i];
      int result;
      if(program->status == COMPLETE){
        result = 1;
      }
      else{
        result = 0;
      }
      fprintf(output, "%s,%f,%d\n", program->name, difftime(program->end_time, program->start_time), result);
    }

    // Cerramos el archivo de salida
    fclose(output);

    //Liberamos la lista de programas
    for(int i=0; i<N; i++){
      destroy_program(programs_list[i]);
    }
    free(programs_list);
  }

  return 0;
}