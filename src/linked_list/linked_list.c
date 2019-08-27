////////////////////////////////////////////////////////////
// Referencia : código lista ligada sacado del curso     //
// Estructuras de Datos 2019-1 Taller 1                 //
/////////////////////////////////////////////////////////

#include <stdlib.h>
#include <stdio.h> //BORRAR
#include "linked_list.h"

////// Funciones privadas ///////

/** Crea un nodo a partir de un valor */
static Node* node_init(Program* value)
{
  // Pido memoria para el nodo
  Node* node = malloc(sizeof(Node));

  // Le asigno el valor correspondiente
  node -> value = value;

  // Hago que apunte al null
  node -> next = NULL;

  // Retorno el nodo
  return node;
}

/** Libera iterativamente la memoria del nodo y los nodos siguientes */
static void node_destroy(Node* node)
{
  // Esta funcion es iterativa porque puede generar stack overflow
  while (node)
  {
    Node* next = node -> next;
    destroy_program(node->value);
    free(node);
    node = next;
  }
}

// Agrega un nodo a la lista ligada
static void ll_add_node(LinkedList* ll, Node* node)
{
  if (!ll -> head)
  {
    // Si la lista no tiene elementos, el nodo es inicial y final
    ll -> head = node;
    ll -> tail = node;
  }
  else
  {
    // Sino, lo agrego al final
    ll -> tail -> next = node;
    ll -> tail = node;
  }
  ll->size++;
}

////// Funciones publicas ///////

/** Crea una lista ligada vacia */
LinkedList* ll_init()
{
  // Creo la lista ligada
  LinkedList* ll = malloc(sizeof(LinkedList));

  // Pongo sus punteros en nulo y su contador en 0
  ll -> head = NULL;
  ll -> tail = NULL;
  ll-> size = 0;

  // Retorno la lista ligada
  return ll;
}

/** Destruye la lista ligada */
void ll_destroy(LinkedList* ll)
{
  // Primero destruyo los nodos de la lista
  node_destroy(ll -> head);

  // Luego libero la memoria de la lista
  free(ll);
}

/** Agrega un valor a la lista */
void ll_append(LinkedList* ll, Program* value)
{
  // Creo el nodo a insertar
  Node* node = node_init(value);

  // Funcion que agrega un nodo al final de una lista ligada
  ll_add_node(ll, node);
}

Program* ll_pop(LinkedList* ll){
  Node* nodo = ll->head;
  if (!ll -> head->next){
    ll->size = 0;
    ll->head = NULL;
    ll->tail = NULL;
  }

  else{
    ll->head = ll->head->next;
    ll->size--;
  }
  Program* program = nodo->value;
  free(nodo);
  return program;
}