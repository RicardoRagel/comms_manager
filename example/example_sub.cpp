/*****************************************************
 * Description:
 * ------------
 * Just an example about how to use the CommsManager
 * library to receive data from a specified Channel 
 * ("/example_channel/data") using a specific message
 * ("msg/example_msg.h"->ExampleMsg)
 * 
 * Author:
 * -------
 * Ricardo Ragel de la Torre 
 * 	E-mail: 	ricardoragel4@gmail.com
 *  GitHub:		RicardoRagel
*****************************************************/

// General includes
#include <stdio.h>
#include <string>
#include <signal.h>

// Example Data Message
#include "msg/example_msg.h"

//!Ctrl+C handler
volatile bool isRunning = true;
void ctrlCHandler(int sig) 
{    
    if (sig == SIGINT)
    {
        isRunning = false;
    }
}

//! Get actual time in microsec
double GetTime(void)
{
	timespec structtime;
	clock_gettime(CLOCK_REALTIME, &structtime);
	double micro = ((structtime.tv_sec)*1e6 + (structtime.tv_nsec)*1e-3); //microsec
	return micro;
}

// Print this program usage
void printUsage(void)
{
	printf("USAGE:\t$./example_sub <BLOCK>\n");
	printf("\tBLOCK = true to make blocking reads or false to make non-blocking reads\n");
}

int main(int argc, char *argv[])
{
	// Check arguments
	bool blocking = false;
	if( argc == 2 ) 
	{
		if(std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")
		{
			printUsage();
			exit(0);
		}
		if(std::string(argv[1]) != "true" && std::string(argv[1]) != "false")
		{
			printf("ERROR:\tUnrecognised argument %s\n", argv[1]);
			printUsage();
			exit(0);	
		}
		else
		{
			if(std::string(argv[1]) == "true")
				blocking = true;
		}
	}
	else if( argc > 2 ) 
	{
		printf("ERROR:\tToo many arguments supplied.\n");
		printUsage();
		exit(0);
	}
	else 
	{
		printf("ERROR:\tOne argument expected.\n");
		printUsage();
		exit(0);
	}

	// Set the running flag and prepare the Ctrl+C handler 
	isRunning=true;
	signal(SIGINT, ctrlCHandler);
	
	// Create the example comms channel
	std::string channel_name = "/example_channel/data";
	ExampleMsg msg(channel_name.c_str());

	// Control time cycle variables
	double rate = 20.0; // Hz
	struct timeval timer;
	double InitialTime;
	double ElapseTime;
	// 0 sec
	timer.tv_sec = 0;
	timer.tv_usec = 0;

	if(blocking)
		printf("Example Comms Channel Blocking Subscriber reading messages from the channel: %s\n\n", channel_name.c_str());
	else
		printf("Example Comms Channel Non-blocking Subscriber reading messages from the channel: %s\n\n", channel_name.c_str());
	
	while(isRunning)
	{
		// Sleep up to 0.1 sec and save the initial cycle time
		select(0, NULL, NULL, NULL, &timer);
		InitialTime = GetTime();
		
		// Fill timestamp with actual time
		clock_gettime(CLOCK_REALTIME, &msg.data.timestamp);
		
		// Read message from the channel
		bool new_msg_exist = msg.read(blocking);

		if(new_msg_exist)
		{
			printf("[%.3f] New message: flag=%s, i=%d, V=[%.2f, %.2f, %.2f]\n", GetTime()/1e6, 
																		  msg.data.flag == false?"false":"true",
																		  msg.data.i,
																		  msg.data.V[0], 
																		  msg.data.V[1], 
																		  msg.data.V[2]);
		}
		else
		{
			printf("[%.3f] No new messages...\n", GetTime()/1e6);
		}

		// Save the elapsed time since the cycle began
		ElapseTime = GetTime() - InitialTime;

		// 5Hz (rate) -> 0.2 sec = 200000 microsec
		timer.tv_sec = 0;
		timer.tv_usec = (1e6/rate) - ElapseTime;
	}
	
	return 0;
}
