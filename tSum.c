#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>

#include <stdio.h>
#include "functions.h"

static sem_t lock;

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

	printf("Sum: %i\n", sum);

	return 0;
}

