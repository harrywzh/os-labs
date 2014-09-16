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
#define NUMPROCS 5
#define ROUNDS 10

//Zihui WANG
//Lab 4 Question 3
//A0120868B

int n = 0 ;
sem_t cs1, cs2, cs3, cs4, cs5;
int child () {
 int i = 0 ;
 int m = ++n; //assign a number to each process
 for ( ; i < ROUNDS ; i ++ ) {
	 // Add your entry protocol here
	if (m==1){
		sem_wait(&cs1); //pick up lower chopstick first
		sem_wait(&cs5);		
	} else if (m==2){
		sem_wait(&cs1);
		sem_wait(&cs2);
	} else if (m==3){
		sem_wait(&cs2);
		sem_wait(&cs3);
	} else if (m==4){
		sem_wait(&cs3);
		sem_wait(&cs4);
	} else if (m==5){
		sem_wait(&cs4);
		sem_wait(&cs5);
	} else {
		printf("Too many philosopers\n");
		exit(1);
	}
	 // Start of critical section -- simulation of slow n++
	 printf("Philosopher %d is eating\n", m);
	 int sleeptime = rand()%20000 ;
	 if ( sleeptime > 10000 ) usleep(sleeptime) ;
	 // End of critical section
	 

	 // Add your exit protocol here
	if (m==1){
		sem_post(&cs5);
		sem_post(&cs1);
	} else if (m==2){
		sem_post(&cs1);
		sem_post(&cs2);
	} else if (m==3){
		sem_post(&cs2);
		sem_post(&cs3);
	} else if (m==4){
		sem_post(&cs3);
		sem_post(&cs4);
	} else if (m==5){
		sem_post(&cs4);
		sem_post(&cs5);
	} 
 
 }
 return 0 ;
}
int main ( int argc, char ** argv ) {
 int i ;
 void * p ;
 srand(time(NULL));
 if (sem_init(&cs1,1,1) < 0 || sem_init(&cs2,1,1) < 0 || sem_init(&cs3,1,1) < 0 || sem_init(&cs4,1,1) < 0 || sem_init(&cs5,1,1) < 0)
{ // check initialize error
	 perror(NULL) ; 
	 return 1 ; 
}
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
 //printf("n=%d\n",n);
 return 0 ;
}