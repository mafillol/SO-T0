#include "../linked_list/linked_list.h"
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



volatile pid_t* pids; //Array para los pid de los hijos
int max_pids; //maxima cantidad de procesos concurrentes
Program** programs_list; // Array de programas
int N; // Cantidad total de programas (i.e len de programs_list)
int* count; // Cantidad de programas ejecutados
pid_t parent_pid;

static int default_handler()
{
    struct sigaction  action;

    memset(&action, 0, sizeof action);
    sigemptyset(&action.sa_mask);

    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;

    if (sigaction(SIGINT, &action, NULL) == -1)
        return errno;

    return 0;
}

// Funcion que maneja la señal de interrupcion
void end_all_process(int sig){
  printf("Vamos a terminar los procesos\n" );
  //Desactivo el seteo del tiempo
  signal (SIGCHLD, SIG_IGN);
  //Desactivo la alarma
  signal (SIGALRM, SIG_IGN);
  //Desactivo la interrupcion
  default_handler();

  for(int i=0;i<max_pids;i++){
    printf("proceso a morir %d\n", pids[i]);
    // Envio señal de termino de proceso
    kill(pids[i], SIGKILL);
    printf("Proceso terminado\n");
  }
}

void end_process(int sig){
  //Desactivo la alarma
  signal (SIGALRM, SIG_IGN);

  //Desactivo interrupcion
  signal (SIGKILL, SIG_IGN);

  //Mato al proceso
  pid_t end_pid = getpid();
  kill(end_pid, SIGKILL);

  //Reactivo interrupcion
  signal (SIGKILL, &end_all_process);
}

void set_end_time_program(int sig){
  time_t end = time(NULL);
  pid_t pid;
  int status;

  while ((pid = waitpid(-1, &status, WNOHANG)) != -1){
    if(pid != 0 && pid!=parent_pid){
      remove_pid(pid,pids, max_pids);
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

  // Map space para el array de los pids
  // Obtenido de link: https://stackoverflow.com/questions/26161486/creating-multiple-children-of-a-process-and-maintaining-a-shared-array-of-all-th
  pids = mmap(0, max_pids*sizeof(pid_t), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  memset((void *)pids, 0, max_pids*sizeof(pid_t));

  programs_list = mmap(0, N*sizeof(Program*), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  memset((void *)programs_list, 0, N*sizeof(Program*));

  count = mmap(0, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  memset((void *)count, 0, sizeof(int));

  int* index;
  index = mmap(0, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  memset((void *)index, 0, sizeof(int));

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
            programs_list[i]->status = INPROGRESS;
            count[0]++;
            programs_list[i]->process_pid = getpid();
            pids = add_pid(getpid(), pids, max_pids);

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
            sleep(10);
            execvp(programs_list[i]->name,args);
          }
        }
        // Si no hay ningun programa por correr, terminamos
        exit(1);
      }
    }

    // Esperamos a que todos los hijos ejecuten
    else{
      wait(NULL);
    }
  }

  printf("Vamos aqui\n");
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


  //   // Capturamos la señal de interrupcion
  //   signal(SIGINT, &end_process);


  return 0;
}