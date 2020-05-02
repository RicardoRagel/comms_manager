/*****************************************************
 * Description:
 * ------------
 * Just an example about how to use the CommsManager
 * library to publish into a specified Channel 
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

int main(int argc, char *argv[])
{
	// Set the running flag and prepare the Ctrl+C handler 
	isRunning=true;
	signal(SIGINT, ctrlCHandler);
	
	// Create the example comms channel
	std::string channel_name = "/example_channel/data";
	ExampleMsg msg(channel_name.c_str());

	// Control time cycle variables
	double rate = 5.0; // Hz
	struct timeval timer;
	double InitialTime;
	double ElapseTime;
	// 0 sec
	timer.tv_sec = 0;
	timer.tv_usec = 0;

	printf("Example Comms Channel Publisher writing to messages through the channel: %s\n", channel_name.c_str());
	
	while(isRunning)
	{
		// Sleep up to 0.1 sec and save the initial cycle time
		select(0, NULL, NULL, NULL, &timer);
		InitialTime = GetTime();
		
		// Fill message with some random numbers
		for (int i=0; i<8; i++)
		{
			// Save as message (range 12.6)
			msg.data.V[i] = i;
		}
		msg.data.flag = !msg.data.flag;
		
		// Fill timestamp with actual time
		clock_gettime(CLOCK_REALTIME, &msg.data.timestamp);
		
		// Send message into the channel
		msg.write();
		
		// Save the elapsed time since the cycle began
		ElapseTime = GetTime() - InitialTime;

		// 5Hz (rate) -> 0.2 sec = 200000 microsec
		timer.tv_sec = 0;
		timer.tv_usec = (1e6/rate) - ElapseTime;
	}
	
	return 0;
}
