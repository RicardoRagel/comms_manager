#ifndef __EXAMPLEDATA_H__
#define __EXAMPLEDATA_H__ 

#include <time.h>

// Data structure for an example comms channel message
struct ExampleData
{
	struct  timespec timestamp; // Real Time
	bool 	flag;				// Example bool data
	int		i;					// Example int data
	float 	V[3];		    	// Example array data 
};


#endif
