#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "list.h"
#include "init_matrix_specs.h"

#define filename "dag.txt"

int main(int argc, char *argv[]) {

	// check if user has specified the number of threads during the program execution 
	if(argc != 2) 
  	{
    	printf("Please specify the number of threads.\n");
    	exit(EXIT_FAILURE);
  	}
	
	else 
	{
		int size, edges, x, y;
		char *p_size, *p_edges;

		// structs for timing 
		struct timeval start, end;

		// get the size(rows=columns) and number of edges from the init_matrix_specs file
		p_size = get_matrix_size();
	  	p_edges = get_edges_number();

		// string to int
		size = atoi(p_size); 
		edges = atoi(p_edges);

		printf(" Size of the matrix (rows x columns): %d x %d \n",size, size);
		printf(" Number of edges in the graph: %d\n\n\n",edges);

		// initialize the matrix and Node_Degree arrays just like the instructions
	 	int Matrix[size][size];
		int Node_Degree[size][2];

		// initializing the matrix cells with 0
		for (int i = 0; i < size; i++)
			for (int j = 0; j < size; j++)
				Matrix[i][j] = 0;

		// initializing the Node_Degree cells with 0
		for (int i = 0; i < size; i++)
			for (int j = 0; j < 2; j++)
				Node_Degree[i][j] = 0;

	  	FILE* file = fopen(filename, "r");
		int i = 0;

		// insert 1 in the cells of the matrix that have an edge just like the instructions
		while (fgets(first_line, sizeof(first_line), file))
		{
			i++;
			if(i > 1) // get the numbers from the second row and beyond
			{ 
			char *first_number = strtok(first_line, " "); // first word (corresponds to rows or columns)

			char *second_number = strtok(NULL, " "); // second word (corresponds to rows or columns)
		  
			x = atoi(first_number);
			y = atoi(second_number);

		  		// printf(" %d %d\n\n",x,y); // printing the all the numbers and the edges of the matrix (identical to dag.txt)

		 		Matrix[x][y] = 1;
			}
	  	}

		fclose(file);
	  
	  	// printf(" Matrix:\n");

		// printing the matrix and setting the node_degree array 
		for(int i=0;i<size;i++)
		{
			Node_Degree[i][0] = 1;

			for(int j=0;j<size;j++)
			{
				// printf(" %d ", Matrix[i][j]); // print matrix

				// if there is a ingoing edge to the node increase the degree by 1
				if(Matrix[j][i] == 1)
					Node_Degree[i][1]++;
			}
			
			// printf("\n"); // uncomment for proper matrix print(newline at the end of each row)
		}

		printf("\n\n\n");

	  	// printf(" Node Degree:\n");
	  
		// printing the node_degree array
		/*for(int i=0;i<size;i++)
		{
			for(int j=0;j<2;j++)
			{
				printf(" %d ", Node_Degree[i][j]);
			}

			printf("\n");
		}*/
		
		// Check that there is no symmetry that would mean loops
    	for (int i = 0; i < size; i++)
		{
        	for (int j = 0; j < size; j++)
		    {
		        if (Matrix[i][i])
		        {
					printf("\n The graph contains self loop and it's not a DAG\n");
					exit(0);
				}
		        if (Matrix[i][j] == 1 && Matrix[j][i] == 1)
		        {
		            printf("\n The graph contains circle \n");
		            exit(1);
		        }
		    }
		}
		// initializing S,L lists 
		struct Node* S = NULL;
		struct Node* L = NULL;
		
		int Threads = atoi(argv[1]);
		
		// start timing
		gettimeofday(&start, NULL);

		// start of parallel region
		#pragma omp parallel num_threads(Threads) shared(S,L) 
		{
			// each thread initialize its id variable with its id number
		    int id = omp_get_thread_num();
		    
			// scanning Matrix for nodes unused
     		// separating array's columns with hops according to thread number
		    for (int y = id; y < size; y += Threads)
		    {
		        int nodeFound = 0;
				// searching each row of the column
		        for (int x = 0; x < size; x++)
		        {
					// if Matrix[x][y] == 1 then there is edge from x to y
		            if (Matrix[x][y])
		            {
		                nodeFound++;
		                break;
		            }
		        }
				// no edge found so the y:(node) can pushed into S list
		        if (!nodeFound)
		        {
					// the S list is shared and we put the push action into critical for mutual exclusion
					#pragma omp critical
		            {
		                push(&S, y);
		            }
		        }
		    }

			// as we have initialized S list with nodes, we make task for the manipulation of each node in S
			#pragma omp task
		    {
				while (S!=NULL)
		    	{
		        	int node;
					// the S, L list is shared and we put the push and dequeue action into critical for mutual exclusion
					#pragma omp critical
		        	{
		            	node = dequeue(&S);
		            	if (node == -1) // if S list is empty end 
		                	exit(0);
					// put node into L list
		            push(&L, node);
		        	}

				// creating list with all the nodes that have input edge from the node we just pushed into L
		        struct Node *neigboors = NULL;
		        for (int i = 0 ;i < size; i++)
		        {	
		            if (Matrix[node][i])
		            {
						#pragma omp critical
		               	{
						push(&neigboors, i);
						//erase the incoming edge from the node by setting the value from 1 to 0
			            Matrix[node][i] = 0;
						}
		            }
		        }

				// creating taskwait structure block in order to wait all the threads update Matrix
          		// then we check in neighbors for nodes with no incoming edges and we put them into S 
				#pragma omp taskwait
				{
					while (neigboors != NULL)
		        	{
				        int zero_in = dequeue(&neigboors);
				        int nodeFound = 0;
				        for (int i = 0 ;i < size; i++)
				        {
				            if (Matrix[i][zero_in])
				            {
				                nodeFound++;
				                break;
				            }
				        }
		            if (!nodeFound)
		            {
						#pragma omp critical
		                {
		                    push(&S, zero_in);
		                }
					}
		        	}
				}
		    	}
			}
		}
		gettimeofday(&end, NULL);
	  	printf("\n\n L list: \n");
		printList(L);
		printf("\n\n Time in micro-seconds: %ld\n", (end.tv_usec-start.tv_usec));
		return 0;
		 
	  printf("\n");

	}
}