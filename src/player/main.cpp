/***********************************************************
 * player/main.cpp
 * 
 * USAGE: ./player <_log_name_1> <_log_name_2> ...
 * 
 * Note 1: arguments without extension.
 * Note 2: binary files need to contain the channel name as a char[256] 
 * and the channel size as a integer in the beginning of the file
************************************************************/

#include "CommsPlayer.hpp"
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

//!Ctrl+C handler
volatile bool isRunning = true;
void ctrlCHandler(int sig) 
{    
    if (sig == SIGINT)
    {
        isRunning = false;
	}
}

/// Threads function declaration
void *threadfunction(void *);


int main(int argc, char *argv[])
{
    // Set the running flag and prepare the Ctrl+C handler 
    isRunning=true;
    signal(SIGINT, ctrlCHandler);
    
    // Number of channel channel to play
    int n_channels = argc-1;
    
    // Create the player objects without initialization
	CommsPlayer *players;
	players = new CommsPlayer[n_channels];

	for(int i=0; i <n_channels; i++)
	{	 		
		// Initialization player objects. 
		players[i].initPlayer(argv[i+1]);
		
		// Start reproduction
		players[i].startPlayer();
    }
    
    // Wait until process ends
	while(isRunning)
	{
		usleep(250000);
	}
	
	if(isRunning == false)
	{
		printf("\nStopping reproductions by signal\n");
	}
    
	for(int i=0; i <n_channels; i++)
		players[i].stopPlayer();
   
   return 0;
}


