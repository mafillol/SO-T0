#include "process.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


Program* init_program(int n_arg, char* name){
	// Pido memoria para el programa
	Program* program = malloc(sizeof(Program));

	program->name = strdup(name);
	program->start_time = 0;
	program->end_time = 0;
	program->n_arg = n_arg;
	program->status = 0;
	program->process_pid = 0;

	program->arg = (char**)calloc(n_arg, sizeof(char*));

	return program;
}

Program** read_file(const char* name_file){

	//LinkedList* list = ll_init();

	char* line = NULL; 
	size_t size = 0;

	// Abro el archivo
	FILE* file = fopen(name_file, "r");
	getline(&line, &size, file);
	int N = atoi(line);

	Program** list = (Program**)calloc(N, sizeof(Program*));

	Program* program;

	for(int i=0; i<N; i++){
		getline(&line, &size, file);
		int n_arg = atoi(strtok(line, " "));
		char* name = strtok(NULL, " ");
		strip(name);

		program = init_program(n_arg, name);

		for(int j=0;j<n_arg;j++){
			char* token = strtok(NULL, " ");
			strip(token);
			program->arg[j] = strdup(token);
		}

		//l_append(list, program);
		list[i] = program;
	}

	//Liberamos la linea
	free(line);
	//Cerramos el archivo
	fclose(file);
	for(int p=0; p<N;p++){
		printf("%s\n", p.name);
	}
	
	//Retornamos la lista con los programas
	return list;
}

void destroy_program(Program* program){
	for(int i=0; i<program->n_arg;i++){
		free(program->arg[i]);
	}
	free(program->arg);
	program->arg = NULL;
	free(program->name);
	free(program);
	program = NULL;
}
//Funcion auxiliar obtenida del link: 
//https://cboard.cprogramming.com/c-programming/70320-how-remove-newline-string.html
void strip(char* string){
  string[strcspn (string,"\n")] = '\0';
}
