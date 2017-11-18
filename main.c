#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Macro definition(s)
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

// Constants
static const int ROOT = 0;

int main(int argc, char *argv[])
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

  if (argc < 2)
  {
  if (process_rank == ROOT)
    printf("ERROR: Missing array length exponent. Usage: ./a2 [array length exponent]\n");
  // MPI clean-up
  MPI_Finalize();
  return 0;
  }

  int chunk = 0, i_start = 0;
  const int cells = ((table_size * table_size) / 2) + (table_size / 2);

  // TESTING
  if (process_rank == ROOT) 
  {
    printf("Cells: %i\n", cells);
  }

  // Calculate each processors chunk, the 
  // number of cells this processor will calculate
  chunk = floor(cells / num_processors);

  if (process_rank < cells % num_processors)
        chunk += 1;

  // TESTING
  printf("process %i chunk: %i", process_rank, chunk);

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

  // TESTING
  printf("process %i (i,j) = (%i,%i)\n", process_rank, i, j);

  MPI_Finalize();
  return 0;
}
