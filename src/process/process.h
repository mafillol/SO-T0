#pragma once
#include <time.h>
//////////////////////////////////////////////////
//////             ESTRUCTURAS            ////////
//////////////////////////////////////////////////
/** Representa el estado de un programa */
enum status
{
  INCOMPLETE = 0,
  COMPLETE = 1,
  ERROR = -1,
  INPROGRESS = 2,
};
typedef enum status Status;

typedef struct program
{

  /** nombre del programa */
  char* name;
  /** tiempo de inicio de ejecucion*/
  time_t start_time;
  /** tiempo de termino de ejecucion*/
  time_t end_time;
  /** cantidad de argumentos*/
  int n_arg;
  /** lista de argumentos que recibe el programa*/
  char** arg;
  /** Estado de termino del programa */
  Status status;
  /** Pid del proceso que ejecuta el programa*/
  pid_t process_pid;

} Program;

//////////////////////////////////////////////////
//////        FUNCIONES PRIVADAS          ////////
//////////////////////////////////////////////////


/** Crea un programa nuevo*/ 
Program* init_program(int n_arg, char* name);

/** Quita el salto de linea a un string*/
void strip(char* string);

//////////////////////////////////////////////////
///////        FUNCIONES PUBLICAS         ////////
//////////////////////////////////////////////////

/** Lee el archivo y entrega la lista de programas*/
Program** read_file(const char* name_file);
/** Destruimos el programa*/
void destroy_program(Program* program);

