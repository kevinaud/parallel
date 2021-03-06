/******************************************************************************
 * Filename: tSum.c
 * Author Kevin Aud
 * Description:
 * 		Uses threads to sum the integers in a binary file. 
 *
 * Dates Modified:
 * 		04/24/2016 - File created
 *****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdio.h>

static sem_t lock;

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
 * Name:			thread_function
 * Description:		sums up the integers in a binary array file using a child process
 *
 * Input:
 * 		params		array of params to run the function
 *
 * Output:
 * 		void
 ******************************************************************************/
void *thread_function(void *params){
	int buffer[1];

	while(read(*((int**)params)[0], buffer, sizeof(int)) != 0){
		sem_wait(&lock);

		*((int**)params)[1] += buffer[0];

		sem_post(&lock);
	}	

	pthread_exit(0);
}

int main(int argc, char* argv[]){
	
	if(argc != 3){
		exit(1);
	}

	char* 	filename; 
	char* 	numThreadsStr;
	int		numThreads;

	filename = stringCopy(argv[1]);

	numThreadsStr = stringCopy(argv[2]);
	numThreads = atoi(numThreadsStr);
	free(numThreadsStr);

	int	fhandle;

	fhandle = open(filename, O_RDONLY);

	if(fhandle == -1){
		exit(1);
	}

	free(filename);

	int sum;
	sum = 0;

	pthread_t *thread_ids;
	thread_ids = malloc(sizeof(pthread_t) * numThreads);

	void** params;
	params = malloc(sizeof(void*) * 2);
	params[0] = &fhandle;
	params[1] = &sum;


	sem_init(&lock, 0, 1);

	int success;
	int i;
	for(i = 0; i < numThreads; i++){
		success = pthread_create(&thread_ids[i], NULL, &thread_function, params);		
		if(success != 0){
			exit(1);
		}
	}

	for(i = 0; i < numThreads; i++){
		pthread_join(thread_ids[i], NULL);			
	}

	free(params);	
	free(thread_ids);

	printf("Sum: %i\n", sum);

	return 0;
}











