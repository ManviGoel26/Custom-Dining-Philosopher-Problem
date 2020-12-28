# Custom-Dining-Philosopher-Problem
C language solution to a custom dining philosopher problem using blocking and non-blocking semaphores with pthread mutex.
The problem statement is written in Assignment4.

## Usage
1. The number of philosphers can be changed in the Makefile
2. To run the blocking semaphore, run command `make block` which uses pthread_cond_wait and pthread_cond_signal.
3. To run the Non-blocking semaphore, run command `make nonblock` which uses busy waiting.

## Assignment Details
Course Project for Operating Systems (CSE231) 
Instructor : Dr. Arani Bhattacharya

**Skills Used** : C Programming Language

### References
1. The Little Book of Semaphores, Book by Allen B. Downey
