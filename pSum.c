/******************************************************************************
 * Filename: pSum.c
 * Author Kevin Aud
 * Description:
 * 		Uses child processes to sum the integers in a binary file. 
 *
 * Dates Modified:
 * 		04/24/2016 - File created
 *****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <pthread.h>
#include <fcntl.h>
#include <stdio.h>

/*******************************************************************************
 * Name:			stringLength
 * Description:		determines the length of a character array
 *
 * Input:
 * 		char*		character array to determine the length of 
 *
 * Output:
 * 		return		length of array
 ******************************************************************************/
int stringLength(char* source)
{
	char c;
	int len;
	len = 0;
	c = source[0];

	while(c != '\0')
	{
		len++;
		c = source[len];
	}

	return len;
}

/*******************************************************************************
 * Name:			stringCopy
 * Description:		returns pointer to copy of array
 *
 * Input:
 * 		char*		string to copy 
 *
 * Output:
 * 		char*		copy of string
 ******************************************************************************/
char* stringCopy(char* source)
{
	char* dest;
	int sourceLen;
	sourceLen = stringLength(source);

	dest = malloc(sizeof(char) * (sourceLen + 1));

	int i;
	for(i = 0; i < sourceLen; i++)
	{
		dest[i] = source[i];
	}
	dest[sourceLen] = '\0';

	return dest;
}

/*******************************************************************************
 * Name:			processing_function
 * Description:		sums up the integers in a  binary array file using a thread
 *
 * Input:
 * 		fhandle		File handle to a previously opened binary file
 *
 * Output:
 * 		return		the sum of the part of the array that was processed by this 
 * 					thread
 ******************************************************************************/
int processing_function(int fhandle){
	int sum;
	sum = 0;

	int buffer[1];
	while(read(fhandle, buffer, sizeof(int)) != 0){
		flock(fhandle, LOCK_EX);

		sum += buffer[0];

		flock(fhandle, LOCK_UN);
	}	

	return sum;	
}

int main(int argc, char* argv[]){
	
	if(argc != 3){
		exit(1);
	}

	char* 	filename; 
	char* 	numProcessStr;
	int		numProcesses;

	filename = stringCopy(argv[1]);

	numProcessStr = stringCopy(argv[2]);
	numProcesses = atoi(numProcessStr);
	free(numProcessStr);

	int	fhandle;

	fhandle = open(filename, O_RDONLY);

	if(fhandle == -1){
		exit(1);
	}

	free(filename);

	int sum;
	sum = 0;

	pid_t *pids;
	pids = malloc(sizeof(pid_t) * numProcesses);

	int i;
	for(i = 0; i < numProcesses; i++){
		pids[i] = fork();

		if(pids[i] == 0){
			sum += processing_function(fhandle);		
			return sum;
		}
	}

	free(pids);
	int status;

	for(i = 0; i < numProcesses; i++){
		wait(&status);			
		sum += WEXITSTATUS(status);
	}

	printf("Sum: %i\n", sum);	

	return 0;
}



















