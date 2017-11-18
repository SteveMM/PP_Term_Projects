#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "nxn_mult_table.h"

// Macro definition(s)
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

// Constants
static const int FIRST = 0;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
      if (process_rank == ROOT)
        fprintf(stderr, "ERROR: Missing multiplication table size.\nProgram Usage: table_size <size>\n");  
    }
    else
    {
      // Define problem paramaters
      long long int table_size = atoll(argv[1]);
        
      MPI_Status status;
      MPI_Request request;

      int process_rank;
      int num_processors;

      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
      MPI_Comm_size(MPI_COMM_WORLD, &num_processors);

      if (process_rank == ROOT)
      {
          int evaluate_length = 0, i_start = 0;
          // Calculate evaluate_length, the number of cells this processor will calculate
          evaluate_length = floor(n / num_processors);

          if (process_rank < table_size % num_processors)
  		          evaluate_length += 1;

          int remainder = MIN(p_rank, table_size % num_processors);
          
          const int cells = table_size * table_size + (table_size / 2);
          const int chunk = process_rank * evaluate_length + remainder;
          printf("Process %i chunk: %i\n", process_rank, chunk);
          
          // Calculate all (i,j) indicies for each process to start at
          const int offset = 1;
          const int start = 1;
          int end = process_rank * chunk;
          
          int i = start;
          int j = start;
          while (end > 0)
          {
              end--; chunk--; i++;
              if (i == (table_size + offset)) 
              {
                  j++; i = j;
              }
          }
          
          printf("process %i (i,j) = (%i,%i)", process_rank, i, j);
          
          // Distribute (i,j) pairs to each process
      }
      else
      {
          // Search for unique mult_table elements within each process
      }
    }

    MPI_Finalize();
    return 0;
}
