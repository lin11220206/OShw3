#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char *argv[])
{	
	if(argc != 4)
    {
        printf("please enter ./hw3 [fileName] [\"keyword\"] [numOfProcess]\n");
        return -1;
    }

	char *fileName = argv[1];
	char *keyword = argv[2];
	int pNum = atoi(argv[3]);
	int fileSize = 0;
	FILE *fp;

	fp = fopen(fileName, "r");
	if(fp == NULL)
	{
		printf("file doesn't exist!\n");
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	if(fileSize <= 1)
	{
		printf("empty file\n");
		return -1;
	}
	fileSize--;
	rewind(fp);
	
	char *buffer;	
	pid_t pid;
	int id = 0, i, j, sum = 0;	
	int *count = malloc(sizeof(int)*pNum);
	int *displs = malloc(sizeof(int)*pNum);
	int *result;
	result = mmap(NULL, sizeof(int)*pNum, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	memset(result, 0, sizeof(int)*pNum);

	for(i=0; i<pNum; i++)
	{
		count[i] = fileSize/pNum;
		if(i < fileSize%pNum) 
			count[i]++;
	}
	displs[0] = 0;
	for(i=1; i<pNum; i++)
		displs[i] = displs[i-1] + count[i-1];

	for(i=1; i<pNum; i++)
	{
		displs[i] -= (strlen(keyword) - 1);
		count[i] +=	(strlen(keyword) -1 );
	}

	buffer = mmap(NULL, sizeof(char)*fileSize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	fread(buffer, sizeof(char), fileSize, fp);	
	fclose(fp);	

	for(i=0; i<pNum; i++)
	{
		pid = fork();
		if(pid == 0)
		{
			char *s = malloc(sizeof(char)*count[i]+1);
			strncpy(s, buffer+displs[i], count[i]);

			if(strncmp(s, keyword, strlen(keyword)) == 0)
				result[i]++;				

			char *start = s;
			while(start = strstr(start+1, keyword))
			{
				if(start == NULL)
					break;				
				result[i]++;
			}						

			free(s);
			exit(0);
		}
	}
	
	for(i=0; i<pNum; i++)
		wait(NULL);

	printf("Result:\n");
	for(i=0; i<pNum; i++)
	{
		printf("process[%d]: %d\t", i+1, result[i]);
		sum += result[i];
	}

	printf("\ntotal: %d\n", sum);
	
	return 0;
	
}
