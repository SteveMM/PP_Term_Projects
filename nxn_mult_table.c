#include "nxn_mult_table.h"

void init_mult_table(nxn_mult_table *mult_table)
{
    mult_table->data = (long long **)malloc(mult_table->row_col_length * sizeof(long long *));

    #pragma openmp parallel for
    for (int row = 0; row < mult_table->row_col_length; ++row)
    {
        mult_table->data[row] = (long long *)malloc(mult_table->row_col_length * sizeof(long long));
        mult_table->data[row][0] = row + 1;

        for (int col = 1; col < mult_table->row_col_length; ++col)
        {
            mult_table->data[row][col] = mult_table->data[row][0] * (col + 1);
        }
    }
}

void free_mult_table(nxn_mult_table *mult_table)
{
    // This is probably overkill/stupid...
    //#pragma openmp parallel for
    for (int row = 0; row < mult_table->row_col_length; ++row)
        free(mult_table->data[row]);

    free(mult_table);
}