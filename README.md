### CP431 Term ProjectMPI 
### programming for multiplication tables Instructor: 
##### I. Kotsireas, e-mailikotsire@wlu.ca.
- This assignment must be completed on a SHARCnet cluster using MPI C programming.
- Strict adherence to the SHARCNET etiquette (as per the course outline) is required.

### This Term Project is aimed at developing an efficient implementation of a parallel algorithm to compute the different elements in an N x N multiplication table.


### 1  Statement of the problem

- An N x N multiplication table is an N x N array A defined by Aij =i.j, for i, j= 1; . . . ; N. 
- It is clear that an N x N multiplication table is symmetric, so we only need to worry about elements above the main diagonal.

- It is also clear that an N x N multiplication table has the squares 1^2, 2^2,. . .,N^2 as the elements of its main diagonal.
- It can be seen that some elements above the main diagonal of an N x N multiplication table are repeated.
- In order to quantify this (repetition) phenomenon, we define the following function: M(N) = number of different elements in an N x N multiplication matrix.

### 2   To-do list and deliverables
- Verify by hand thatM(5) = 14.
- Verify by hand thatM(10) = 42.
- Design an efficient parallel algorithm to compute the functionM(N) for values of N up to 10^5.
- Pay special attention to theload balancingissues that arise during the computation of M(N).You will need to come up with an effective load balancing scheme, so that the work is as evenlydivided as possible between thepprocessors.
- Run your code on SHARCnet to compile a table of values of the functionM(N) for N= 5, 10,20, 40, 80, 160, 320, 640, 1000, 2000, 8000, 16000, 32000, 40000, 50000, 60000, 70000, 80000,90000, 100000.
