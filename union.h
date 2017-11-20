#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Simple struct to hold subarray indices and displacements used in parallel array intersection.
typedef struct 
{
    long long *subarray_a_lengths;
    long long *subarray_b_lengths;
    long long *subarray_a_indices;
    long long *subarray_b_indices;
} array_info;

/**
  Runs a binary search on a given array and returns the index of the last occurrence of num_to_find if it exists in the array.
  
  @param array Pointer to the array to search
  @param num_to_find The value to search for in the array
  @param array_size The length of the array to search
  
  @return index_found The index of the last occurrence of num_to_find, or -1 if num_to_find does not exist in the array
*/
long long binary_search(const long array[], const long num_to_find, const long long array_size);

/**
  Partitions an array into subarrays, each containing either a list of indicies or a list of displacements
  of two arrays to merge.
 
  @param array_size The length of the array
  @param arr_a The first array to merge
  @param arr_b The second array to merge
  @param num_processors The number of designated processors
  @param data A structure to hold the indicies and displacements based on arr_a and arr_b
*/
void partition_data(const long long array_size, const long *arr_a, const long *arr_b, const int num_processors, array_info *data);

/**
  Determines the union of two sub-arrays and stores the results in a third sub-array
  Ex: a = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }
      b = { 2, 2, 3, 4, 5, 6, 7, 7, 8, 9 }
      c = { 2, 3, 4, 5, 6, 7, 8, 9 }
 
  @param sub_arr_a Pointer to the first array to intersect
  @param sub_arr_b Pointer to the second array to intersect
  @param size_a Length of sub_arr_a
  @param size_b Length of sub_arr_b
  @param sub_arr_c Pointer to the array to store the sorted intersection of sub_arr_a and sub_arr_b
*/
void get_union(const long *sub_arr_a, const long *sub_arr_b, const long long size_a, const long long size_b, long *sub_arr_c);