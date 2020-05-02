#ifndef __EXAMPLEMSG_H__
#define __EXAMPLEMSG_H__ 

// Example communication channel class for memory reserve and data struct
#include "CommsManager.hpp"
#include "example_data.h"

// CommsManager derived class
class ExampleMsg : public CommsManager 
{
  public:

    //Constructor
	ExampleMsg(const char *channel_name) : CommsManager(channel_name, &data, sizeof(ExampleData))
	{
		// Initial time
		clock_gettime(CLOCK_REALTIME, &data.timestamp);

		// By default, set to -1 and false
		for(int i=0; i<8; i++)
			data.V[i]=-1;

		data.flag = false;
	}
																	
    // Data struct
    ExampleData data;
};

#endif
