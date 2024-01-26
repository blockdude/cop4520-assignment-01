# Results and Documentation

To ensure efficiency and proper load balancing I used a ticketing
system. Each thread would take a number to evaluate and when it
was done it would take another number that was available.

To ensure correctness I used atomic variable types and mutex locks for
shared memory. The atomic variable was used for the ticketing
system to ensure that no two threads would be evaluating the same
number. I used a mutex lock for a structure to hold total number
of primes, prime sum, and top ten primes.

I tested my program with different thread counts and prime ranges
to make sure results were consistent and performace was indeed
being improved by use of multiple threads.

Prime range: 10^8
- 1 thread  : ~40 seconds
- 4 threads : ~11 seconds
- 8 threads : ~9  seconds

Prime range: 10^7
- 1 thread  : ~1.5 seconds
- 4 threads : ~0.5 seconds
- 8 threads : ~0.4 seconds


# Building and Compiling

To compile you will need GCC and optionally Make.

## Method 1: using GCC

To compile using GCC navigate to directory where the source file
is then run the command:

> gcc main.cpp -lstdc++ -lm -o main

Or 

> g++ main.cpp -o main

This should generate a binary file named "main".

## Method 2: using Make

To use Make to build this program first navigate to the directory
with the source file then run the command:

> make

This should generate a binary file named "main".


# Runing

To run the program use:

> ./main

Or if you are using Make:

> make run

This should generate a file named "primes.txt" that will contain
execution time ( in seconds ), total number of primes below 10^8, sum of
all primes below 10^8, and the 10 largest primes in said range.
