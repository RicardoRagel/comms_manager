#ifndef __EXAMPLEDATA_H__
#define __EXAMPLEDATA_H__ 

#include <time.h>

// Data structure for an example comms channel message
struct ExampleData
{
	struct  timespec timestamp; // Real Time
	float 	V[3];		    	// Example array data 
	bool 	flag;				// Example bool data
};


#endif
