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
#define ROUNDS 1000
#define TIMESEED 10000

//Zihui WANG
//Lab 4 Question 2
//A0120868B

int farmer_in = 0, cabbage_in = 0, goat_in = 0, wolf_in = 0 ;
sem_t cabbage_goat, goat_wolf ;
int farmer() {
 int i ;
 for ( i = 0 ; i < ROUNDS ; i ++ ) {
	 farmer_in = 1 ; // Farmer notiﬁes the world that he is in
	 
	 // Entry code here (to allow other processes to go in)
	 sem_post(&cabbage_goat); //when farmer enters, gives each semaphore an "extra" slot, allowing both to enter if desired
	 sem_post(&goat_wolf);
	 
	 printf("Farmer is in\n") ;
	 usleep(rand()%TIMESEED) ; // Stay in for a little while
	 
	 // Exit code here (may need to wait; immediate exit may lead to dangerous conditions)
	 sem_wait(&cabbage_goat); //the farmer takes away the "extra" slots, but must wait for such a slot to be free when one of the two leaves
	 sem_wait(&goat_wolf);
	 
	 farmer_in = 0 ; // Farmer notiﬁes the world that he is out
	 printf("Farmer is out\n") ;
	 usleep(rand()%TIMESEED) ;
 }
 return 0 ;
}
int cabbage() {
 int i ;
 for ( i = 0 ; i < ROUNDS ; i ++ ) {
 
	 // Entry code here (wait to avoid dangerous conditions)
	 sem_wait(&cabbage_goat); //only one of the pair can be in at once
	 
	 cabbage_in = 1 ; // Cabbage notiﬁes the world that it went in
	 printf("Cabbage is in\n") ;
	 // Detect dangerous conditions
	 if ( goat_in && ! farmer_in )
	 // This should never execute
	 printf("\"HELP, I'M GONNA BE EATEN!\" the Cabbage says.\n");
	 usleep(rand()%TIMESEED) ; // stay in for a little while
	 cabbage_in = 0 ; // Cabbage notiﬁes the world that it went out
	 
	 // Exit code here (to allow other processes to go in)
	 sem_post(&cabbage_goat);
	 
	 printf("Cabbage is out\n") ;
	 usleep(rand()%TIMESEED) ; // stay out for a little out
 }
 return 0 ;
}
int goat() { int i ;
 for ( i = 0 ; i < ROUNDS ; i ++ ) {
 
	 // Entry code here (to avoid dangerous conditions)
	 sem_wait(&cabbage_goat); //only one of the pair can be in at once
	 sem_wait(&goat_wolf); //only one of the pair can be in at once
	 
	 goat_in = 1 ; // Notify the world that goat is in
	 printf("Goat is in\n") ;
	 // Detect dangerous conditions
	 if ( cabbage_in && ! farmer_in )
	 // This should never execute
	 printf("\"THE CABBAGE LOOKS SO YUMMY!\" the Goat says.\n");
	 if ( wolf_in && ! farmer_in )
	 // This should never execute
	 printf("\"HELP, I'M GONNA BE EATEN!\" the Goat says.\n");
	 usleep(rand()%TIMESEED) ; // stay in for a little while
	 goat_in = 0 ; // Notify the world that goat is out
	 
	 // Exit code here (to let other processes go in)
	 sem_post(&cabbage_goat);
	 sem_post(&goat_wolf);
	 
	 printf("Goat is out\n") ;
	 usleep(rand()%TIMESEED) ; // stay out for a little while
 }
 return 0 ;
}
int wolf() {
 int i ;
 for ( i = 0 ; i < ROUNDS ; i ++ ) {
 
	 // Entry code here (wait to avoid dangerous conditions)
	 sem_wait(&goat_wolf); //only one of the pair can be in at once
	 
	 wolf_in = 1 ; // Notify the world that wolf is in
	 printf("Wolf is in\n") ;
	 // Detect dangerous conditions
	 if ( goat_in && ! farmer_in )
	 // This should never execute
	 printf("\"THE GOAT LOOKS SO YUMMY!\" the Wolf says.\n");
	 usleep(rand()%TIMESEED) ; // stay in for a little while
	 wolf_in = 0 ; // Notify the world that wolf is out
	 
	 // Exit code here (to let other processes go in)
	 sem_post(&goat_wolf);
	 
	 printf("Wolf is out\n") ; 
	 usleep(rand()%TIMESEED) ; // Stay out for a little while
 }
 return 0 ;
}
int main ( int argc, char ** argv ) {
 int i, (*child[4])() ;
 void * stack ;
 // initialize the array of processes
 child[0] = farmer ; child[1] = cabbage ; child[2] = goat ; child[3] = wolf ;
 srand(time(NULL)); // initialize the random seed
  // start all processes in the array
  
 if (sem_init(&cabbage_goat,1,1) < 0 ||  sem_init(&goat_wolf,1,1) < 0)
{ // check initialize error
	 perror(NULL) ; 
	 return 1 ; 
}

 
 for ( i = 0 ; i < 4 ; i ++ ) { if ( (stack = malloc(STACKSIZE)) == NULL ) {
 printf("Memory allocation error\n") ;
 exit(1) ;
 }
 int c = clone(child[i],stack+STACKSIZE-1,CLONE_VM|SIGCHLD,NULL) ;
 if ( c < 0 ) {
 perror(NULL) ;
 exit(1) ;
 }
 }
 // Wait for all children to terminate
 for ( i = 0 ; i < 4 ; i ++ ) {
 wait(NULL) ;
 }
 return 0 ;
}