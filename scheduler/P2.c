/* AUTHORSHIP STATEMENT
// Name: Daniel Loyd
// ID  : 951573556
// Proj: CIS 415 Project 1
// This file is my own work with the help of Rosco
// 	Code in lab(bulk_scheduler.c) was used as inspiration(changes such as inverting Run to Stop)
// Internet consulted for deeper understanding of signals
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

int inputArraySize = 100;
int numCommands = 0;
int pbCounter = 0;
char **inputArray = NULL;
struct processBlock *pbArray = NULL;
int Stop = 1;

struct processBlock
{
	char *command;
	char **parsedArgs;
	int numArgs;
	pid_t pid;
};

int CountWords(char *string)
{
//
// countWords will return the number of space seperated strings
// It will only count a word once a non-space character is found after at least one space is found.
// This is to handle the case that there are multiple spaces between words or a space at the end of the line
//	 with no word after it
// example: "hi     how are you " will return 4
//
	int numWords, index;
	numWords = index = 0;
	char temp = string[index];
	int encounteredSpace = 1;
	while(temp != '\0')
	{
		switch(temp)
		{
			case ' ':
				encounteredSpace = 1;
			default:
				if(encounteredSpace)
				{
					numWords++;
					encounteredSpace = 0;
				};
				break;
		}
		index++;
		temp = string[index];
	}
	return numWords;
}

void FreeData()
{
//
// Frees inputArray and all malloc'd data created by MallocData
//
	int argIndex, index;
	if(pbArray != NULL)
	{
		for(index = 0; index < numCommands; index++)
		{
			free(pbArray[index].command);
			for(argIndex = 0; argIndex < pbArray[index].numArgs; argIndex++)
			{
				free(pbArray[index].parsedArgs[argIndex]);
			}
			free(pbArray[index].parsedArgs);
		}
	free(pbArray);
	}
	if(inputArray != NULL)
	{
		for(index = 0; index < numCommands; index++) //number of pb's = number of input lines
		{
			free(inputArray[index]);
		}
	free(inputArray);
	}
}

void handler_usr1(int sig)
{
//
// Let run the child who receives this signal
//
	Stop = 0;
}

char **ParseLine(char *inputString, int numWords)
{
//
// returns the input string parsed into numWords strings
// Stored and returned as a char **
// This memory is malloc'd, must be freed later
// Parses a string until a space, new line, or null char is found, then stores upto this point as a string
//
	int inCounter, retIndex, wordIndex, terminate;
	inCounter = retIndex = wordIndex = terminate = 0;
	char **ret = (char **)malloc(sizeof(char *) * (numWords+1));
	char *wordToStore;
	char wordToAdd[256];
	char temp;
	while(!terminate)
	{
		temp = inputString[inCounter];
		switch(temp)
		{
			case '\0':
				terminate = 1;  //fall through to next case
			case ' ':		//fall through
			case '\n':
				wordToAdd[wordIndex] = '\0';
				wordToStore = strdup(wordToAdd);
				ret[retIndex] = wordToStore;
				retIndex++;
				wordIndex = 0;
				break;
			default:
				wordToAdd[wordIndex] = temp;
				wordIndex++;
				break;
		}
		inCounter++;
	}
	ret[retIndex] = NULL; 
	return ret;
}

void MallocData()
{
//
// malloc out the necessary number of processBlocks
// 	Note: if ReadInput was not called before this function, numCommands = 0
//
	int allocIndex, numWords;
	pbArray = (struct processBlock *)malloc(sizeof(struct processBlock) * numCommands);
	
	for(allocIndex = 0; allocIndex < numCommands; allocIndex++)
	{
		numWords = CountWords(inputArray[allocIndex]);
		pbArray[allocIndex].numArgs = numWords;
		pbArray[allocIndex].parsedArgs = ParseLine(inputArray[allocIndex], numWords);
		pbArray[allocIndex].command = strdup(pbArray[allocIndex].parsedArgs[0]);
		pbArray[allocIndex].pid = -1;
	}
}

void ReadInput(FILE *f_in)
{
//
// Reads input from f_in and stores each line into inputArray
//
	int numChars, inputCounter, charIndex;
	numChars = inputCounter = charIndex = 0;
	size_t bufferSize = 512;
	char *buffer = (char *)malloc(sizeof(char)*bufferSize);
	
	numChars = getline(&buffer, &bufferSize,f_in);
	while(numChars != -1)		//strip the new line and store the line
	{
		while(buffer[charIndex] != '\n')
		{
			charIndex++;
		}
		buffer[charIndex] = '\0'; 	//remove \n
		char *line = strdup(buffer);
		inputArray[numCommands] = line;
		charIndex = 0;
		numCommands++;
		numChars = getline(&buffer, &bufferSize, f_in);
	}
	free(buffer);
	fclose(f_in);
}

void LaunchProcesses()
{
//
// Fork child processes and call execvp
// Children microsleep until they receive SIGUSR1, then they call execvp
//
	for(pbCounter = 0; pbCounter < numCommands; pbCounter++)
	{	
		pbArray[pbCounter].pid = fork();
		if (pbArray[pbCounter].pid < 0)
		{
			perror("ERROR while forking");
			FreeData();
			exit(1);
		}
		if (pbArray[pbCounter].pid == 0)  //if child proccess
		{
			while(Stop) {usleep(1); }
			execvp(pbArray[pbCounter].command, (char * const*)pbArray[pbCounter].parsedArgs);
			perror("ERROR with execvp");
			FreeData();
			exit(1);
		}
	}
}
/*
//Send appropriate signal to child with pid toReceive
*/
void SendRunSig(pid_t toReceive)
{
	kill(toReceive, SIGUSR1);
}
void SendStopSig(pid_t toReceive)
{
	kill(toReceive, SIGSTOP);
}
void SendContSig(pid_t toReceive)
{
	kill(toReceive, SIGCONT);
}
void WaitForOthers()
{
//
// Call wait(NULL) to avoid zombie children
//
	for(pbCounter = 0; pbCounter < numCommands; pbCounter++)
	{
		wait(NULL);
	}
}
int main(int argc, const char **argv)
{
	inputArray = (char **)malloc(sizeof(char *)*inputArraySize);
	FILE *inputStream;
	switch (argc)		//switch to set input and output
	{
		case 2:
			inputStream = fopen(argv[1], "r");
			break;
		default:
			fprintf(stderr, "Un-supported number of arguments.\n");
			return 1;
	}
	if (inputStream == NULL) 		//check for bad files
	{
		perror("ERROR opening input");
		return 1;
	}

	ReadInput(inputStream);
	if(numCommands == 0)  //dont run if empty file
	{
		free(inputArray);
		return 0;
	}
	MallocData();
	signal(SIGUSR1, &handler_usr1);
	LaunchProcesses();

	//Start processes
	for(pbCounter = 0; pbCounter < numCommands; pbCounter++) 
	{
		SendRunSig(pbCounter[pbArray].pid);
	}
	//Stop processes
	for(pbCounter = 0; pbCounter < numCommands; pbCounter++)
	{
		SendStopSig(pbCounter[pbArray].pid);
	}
	//Continue processes
	for(pbCounter = 0; pbCounter < numCommands; pbCounter++)
	{
		SendContSig(pbCounter[pbArray].pid);
	}
	WaitForOthers();

//begin program clean up
	signal(SIGUSR1, SIG_DFL);
	FreeData();
	return 0;
}
