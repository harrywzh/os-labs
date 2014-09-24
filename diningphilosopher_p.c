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
//Lab 4 Question 3p
//A0120868B

int n = 0 ;
int a = 1;
int count = 0;
int cs1[2], cs2[2], cs3[2], cs4[2], cs5[2];
int child () {
 int i = 0 ;
 int m = ++n; //assign a number to each process
 for ( ; i < ROUNDS ; i ++ ) {
	 // Add your entry protocol here
	if (m==1){
		if ( read(cs1[0],&a,sizeof(a)) < 0 || read(cs5[0],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		} //pick up lower chopstick first		
	} else if (m==2){
		if ( read(cs1[0],&a,sizeof(a)) < 0 || read(cs2[0],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		} 
	} else if (m==3){
	if ( read(cs2[0],&a,sizeof(a)) < 0 || read(cs3[0],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		} 
	} else if (m==4){
		if ( read(cs3[0],&a,sizeof(a)) < 0 || read(cs4[0],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		} 
	} else if (m==5){
		if ( read(cs4[0],&a,sizeof(a)) < 0 || read(cs5[0],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		} 
	} else {
		printf("Too many philosopers\n");
		exit(1);
	}
	 // Start of critical section -- simulation of slow n++
	 printf("Philosopher %d is eating\n", m);
	 count++;
	 int sleeptime = rand()%20000 ;
	 if ( sleeptime > 10000 ) usleep(sleeptime) ;
	 // End of critical section
	 

	 // Add your exit protocol here
	if (m==1){
		if ( write(cs1[1],&a,sizeof(a)) < 0 || write(cs5[1],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		}
	} else if (m==2){
		if ( write(cs1[1],&a,sizeof(a)) < 0 || write(cs2[1],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		}
	} else if (m==3){
		if ( write(cs2[1],&a,sizeof(a)) < 0 || write(cs3[1],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		}
	} else if (m==4){
		if ( write(cs3[1],&a,sizeof(a)) < 0 || write(cs4[1],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		}
	} else if (m==5){
		if ( write(cs4[1],&a,sizeof(a)) < 0 || write(cs5[1],&a,sizeof(a)) < 0 ) { 
			perror(NULL) ; 
			return 1 ; 
		}
	} 
 
 }
 return 0 ;
}
int main ( int argc, char ** argv ) {
 int i ;
 
 void * p ;
 srand(time(NULL));
 if (pipe(cs1) < 0 || pipe(cs2) < 0 || pipe(cs3) < 0 ||pipe(cs4) < 0 || pipe(cs5) < 0)
	{ // check initialize error
		 perror(NULL) ; 
		 return 1 ; 
	}
if ( write(cs1[1],&a,sizeof(a)) < 0 ||  write(cs2[1],&a,sizeof(a)) < 0 || write(cs3[1],&a,sizeof(a)) < 0 || write(cs4[1],&a,sizeof(a)) < 0 || write(cs5[1],&a,sizeof(a)) < 0 ) { 
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
 printf("Total of %d meals eaten\n",count);
if (close(cs1[0])<0 || close(cs1[1])<0 || close(cs2[0])<0 || close(cs2[1])<0 || close(cs3[1])<0) 
	{ // close unused pipe ends, short circuit || ensures all will execute if no error
		 perror(NULL) ; 
		 return 1 ; 
	 }
if (close(cs3[0])<0 || close(cs4[1])<0 || close(cs4[0])<0 || close(cs5[1])<0 || close(cs5[0])<0) 
	{ // close unused pipe ends, short circuit || ensures all will execute if no error
		 perror(NULL) ; 
		 return 1 ; 
	 }
 return 0 ;
}