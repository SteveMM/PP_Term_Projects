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

#ifndef SETBIT
#define SETBIT(A,k) (A[(k/32)] |= (1 << (k%32)))
#endif

#ifndef CLEARBIT
#define CLEARBIT(A,k) (A[(k/32)] &= ~(1 << (k%32))) 
#endif

#ifndef TESTBIT
#define TESTBIT(A,k) (A[(k/32)] & (1 << (k%32)))
#endif

// Constants
static const int ROOT = 0;

// Send/Recv Tags
static const int TAG_CHUNK_SIZE = 0;
static const int TAG_MATRIX_CHUNK_DATA = 1;
static const int TAG_BIT_MAP = 1;

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
  long long int num_values = table_size * table_size;
  const int cells = ((num_values / 2) + ceil((float) table_size / 2));

  // Calculate each processors chunk, the 
  // number of cells this processor will calculate
  //chunk = floor(cells / num_processors);
  unsigned long long chunk_sizes[num_processors];
  for (int i = 0; i < num_processors; ++i)
  {
      chunk_sizes[i] = floor((float) cells / num_processors);
      // printf("(float) cells / num_processors) %f\n", (float) cells / num_processors);
    
      if (i < cells % num_processors)
        chunk_sizes[i] += 1;
  }

  printf("chunk sizes: %llu\n", chunk_sizes[process_rank]);
  long long *data_array = (long long *)malloc(sizeof(long long) * chunk_sizes[process_rank]);

  // Calculate all (i,j) indicies for each process to start at
  const int offset = 1;
  const int start = 1;
  long long my_chunk = chunk_sizes[process_rank];
  
  long long end = 0LL;
  
  for (int i = 0; i < process_rank; ++i)
  {
     end += chunk_sizes[i];
  }
  
  unsigned long long i = start;
  unsigned long long j = start;
  while (end > 0)
  {
      end--; my_chunk--; i++;
      if (i == (table_size + offset)) 
      {
          j++; i = j;
      }
  }

  my_chunk = chunk_sizes[process_rank];
  long long index = 0;

  const int n = ceil(num_values / sizeof(int));
  int unique_bit_map[n];
  int visited_bit_map[n];

  for (int i = 0; i < n; i++) {
    unique_bit_map[i] = 0;
    visited_bit_map[i] = 0;
  }

  long long int product = 0;

  while (my_chunk > 0) 
  {
    // data_array[index++] = i * j;
    product = i * j;
    if (!TESTBIT(visited_bit_map, product)) {
      SETBIT(unique_bit_map, product);
      SETBIT(visited_bit_map, product);
    } else {
      unique_bit_map[i] = 0;
    }
    i++; my_chunk--;
    if (i == (table_size + offset)) 
    {
      j++; i = j;
    }
  }

  MPI_Barrier(MPI_COMM_WORLD);
  if (process_rank != ROOT)
  {
    // MPI_Send(&chunk_sizes[process_rank], 1, MPI_LONG_LONG, ROOT, TAG_CHUNK_SIZE, MPI_COMM_WORLD);
    // MPI_Send(data_array, chunk_sizes[process_rank], MPI_LONG_LONG, ROOT, TAG_MATRIX_CHUNK_DATA, MPI_COMM_WORLD);
    MPI_Send(unique_bit_map, n, MPI_INT, ROOT, TAG_BIT_MAP, MPI_COMM_WORLD);
  }

  if (process_rank == ROOT) 
  { 
    // const int n = table_size * table_size;
    // int bit_map[n];

    // for (int i = 0; i < n; i++)
    //   bit_map[i] = 0; 

    // for (int i = 0; i < chunk_sizes[0]; i++) {
    //   if (!(TESTBIT(bit_map, data_array[i]))) {
    //     counter++;
    //     SETBIT(bit_map, data_array[i]);
    //   }
    // }

    for (int rank = 1; rank < num_processors; ++rank) 
    {
        // long long next_array_chunk_size;
        // MPI_Recv(&next_array_chunk_size, 1, MPI_LONG_LONG, rank, TAG_CHUNK_SIZE, MPI_COMM_WORLD, NULL);  
      
        // long *next_proc_array = (long *) malloc(next_array_chunk_size * sizeof(long));
        // MPI_Recv(next_proc_array, next_array_chunk_size, MPI_LONG_LONG, rank, TAG_MATRIX_CHUNK_DATA, MPI_COMM_WORLD, NULL);
        int incoming_bit_map[n];
        MPI_Recv(incoming_bit_map, n, MPI_INT, rank, TAG_BIT_MAP, MPI_COMM_WORLD, NULL);

        printf("a: ");
        for (long long int i = 0; i < num_values; i++) {
          printf("%i", unique_bit_map[i] & 0x01);
        }
        printf("\n");

        printf("b: ");
        for (long long int i = 0; i < num_values; i++) {
          printf("%i", incoming_bit_map[i] & 0x01);
          unique_bit_map[i] = unique_bit_map[i] ^ incoming_bit_map[i];
        }
        printf("\n");

        printf("c: ");
        for (long long int i = 0; i < num_values; i++) {
          printf("%i", unique_bit_map[i] & 0x01);
        }
        printf("\n");
        
        
        // for (long long i = 0; i < next_array_chunk_size; i++) {
        //   if (!(TESTBIT(bit_map, next_proc_array[i]))) {
        //     counter++;
        //     SETBIT(bit_map, next_proc_array[i]);
        //   }
        // }

        // free(next_proc_array);
    }
        printf("d: ");
        for (long long int i = 0; i < num_values; i++) {
          printf("%i", unique_bit_map[i] & 0x01);
          if (TESTBIT(unique_bit_map, i)) {
            counter++;
          }
        }
      printf("values: %lli, counter: %lli\n", num_values, counter);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  free(data_array);
  
  MPI_Finalize();
  return 0;
}
