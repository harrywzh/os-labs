// Lab 3 Question 2
// Zihui WANG
// A0120868B
// Lab Time Mon 10:00-12:00

// Creates process network as shown in question and reads n from command line
 
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
 int q[2] ; // return pipe
 
 int output, savedoutput, input, result, cpid ; 
 //output = STDOUT_FILENO ; 
 if ( pipe(q) < 0 ) 
	 { // build pipe to return result 
		 perror(NULL) ; 
		 return 1 ; 
	 } 
	
 while ( 1 ) 
 { // every new child must go through the while body again 

	 n -- ; // argument for child 
	 
	 if ( pipe(p) < 0 )
	 { // build pipe to connect processes
		 perror(NULL) ; 
		 return 1 ; 
	 } 
	
	 
	 if (n < 0) //in the last process only, do not fork
	 {
		output = q[1] ;  
	 }
	 else 
	 {
		output = p[1] ; 
		cpid = fork() ; 
		 if ( cpid == 0 ) 
		 { 
			input = p[0];
			 if (close(p[1])<0) 
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
		 { 
		 
		 }
	}
	
	 
	 
	if (close(p[0])<0) 
	{ // close unused pipe ends 
		 perror(NULL) ; 
		 return 1 ; 
	 } 
	 break ; 
 } 
 
 
 


 
 if ( getpid()==superdaddy ) // the original parent sends the initial number and the result 
 {
	result = 0;
	if ( write(output,&result,sizeof(result)) < 0 ) { 
	 // each process will send the computed partial result to its own parent
		 perror(NULL) ; 
		 return 1 ; 
	}  
	if ( read(q[0],&result,sizeof(result)) < 0 ) 
	{ 
		 perror(NULL) ; 
		 return 1 ; 
	} 
	if (close(q[0])<0 || close(q[1])<0 || close(output)<0) 
	{ // close unused pipe ends, short circuit || ensures all will execute if no error
		 perror(NULL) ; 
		 return 1 ; 
	 }
	printf("Result: %d\n", result);
 }
 else
 {
	 if ( read(input,&result,sizeof(result)) < 0 ) 
	 { 
		 perror(NULL) ; 
		 return 1 ; 
	 } 
	
		result = result + 1;
		if ( write(output,&result,sizeof(result)) < 0 ) { 
		 // each process will send the computed partial result to its own parent
			 perror(NULL) ; 
			 return 1 ; 
		} 
	
 }
 wait(NULL) ; // wait for the child to terminate (avoid zombies) 
 return 0 ; 
} 
 
