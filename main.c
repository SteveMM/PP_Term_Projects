#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

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

// Global Counter
static long long counter = 0LL;

typedef struct {
  long num;
  int count;
} hash_entry;

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
  const int cells = ((table_size * table_size) / 2) + (table_size / 2);

  // TESTING
  if (process_rank == ROOT) 
  {
    printf("Cells: %i\n", cells);
  }

  // Calculate each processors chunk, the 
  // number of cells this processor will calculate
  //chunk = floor(cells / num_processors);
  long long chunk_sizes[num_processors];
  
  for (int i = 0; i < num_processors; ++i)
  {
      chunk_sizes[i] = floor((float) cells / num_processors);
      printf("(float) cells / num_processors) %i", (float) cells / num_processors));
    
      if (i < cells % num_processors)
        chunk_sizes[i] += 1;
  }

  long *data_array = (long *)malloc(sizeof(long) * chunk_sizes[process_rank]);

  // Calculate all (i,j) indicies for each process to start at
  const int offset = 1;
  const int start = 1;
  long long my_chunk = chunk_sizes[process_rank];
  
  long long end = 0LL;
  
  for (int i = 0; i < process_rank; ++i)
  {
     end += chunk_sizes[i];
  }
  
  //long long end = process_rank * my_chunk;
  long long i = start;
  long long j = start;
  while (end > 0)
  {
      end--; my_chunk--; i++;
      if (i == (table_size + offset)) 
      {
          j++; i = j;
      }
  }

  my_chunk = chunk_sizes[process_rank];
  long product;
  long long index = 0;
  while (my_chunk > 0) 
  {
    data_array[index++] = i * j;
    //printf("Product: %lli\n", i * j);
    i++; my_chunk--;
    if (i == (table_size + offset)) 
    {
      j++; i = j;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (process_rank != ROOT)
  {
    MPI_Send(&chunk_sizes[process_rank], 1, MPI_LONG_LONG, ROOT, TAG_CHUNK_SIZE, MPI_COMM_WORLD);
    MPI_Send(data_array, chunk_sizes[process_rank], MPI_LONG, ROOT, TAG_MATRIX_CHUNK_DATA, MPI_COMM_WORLD);
  } // else
  // {
  //   MPI_Isend(&chunk, 1, MPI_LONG_LONG, ROOT, TAG_CHUNK_SIZE, MPI_COMM_WORLD, NULL);
  // }

  if (process_rank == ROOT) 
  { 
    int *hash_map = (int*) calloc(table_size * table_size, sizeof(int));
    printf("rank %i chunk: %lli -> ", process_rank, chunk_sizes[0]);
    for (long long i = 0; i < chunk_sizes[0]; i++) {
      printf("%lli ", data_array[i]);
      counter++;
      hash_map[data_array[i]]++;
      if (hash_map[data_array[i]] == 2) {
        counter--;
      }
    }
    printf("counter: %lli", counter);
    printf("\n");
    for (int rank = 1; rank < num_processors; ++rank) 
    {
        long long next_array_chunk_size;
        MPI_Recv(&next_array_chunk_size, 1, MPI_LONG_LONG, rank, TAG_CHUNK_SIZE, MPI_COMM_WORLD, NULL);  
      
        printf("rank %i chunk: %lli -> ", rank, next_array_chunk_size);
      
        long *next_proc_array = (long *) malloc(next_array_chunk_size * sizeof(long));
        MPI_Recv(next_proc_array, next_array_chunk_size, MPI_LONG, rank, TAG_MATRIX_CHUNK_DATA, MPI_COMM_WORLD, NULL);
        
        for (long long i = 0; i < next_array_chunk_size; i++) {
          counter++;
          hash_map[next_proc_array[i]]++;
          if (hash_map[next_proc_array[i]] == 2) {
            counter--;
          }
          printf("%lli ", next_proc_array[i]);
        }
        printf("counter: %lli", counter);
        printf("\n");
        free(next_proc_array);
    }
      printf("counter: %lli\n", counter);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  free(data_array);
  
  MPI_Finalize();
  return 0;
}
