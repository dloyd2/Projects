/* AUTHORSHIP STATEMENT
// Name: Daniel Loyd
// ID  : 951573556
// Proj: CIS 415 Project 1
// This file is entirely my own work
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int inputArraySize = 100;
int numCommands = 0;
int pbCounter = 0;
char **inputArray = NULL;
struct processBlock *pbArray = NULL;

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
// Frees inputArray and all memory allocated by MallocData
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
// Malloc's all memory needed for processBlocks
// Intializes values of processBlocks
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
// Reads input from f_in line by line and stores it into inputArray
// Counts and stores the number of lines in the file in numCommands
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
// Launches all commands stored in processBlocks. 
// Leaves the program if an error is encountered in forking or execvp'ing
// 	Free's all memory in this event
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
			execvp(pbArray[pbCounter].command, (char * const*)pbArray[pbCounter].parsedArgs);
			perror("ERROR with execvp");
			FreeData();
			exit(1);
		}
	}
}

void WaitForOthers()
{
//
// call wait(NULL) for each child process to avoid zombie children
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
	if(numCommands ==0) //don't run if empty file
	{
		free(inputArray);
		return 0;
	}
	MallocData();
	LaunchProcesses();
	WaitForOthers();

//begin program clean up
	FreeData();
	return 0;
}
