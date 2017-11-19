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
static const int COUNT = 1;

int main(int argc, char *argv[])
{   
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
      printf("ERROR: Missing table size. Usage: ./main [table size]\n");
      
      // MPI clean-up
      MPI_Finalize();
      return 0;
  }
  
  // Define problem paramaters
  int table_size = atol(argv[1]);
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

  int *data_array = (int*)malloc(chunk * sizeof(int));

  // Calculate all (i,j) indicies for each process to start at
  const int offset = 1;
  const int start = 1;
  int my_chunk = chunk;
  int end = process_rank * my_chunk;
  int i = start;
  int j = start;
  while (end > 0)
  {
      end--; my_chunk--; i++;
      if (i == (table_size + offset)) 
      {
          j++; i = j;
      }
  }

  my_chunk = chunk;
  int product;
  int index = 0;
  while (my_chunk > 0) 
  {
    data_array[index++] = i * j;
    printf("(%i,%i) product: %i\n", i, j, i * j);
    i++; my_chunk--;
    if (i == (table_size + offset)) 
    {
      j++; i = j;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Isend(&data_array, chunk, MPI_INT, ROOT, process_rank, MPI_COMM_WORLD, &request);

  int data_length;
  if (process_rank == ROOT) {
    for (int rank = 0; rank < num_processors; rank++) {
      MPI_Irecv(&data_array, chunk + 1, MPI_INT, ROOT, rank, MPI_COMM_WORLD, &request);
      data_length = sizeof(data_array) / sizeof(int);
      printf("data length: %i\n", data_length);
      for (int i = 0; i < data_length; i++) {
        printf("%i ", data_array[i]);
      }
      printf("\n");
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
