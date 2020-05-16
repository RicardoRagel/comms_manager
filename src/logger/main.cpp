/***********************************************************
 * logger/main.cpp
 * 
 * Save channels data in binary files .log in /logger/log_files
 * 
 * USAGE: ./comms_logger </channel/name/1> </channel/name/2> ...
 * 
 * Note: The first data always are the channel name as a char[256]
 * and the channel size as a integer. Needed to can reproduce it
 * 
************************************************************/

#include "CommsLogger.hpp"
#include <signal.h>
#include <unistd.h>
#include <ctype.h>

//!Ctrl+C handler
volatile bool isRunning = true;
void ctrlCHandler(int sig) 
{    
    if (sig == SIGINT)
    {
        isRunning = false;
	}
}

int main(int argc, char *argv[])
{
	int i, j, n_channels;
	CommsLogger *loggers;
	char filename[256], ch[256];
	struct timespec t;
	
	// Set the running flag and prepare the Ctrl+C handler 
    isRunning=true;
    signal(SIGINT, ctrlCHandler);
    
    // Create the loggers
    n_channels = argc-1;
    loggers = new CommsLogger[n_channels];
    
    // Start data logs
    clock_gettime(CLOCK_REALTIME, &t);
    for(i=0; i <n_channels; i++)
	{
		// Initialize logger
		loggers[i].initLogger(argv[i+1]);
		
		// Create the name of the log file based on channel name and current time
		j=0;
		while(argv[i+1][j] != '\0')
		{
			if(isalnum(argv[i+1][j]))
				ch[j] = argv[i+1][j];
			else
				ch[j] = '_';
			j++;
		}
		ch[j] = '\0';
		sprintf(filename, "%s_%lu.log", ch, t.tv_sec);
		
		// Start data log
		loggers[i].startLog(filename);
	}
    
	// Wait until process ends
	while(isRunning)
	{
		usleep(250000);
	}
	
	// Stop loggers and quit
	printf("Stopping loggers...\n");
	for(i=0; i <n_channels; i++)
		loggers[i].stopLog();
	printf("done!\n");

    return 0;
}



