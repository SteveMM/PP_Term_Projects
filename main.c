#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "union.h"

// Macro definition(s)
#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef LENGTH
#define LENGTH(a) (sizeof(a) / sizeof((a)[0]))
#endif

// Constants
static const int ROOT = 0;

// Send/Recv Tags
static const int TAG_CHUNK_SIZE = 0;
static const int TAG_MATRIX_CHUNK_DATA = 1;

int main(int argc, char *argv[])
{   
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
  long table_size = atol(argv[1]);
  long long chunk = 0, i_start = 0;
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

  int *data_array = (int *)malloc(sizeof(int) * chunk);

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
  printf("FIRST rank %i chunk: %lli\n", process_rank, chunk);
  
  if (process_rank != ROOT)
  {
    MPI_Send(&chunk, 1, MPI_LONG_LONG, ROOT, TAG_CHUNK_SIZE, MPI_COMM_WORLD);
    MPI_Send(data_array, chunk, MPI_LONG, ROOT, process_rank, MPI_COMM_WORLD);
  }

  if (process_rank == ROOT) 
  {
    for (int rank = 1; rank < num_processors; ++rank) 
    {
        long long next_array_chunk_size;
        MPI_Recv(&next_array_chunk_size, 1, MPI_LONG_LONG, rank, TAG_CHUNK_SIZE, MPI_COMM_WORLD, NULL);
        
        long *next_proc_array = malloc(sizeof(next_array_chunk_size) * sizeof(long));
        MPI_Recv(next_proc_array, next_array_chunk_size, MPI_LONG, rank, TAG_MATRIX_CHUNK_DATA, MPI_COMM_WORLD, NULL);
        
        printf("rank %i chunk: %lli : ", rank, next_array_chunk_size);
        
        for (long long i = 0; i < next_array_chunk_size; i++) 
          printf("%lli ", next_proc_array[i]);
        
        printf("\n");
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
