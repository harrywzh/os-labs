#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>
#define STACKSIZE 10000
#define NUMPROCS 20
#define ROUNDS 10

//Zihui WANG
//Lab 4 Question 1
//A0120868B

int n = 0 ;
sem_t s ;
int child () {
 int i = 0 ;
 for ( ; i < ROUNDS ; i ++ ) {
 // Add your entry protocol here

 //>> Since I am not allowed to initialize the semaphore or create any new global variables
 //>> I must "borrow" n to use as a flag in order to only initialize the semaphore once.
  int constant = NUMPROCS*ROUNDS+10;  //Some number outside the range of possible values
 //>> The first thread that runs will be the "initializing thread", running LINE B, and it will set n to constant so that
 //>> all other threads will be held at LINE A. Since only the first thread has passed LINE A, only it will initialize the 
 //>> semaphore. Then it increments n at LINE C so that all other threads can then proceed. Whichever thread reaches LINE D first
 //>> will restore n to the proper value.
 while (n==constant) usleep(100); //LINE A
 if (n==0) n=constant;  //LINE B
 if (n==constant){
	if (sem_init(&s,1,1) < 0)
	{ // check initialize error
		 perror(NULL) ; 
		 return 1 ; 
	} 
 }
 if (n==constant) n++; //LINE C
 
 sem_wait(&s);
 
 // Start of critical section -- simulation of slow n++
 int tmp = n ;
 int sleeptime = rand()%20000 ;
 if ( sleeptime > 10000 ) usleep(sleeptime) ; n = tmp + 1 ;
 // End of critical section
 

 // Add your exit protocol here
 sem_post(&s);
 if (n>constant) n = n-constant-1; //LINE D
 
 
 }
 return 0 ;
}
int main ( int argc, char ** argv ) {
 int i ;
 void * p ;
 srand(time(NULL));
 for ( i = 0 ; i < NUMPROCS ; i ++ ) {
 p = malloc(STACKSIZE) ;
 if ( p == NULL ) {
 printf("Error allocating memory\n") ;
 exit(1) ;
 }
 // create a child that shares the data segment
 // the stack must be at the top of the allocated area
 int c = clone(child,p+STACKSIZE-1,CLONE_VM|SIGCHLD,NULL) ;
 if ( c < 0 ) {
 perror(NULL) ;
 exit(1) ;
 }
 }
 for ( ; i > 0 ; i -- ) wait(NULL) ;
 printf("n=%d\n",n);
 return 0 ;
}