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
 int q[2] ;
int child () {
 int i = 0 ;
 for ( ; i < ROUNDS ; i ++ ) {
 // Add your entry protocol here
int a = 1;
	if ( read(q[0],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
	}  
 // Start of critical section -- simulation of slow n++
 int tmp = n ;
 int sleeptime = rand()%20000 ;
 if ( sleeptime > 10000 ) usleep(sleeptime) ; n = tmp + 1 ;
 // End of critical section
 // Add your exit protocol here
	if ( write(q[1],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
	} 
 
 }
 return 0 ;
}
int main ( int argc, char ** argv ) {
 int i ;
 void * p ;
 srand(time(NULL));
if ( pipe(q) < 0 )
{ // build and initialize pipe with one object
	 perror(NULL) ; 
	 return 1 ; 
} 
int a = 1;
if ( write(q[1],&a,sizeof(a)) < 0 ) { 
		perror(NULL) ; 
		return 1 ; 
} 
//Maintain pipe to have 1 or 0 objects to act as semaphore
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
 if ( read(q[0],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
	}
if (close(q[0])<0 || close(q[1])<0) 
	{ // close unused pipe ends, short circuit || ensures all will execute if no error
		 perror(NULL) ; 
		 return 1 ; 
	 }
 printf("n=%d\n",n);
 return 0 ;
}