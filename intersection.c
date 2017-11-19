#include "intersection.h"

long long binary_search(const long array[], const long num_to_find, const long long array_size)
{
    long long first_index = 0LL;
    long long last_index = array_size - 1LL;
    long long midpoint = floor((first_index + last_index) / 2LL);
  
    // Assume error case initially
    long long index_found = -1LL;
  
    while (first_index <= last_index)
    {
        if (array[midpoint] < num_to_find)
        {
            first_index = midpoint + 1LL;
        }
        else if (array[midpoint] == num_to_find)
        {
            index_found = midpoint;
            first_index = midpoint + 1LL;
        }
        else
        {
            last_index = midpoint - 1LL;
        }
      
        midpoint = floor((first_index + last_index) / 2LL);
    }
  
    return midpoint;
}

void partition_data(const long long array_size, const long *arr_a, const long *arr_b, const int num_processors, array_info *data)
{
    // initialize arrays to return data
    int *arr_a_size, *arr_b_size, *arr_a_indices, *arr_b_indices;
    
    // malloc space fore each array
    arr_a_size = (long long*)malloc(num_processors * sizeof(long long));
    arr_b_size = (long long*)malloc(num_processors * sizeof(long long));
    arr_a_indices = (long long*)malloc(num_processors * sizeof(long long));
    arr_b_indices = (long long*)malloc(num_processors * sizeof(long long));
    
    // determine the remainder to be used for load balancing
    long long remainder = array_size % num_processors;
    
    // determines the sizes required for each subarray of array a
    // calculated by dividing array length by # of processors
    // case 1: if no remainder of division, add value to arr_a_size array
    // case 2: if remainder, add value + 1 to arr_a_size and decrement remainder
    // result: arr_a_size contains all sub-array displacements of array a
    for (int i = num_processors - 1; i > -1; --i) 
    {
        arr_a_size[i] = array_size / num_processors;
        
        if (remainder > 0LL) 
        {
            arr_a_size[i] += 1LL;
            --remainder;
        } 
    }
  
    // determines the indicies (i_start) for each processor required for array b
    // calculated by adding the array size of processor i and the starting index of processor i and
    // storing this value as the next index of the next processor
    // result: arr_a_indicies full of starting indicies of each sub array of array a
    long long start = 0LL;
    long long index;
    arr_a_indices[0] = start;
    
    for (int i = 1; i < num_processors; ++i) 
    {
        index = arr_a_size[i - 1] + arr_a_indices[i - 1];
        arr_a_indices[i] = index;
    }
    
    // searches for the index of last occuring element in each subarray of b that is equal
    // to or less than the last element of each subarray of a
    // result: arr_b_indicies full of starting index of each sub array of array b and arr_b_size
    // full of the lengths of each sub arry of array b
    long long index_of_key;
    long key;
    long long size;
    
    for (int i = 0; i < num_processors; ++i) 
    {
        index_of_key = arr_a_size[i] + arr_a_indices[i] - 1LL;
        key = arr_a[index_of_key];
        // binary search for the last occurance of key
        // if not found, returns the next closest value below key
        index = binary_search(arr_b, key, array_size);
        
        if (index > 0) 
        {
            // if the index is found, store the starting index for the subarray
            // as well as its size (displacement)
            size = index + 1LL - start;
            arr_b_indices[i] = start;
            arr_b_size[i] = size;
            start = index + 1LL;
        } 
        else if (index == -1LL) 
        {
            arr_b_indices[i] = start;
            arr_b_size[i] = 0LL;
        }
        
        // increment the index by the size of the next subarray of a
        index += arr_a_size[i + 1];
    }
    
    // malloc memory for array_data struct based on the size of each subarray
    size_t size_of_array = (sizeof *data->subarray_a_lengths) * num_processors;
   
    // malloc memory for each attribute of array_data struct
    data->subarray_a_lengths = malloc(size_of_array);
    data->subarray_b_lengths = malloc(size_of_array);
    data->subarray_a_indices = malloc(size_of_array);
    data->subarray_b_indices = malloc(size_of_array);
    
    // initialize array_data struct with appropriate data
    data->subarray_a_lengths = arr_a_size;
    data->subarray_b_lengths = arr_b_size;
    data->subarray_a_indices = arr_a_indices;
    data->subarray_b_indices = arr_b_indices;
}

void get_intersection(const long *sub_arr_a, const long *sub_arr_b, const long long size_a, const long long size_b, long *sub_arr_c)
{
    long long arr_c_index = 0, i = 0, j = 0;
      
    while (i < size_a && j < size_b)
    {
        if (sub_arr_a[i] < sub_arr_b[j])
            ++i;
        else if (sub_arr_a[i] > sub_arr_b[j])
            ++j;
        else
        {
            const long common_element = sub_arr_a[i];

            // Append to sub_arr_c only if the current common element would be unique to the set
            if (binary_search(sub_arr_c, common_element, arr_c_index + 1LL))
                sub_arr_c[arr_c_index++] = common_element;
            
            ++i;
            ++j;
        }
    }
}