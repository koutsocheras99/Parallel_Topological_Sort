#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define filename "dag.txt"

char first_line[256]; // global variable for the first line string

// get the first element of the f_line which corresponds to the rows(or columns) of the matrix

char *get_matrix_size(void) {

	FILE* file = fopen(filename, "r"); 

	int i = 0;

	while (fgets(first_line, sizeof(first_line), file)) 
	{
		i++;

		if(i == 1) // if we are in the first line of the text file
		{ 

		char *matrix_size = strtok(first_line, " "); // using strtok get the first word of the first line as the delimiter is " "

		return matrix_size;

        	}
    	}

	fclose(file);

}

char *get_edges_number(void) {

	FILE* file = fopen(filename, "r");

	int i = 0;

	while (fgets(first_line, sizeof(first_line), file))
	{
		i++;

		if(i == 1)
		{ 

		char *edges = strtok(first_line, " "); // first word (corresponds to rows or columns)
		edges = strtok(NULL, " "); // second word (corresponds to rows or columns)
	   	edges = strtok(NULL, " "); // third word which corresponds to the number of the edges

		return edges;

        	}
    	}

	fclose(file);

}
