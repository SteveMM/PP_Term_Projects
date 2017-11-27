#include "mpi.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

// Macro definition(s)
#ifndef SETBIT
#define SETBIT(A,k) ( A[(k/32)] |= (1 << (k%32)) )
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
static unsigned long long counter = 0LL;

int main(int argc, char *argv[])
{   
  int process_rank;
  int num_processors;

  // Initialize MPI
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &num_processors);

  // Check for command line input
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
  const unsigned long long int num_values = table_size * table_size;
  const unsigned long long int cells = ((num_values / 2) + ceil((float) table_size / 2));
  printf("cells: %llu\n", cells);

  // Calculate each processors chunk, the 
  // number of cells this processor will calculate
  unsigned long long chunk_sizes[num_processors];
  for (int i = 0; i < num_processors; ++i)
  {
      chunk_sizes[i] = floor((float) cells / num_processors);
    
      if (i < cells % num_processors)
        chunk_sizes[i] += 1;
  }

  // Define offset paramaters
  const int offset = 1;
  const int start = 1;

  // Initialize each processors chunk
  unsigned long long my_chunk = chunk_sizes[process_rank];
  unsigned long long end = 0LL;
  
  // Determine the ending position for each process
  for (int i = 0; i < process_rank; ++i)
  {
     end += chunk_sizes[i];
  }
  
  // "Fast-forward" each processor to their start location
  unsigned long long i = start;
  unsigned long long j = start;
  while (end > 0)
  {
      --end; --my_chunk; ++i;
      if (i == (table_size + offset)) 
      {
          ++j; i = j;
      }
  }

  // Reinitialize my_chunk after decrement
  my_chunk = chunk_sizes[process_rank];
  printf("Calculating bitmap size\n");
  const unsigned int n = ceil(num_values / 8);
  printf("Bitmap size: %u\n", n);

  // TESTING
  // printf("n: %llu\n", n);
  // printf("malloc: %llu\n", n * sizeof(int));
  printf("Creating bitmap\n");
  // Define a bitmap for each process
  unsigned int *unique_bit_map = (unsigned int*) calloc(n, sizeof(unsigned int));
  if (unique_bit_map == NULL)
      printf("FAILED TO CALLOC BITMAP\n");

  printf("Filling bitmap\n");
  // Set the bit corrisponding to each product as a unique product
  unsigned long long int product = 0LL; 
  while (my_chunk > 0LL) 
  {
    product = i * j;
    SETBIT(unique_bit_map, product);
    // printf("set %lli ", product);
    ++i; --my_chunk;
    if (i == (table_size + offset)) 
    {
      ++j; i = j;
    }
  }

  // printf("\n");

  // Barrier only for pretty test printing 
  // TODO: Remove barrier
  MPI_Barrier(MPI_COMM_WORLD);
printf("Sending bitmaps\n");
  // Each process sends it's unique bitmap to the root process
  if (process_rank != ROOT) 
    MPI_Send(unique_bit_map, n, MPI_INT, ROOT, TAG_BIT_MAP, MPI_COMM_WORLD);
  


  if (process_rank == ROOT) { 
    for (int rank = 1; rank < num_processors; ++rank) {
        // Allocate space for each incoming bitmap
        unsigned int *incoming_bit_map = (unsigned int*) malloc(n * sizeof(unsigned int));

        // Get each unique bitmap from each process to compare against root bitmap
        MPI_Recv(incoming_bit_map, n, MPI_INT, rank, TAG_BIT_MAP, MPI_COMM_WORLD, NULL);
        printf("Incoming bitmap received\n");

        *unique_bit_map |= *incoming_bit_map;

        // If an incoming bitmap contains a unique product that is not yet in
        // the roots unique bitmap, set that bit as unique
        // for (unsigned long long int i = 0LL; i <= num_values; ++i) {
        //   printf("looking at: %llu", i);
        //   if (TESTBIT(incoming_bit_map, i)) {
        //     printf("...set.");
        //     SETBIT(unique_bit_map, i);
        //   } 
        // }
        
        // Free the incoming bitmap space
        free(incoming_bit_map);
        // printf("\n");
    }
        // printf("\nunique: ");
        // Increment the counter for every bit set in the unique bitmap
        printf("Computing sum\n");
        #pragma omp parallel for
        for (unsigned long long int i = 0LL; i <= num_values; ++i) {
          if (TESTBIT(unique_bit_map, i)) {
            // printf("%lli ", i);
            #pragma omp critical
            ++counter;
          }
        }

      // Print the total count
      printf("counter: %llu\n", counter);
  }

  // Free the unique bitmap of each process
  free(unique_bit_map);
  // Finalize MPI
  MPI_Finalize();
  return 0;
}

