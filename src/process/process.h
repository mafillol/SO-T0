#pragma once
#include <time.h>

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


/// Funciones

// Crea un programa nuevo
Program* init_program(int n_arg, char* name);

Program** read_file(const char* name_file);

void destroy_program(Program* program);

void strip(char* string);