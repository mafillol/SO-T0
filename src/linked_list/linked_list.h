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

/** Estructura de un nodo de la lista simplemente ligada */
typedef struct node
{
  /** Nodo sigiente en la lista (NULL si no hay otro nodo) */
  struct node* next;
  /** valor que guarda el nodo */
  Program* value;
} Node;

/** Lista ligada. Mantiene el nodo inicial y el final */
typedef struct linked_list
{
  /** Nodo inicial de la lista (posicion 0) */
  Node* head;
  /** Nodo final de la lista */
  Node* tail;
  /**Tamaño de la lista*/
  int size;
} LinkedList;


/////// Funciones publicas de la lista ////////

/** Crea una lista ligada vacia */
LinkedList* ll_init();

/** Destruye la lista ligada */
void ll_destroy(LinkedList* ll);

/** Agrega un valor a la lista */
void ll_append(LinkedList* ll, Program* value);


////////////////////////////////////////////////////////////
// Referencia : código lista ligada sacado del curso     //
// Estructuras de Datos 2019-1 Taller 1                 //
/////////////////////////////////////////////////////////


/// Funciones

// Crea un programa nuevo
Program* init_program(int n_arg, char* name);

Program** read_file(const char* name_file);

void destroy_program(Program* program);

Program* ll_pop(LinkedList* ll);

void strip(char* string);

volatile pid_t* remove_pid(pid_t pid, volatile pid_t * list, int max);

volatile pid_t* add_pid(pid_t pid, volatile pid_t * list, int max);