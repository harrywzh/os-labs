// Lab 3 Question 1
// Zihui WANG
// A0120868B
// Lab Time Mon 10:00-12:00

// Compute the nth fibonacci number by spawning new 
// processes instead of performing recursive calls. 
// Each child sends its partial result to the parent 
// via a pipe. 
 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h> 
#include <errno.h> 
 
int main(int argc, char **argv) 
{ 
 int n = atoi(argv[1]) ; // read argument from command line 
 int superdaddy = getpid() ; 
 int p[2] ; // pipe 
 int q[2]; // 2nd pipe
 
 int output, savedoutput, result1, result2, result, cpid ; 
 output = STDOUT_FILENO ; 
 while ( 1 ) 
 { // every new child must go through the while body again 
	 if ( n < 2 ) 
	 { // base case 
		 if ( write(output,&n,sizeof(n)) < 0 ) { 
		 perror(NULL) ; 
		 return 1 ; 
		 } 
		if ( getpid()==superdaddy ) // prints for input of 1
		printf("Result: %d\n", 1) ;
		 return 0 ; 
	 } 
	 
	 savedoutput = output ; // useful when 2 parent spawns two children 
	 n -- ; // argument for child 
	 
	 if ( pipe(p) < 0 || pipe(q) < 0 ) 
	 //using short circuit || is okay as if no error on p, then q will always be evaluated
	 { // build pipe to return result 
		 perror(NULL) ; 
		 return 1 ; 
	 } 
	
	 output = p[1] ; // useful when parent spawns 2 children 
	 cpid = fork() ; // create child to compute fib(n-1) 
	 if ( cpid == 0 ) 
	 { 
		 if (close(p[0])<0 || close(q[0])<0 ) 
		 { // close unused pipe ends 
			 perror(NULL) ; 
			 return 1 ; 
		 } 
		 //printf("This is child one, n is %d",n);
		 continue ; 
	 } 
	 else if (cpid<0) 
	 { 
		 perror(NULL) ; 
		 return 1 ; 
	 }
	 else 
	 { //parent continues to spawn 2nd child
		n -- ;  //use pipe q and n-2 for 2nd child
		output = q[1] ;
		cpid = fork() ; // create child to compute fib(n-2) 
		if ( cpid == 0 ) 
		{ 
			 if (close(p[0])<0 || close(q[0])<0 ) 
			 { // close unused pipe ends 
				 perror(NULL) ; 
				 return 1 ; 
			 } 
			// printf("This is child one, n is %d",n);
			 continue ; 
		 } 
		 else if (cpid<0) 
		 { 
			 perror(NULL) ; 
			 return 1 ; 
		 }
	 
	 }
	
	 
	 
	 if (close(p[1])<0 || close(q[1])<0) 
	 { // close unused pipe ends 
		 perror(NULL) ; 
		 return 1 ; 
	 } 
	 break ; 
 } 
 if ( read(p[0],&result1,sizeof(result1)) < 0 ) 
 { 
	 perror(NULL) ; 
	 return 1 ; 
 } 
 if ( read(q[0],&result2,sizeof(result2)) < 0 ) 
 { 
	 perror(NULL) ; 
	 return 1 ; 
 } 
 result = result1+result2; //fib(n) = fib(n-1)+fib(n-2) 
 
 if ( getpid()==superdaddy ) // the original parent prints the result 
 printf("Result: %d\n", result) ; 
 else if ( write(savedoutput,&result,sizeof(result)) < 0 ) { 
 // each process will send the computed partial result to its own parent
 // the appropriate pipe p or q is used based on which child 
 perror(NULL) ; 
 return 1 ; 
 } 
 wait(NULL) ; // wait for the child to terminate (avoid zombies) 
 return 0 ; 
} 
 
