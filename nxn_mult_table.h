typedef struct
{
    long row_col_length;
    long long **data;    
} nxn_mult_table;

/**
 * Reserves memory for a problem_size * problem_size multiplication table
 * and fills it with values
 * 
 * @param mult_table Pointer to the multiplication table
 * @param problem_size The uniform row and column size for the multiplcation table
 */
void init_mult_table(nxn_mult_table *mult_table;

/**
 * Frees all memory associated with an nxn_mult_table
 * 
 * @param mult_table Pointer to the multiplication table
*/
void free_mult_table(nxn_mult_table *mult_table);