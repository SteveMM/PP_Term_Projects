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
#define SETBIT(A,k) ( A[(k/32)] |= (1 << (k%32)) )
#endif

#ifndef CLEARBIT
#define CLEARBIT(A,k) ( A[(k/32)] &= ~(1 << (k%32)) ) 
#endif

#ifndef TESTBIT

#define TESTBIT(A,k) ( A[(k/32)] & (1 << (k%32)) )  
#endif

// Constants
static const int ROOT = 0;

// Send/Recv Tags
static const int TAG_CHUNK_SIZE = 0;
static const int TAG_MATRIX_CHUNK_DATA = 1;
static const int TAG_BIT_MAP = 1;

// Global Counter
static long long counter = 0LL;

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
  unsigned long long chunk_sizes[num_processors];
  for (int i = 0; i < num_processors; ++i)
  {
      chunk_sizes[i] = floor((float) cells / num_processors);
    
      if (i < cells % num_processors)
        chunk_sizes[i] += 1;
  }

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
  printf("\nproducts: ");
  while (my_chunk > 0) 
  {
    product = i * j;
    if (!TESTBIT(visited_bit_map, product)) {
      SETBIT(unique_bit_map, product);
      SETBIT(visited_bit_map, product);
      printf("set %lli ", product);
    } else {
      CLEARBIT(unique_bit_map, product);
      printf("clear %lli ", product);
    }
    i++; my_chunk--;
    if (i == (table_size + offset)) 
    {
      j++; i = j;
    }
  }

  printf("\n");

  MPI_Barrier(MPI_COMM_WORLD);
  if (process_rank != ROOT)
    MPI_Send(unique_bit_map, n, MPI_INT, ROOT, TAG_BIT_MAP, MPI_COMM_WORLD);

  if (process_rank == ROOT) 
  { 
    int final_bit_map[n];
    for (int i = 0; i < n; i++) {
      visited_bit_map[i] = 0;
      final_bit_map[i] = 0;
    }

    for (int i = 0; i <= num_values; i++) {
      printf("looking at: %i", i);
      if ((TESTBIT(final_bit_map, i) ^ TESTBIT(unique_bit_map, i)) && !(TESTBIT(visited_bit_map, i))) {
        printf("...set.");
        SETBIT(final_bit_map, i);
        SETBIT(visited_bit_map, i);
      }
      printf("\n");
    }

    printf("\n");

    for (int rank = 1; rank < num_processors; ++rank) 
    {
        int incoming_bit_map[n];
        MPI_Recv(incoming_bit_map, n, MPI_INT, rank, TAG_BIT_MAP, MPI_COMM_WORLD, NULL);

        for (int i = 0; i <= num_values; i++) {
          printf("looking at: %i", i);
          if ((TESTBIT(final_bit_map, i) ^ TESTBIT(incoming_bit_map, i)) && !(TESTBIT(visited_bit_map, i))) {
            printf("...set.");
            SETBIT(final_bit_map, i);
            SETBIT(visited_bit_map, i);
          } else {
            CLEARBIT(final_bit_map, i);
          }
          printf("\n");
        }
        printf("\n");
    }
        printf("\nunique: ");
        for (long long int i = 0; i <= num_values; i++) {
          if (TESTBIT(final_bit_map, i)) {
            printf("%lli ", i);
            counter++;
          }
        }
      printf("\nvalues: %lli, counter: %lli\n", num_values, counter);
  }

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
