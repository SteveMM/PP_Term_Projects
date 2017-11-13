#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "nxn_mult_table.h"

// Constants
static const int FIRST = 0;

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
      if (process_rank == FIRST)
        fprintf(stderr, "ERROR: Missing multiplication table size.\nProgram Usage: mult_table <size>\n");  
    }
    else
    {
      MPI_Status status;
      MPI_Request request;

      int process_rank;
      int num_processors;

      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);
      MPI_Comm_size(MPI_COMM_WORLD, &num_processors);

      if (process_rank == FIRST)
      {
          nxn_mult_table mult_table;
          mult_table.row_col_length = argv[2];
          init_mult_table(&mult_table);

          // Distribute segments of mult_table's data to the other processes
      }
      else
      {
          // Search for unique mult_table elements within each process
      }
    }

    MPI_Finalize();
    return 0;
}