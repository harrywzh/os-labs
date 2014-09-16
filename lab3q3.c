// Lab 3 Question 3
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
 int r[2] ; //initial pipe
 int output, savedoutput, input, result, cpid, exitloop ; 
 //output = STDOUT_FILENO ; 
 if ( pipe(r) < 0 || pipe(q) < 0 ) 
	 { // build pipe to return result 
		 perror(NULL) ; 
		 return 1 ; 
	 } 
input = r[0];
savedoutput = r[1];
result = 0;
if ( write(savedoutput,&result,sizeof(result)) < 0 ) { //write to first pipe
	perror(NULL) ; 
	return 1 ; 
}  

 while ( 1 ) 
 { // every new child must go through the while body again 

	 n -- ;	 
		if ( pipe(p) < 0 )
		 { // build pipe to connect processes
			 perror(NULL) ; 
			 return 1 ; 
		 } 
		output = p[1] ; 
		cpid = fork() ; 
		 if ( cpid == 0 ) 
		 { 
			if (n < 1) output = q[1];
			n = 0;
		 } 
		 else if (cpid<0) 
		 { 
			 perror(NULL) ; 
			 return 1 ; 
		 } 
		 else 
		 {
			exitloop = 0;
			input = p[0];
			 
			//wait(NULL);
		 }

	if (n < 1) break;

 } 
 
 
 


 
 if ( getpid()==superdaddy ) // the original parent sends the initial number and the result 
 {
	
	if ( read(q[0],&result,sizeof(result)) < 0 ) 
	{ 
		 perror(NULL) ; 
		 return 1 ; 
	} 
	if (close(q[0])<0 || close(savedoutput)<0) 
	{ // close unused pipe ends, short circuit || ensures all will execute if no error
		 perror(NULL) ; 
		 return 1 ; 
	 }
	printf("Result: %d\n", result);
	//wait(NULL) ;
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
	if (close(input)<0 || close(output)<0) 
	{ // close unused pipe ends, short circuit || ensures all will execute if no error
		 perror(NULL) ; 
		 return 1 ; 
	 }
	
 }
  // wait is not neccessary as the each process's read (incuding the superdaddy) will block until the write is available 
 return 0 ; 
} 
 
