/* AUTHORSHIP STATEMENT
// Name: Daniel Loyd
// ID  : 951573556
// Proj: CIS 415 Project 1
// This file is my own work with inspiration and help from Roscoe and his bulk_scheduler.c
// StackOverflow consulted for deeper understanding of signals
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
int numProcsCompleted = 0;
int pbCounter = 0;
char **inputArray = NULL;
struct processBlock *pbArray = NULL;
int Stop = 1;

enum state{READY, RUNNING, STOPPED, EXITED};
struct processBlock
{
	char *command;
	char **parsedArgs;
	int numArgs;
	pid_t pid;
	enum state status;
};

/*DATA FUNCTIONS*/
int CountWords(char *string)
{
//
// countWords will return the number of space seperated strings
// It will only count a word once a non-space character is found after at least one space is found.
// This is to handle the case that there are multiple spaces between words or a space at the 
//	 end of the line with no word after it
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

void ReadInput(FILE *f_in)
{
//
// Reads input from f_in line by line
// Stores each line into gloabal inputArray
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

/*ALARM FUNCTIONS*/
void SetAlarm()
{
//
// Sets an alarm for 1 second
//
	alarm(1);
}

/*SIGNAL FUNCTIONS*/
void SendRunSig()
{
//
// Sends signal to process specified by pbCounter to start running
// Updates status accordingly
//
	kill(pbArray[pbCounter].pid, SIGUSR1);
	pbArray[pbCounter].status = RUNNING;
}
void SendStopSig()
//
// Sends signal to process specified by pbCounter to stop
// Updates status accordingly
//
{
	kill(pbArray[pbCounter].pid, SIGSTOP);
	pbArray[pbCounter].status = STOPPED;
}
void SendContSig()
//
// Sends signal to process specified by pbCounter to continue running
// Updates status accordingly
//
{
	kill(pbArray[pbCounter].pid, SIGCONT);
	pbArray[pbCounter].status = RUNNING;
}

/*PROGRAM FUNCTIONS*/
void MallocData()
{
//
// Allocate out the number of commands read in from input file
// Note: this function depends on readInput being run first, else numCommands = 0
//
	int allocIndex, numWords;
	pbArray = (struct processBlock *)malloc(sizeof(struct processBlock) * numCommands);
	
	for(allocIndex = 0; allocIndex < numCommands; allocIndex++)
	{
		numWords = CountWords(inputArray[allocIndex]);
		pbArray[allocIndex].status = READY;
		pbArray[allocIndex].numArgs = numWords;
		pbArray[allocIndex].parsedArgs = ParseLine(inputArray[allocIndex], numWords);
		pbArray[allocIndex].command = strdup(pbArray[allocIndex].parsedArgs[0]);
		pbArray[allocIndex].pid = -1;
	}
}
void FreeData()
{
//
// Frees all heap memory allocated by MallocData and the inputArray
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
void LaunchProcesses()
{
//
// Launches all processes in process blocks
// Children created by fork must wait until the parent sends them a signal before execvp'ing
// in the event of a forking or execvp error, all memory is free'd before program exits
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
			while(Stop);
			execvp(pbArray[pbCounter].command, (char * const*)pbArray[pbCounter].parsedArgs);
			perror("ERROR with execvp");
			FreeData();
			exit(1);
		}
	}
}

void ReadStat(int pid)
{
	char procFile[20];
	char currWord[64];
	int wordCounter = 1;
	FILE *procRead = NULL;
	sprintf(procFile, "/proc/%d/stat", pid);
	procRead = fopen(procFile, "r");
	if(procRead == NULL)
	{
		char err[53];
		sprintf(err, "ERROR reading process information for pid: %d", pid);
		perror(err);
		return;
	}
	printf("process with pid %d has:\n", pid);
	while(fscanf(procRead, "%s ", currWord) != EOF)
	{
		switch (wordCounter)
		{
			case 14:
				printf("\tRun for %s clockticks\n", currWord);
				break;
			case 23:
				printf("\tUsed %s bytes in memory\n", currWord);
				break;
			case 42:
				printf("\tHas waited for IO for %s clock ticks\n", currWord);
				break;
			default:
				break;
		}
		wordCounter++;
	}
	fclose(procRead);
}

void RunProcess()
{
//
// Sends SIGUSR1 to a child if the child is in status READY
// Sends SIGCONT to a child if the child is in status STOPPED
// Sets alarm for 1 second
// child that recieves the signal is the one at global counter pbCounter
//
	int curr_state = pbArray[pbCounter].status;
	switch(curr_state)
	{
		case READY:
			SendRunSig();
			break;
		case STOPPED:
			SendContSig();
			break;
		default:
			break;
	}
	SetAlarm();
}
void WaitForOthers()
{
//
// Main process waits for all its children to run and finish
//
	while(numProcsCompleted < numCommands)
	{
		usleep(1);	
	}
}

/*SIGNAL HANDLERS*/
void handler_usr1(int sig)
{
//
// Allow child to run
//
	Stop = 0;
}
void handler_chld(int sig)
{
//
// checks if the child at the global counter is done. If it is, set status to indicate it's done 
// waitpid with option WNOHANG will return the processes pid only if the process is done
//	 else it returns pid 0.
//
	if(pbArray[pbCounter].pid == waitpid(pbArray[pbCounter].pid, NULL, WNOHANG))
	{
		printf("child with pid %d has finished, no more status reports will be given for this process.\n", pbArray[pbCounter].pid);
		pbArray[pbCounter].status = EXITED;
		numProcsCompleted++;
	}
}
void handler_alrm(int sig)
{
//
// When SIGALRM is caught, ensure the current process is not complete, if it isn't stop it
//	check this because SendStopSig will incorrectly set status if its done
// Then find another valid process and run it
//
	if(pbArray[pbCounter].status == RUNNING)
	{
		SendStopSig();
		ReadStat((int)pbArray[pbCounter].pid);
	}
	do
	{
		pbCounter = (pbCounter+1)%numCommands;
	}
	while(pbArray[pbCounter].status == RUNNING || pbArray[pbCounter].status == EXITED);
	RunProcess();
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
	if (numCommands == 0) //don't run if empty file 
	{
		free(inputArray);
		return 0;
	}
	MallocData();
	signal(SIGCHLD, &handler_chld);		//Subcribe to all signals
	signal(SIGUSR1, &handler_usr1);
	signal(SIGALRM, &handler_alrm);

	LaunchProcesses();
	pbCounter = 0;		//Start at first process
	RunProcess();		//Begin scheduling
	WaitForOthers();	//Wait till done, MCP work from here on is in chld and alrm handlers

//begin program clean up
	signal(SIGALRM, SIG_DFL);
	signal(SIGUSR1, SIG_DFL);		//unsubscribe to all signals
	signal(SIGCHLD, SIG_DFL);
	FreeData();
	return 0;
}
