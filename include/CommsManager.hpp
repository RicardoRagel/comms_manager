/************************************************* 
 * Description:
 * ------------
 * Class to create a channel, and to point it in the Bb,
 * or to subscribe to it, creating (only at the first 
 * creating channel) the Bb, if not exist, and all 
 * necessary semaphores.
 * 
 * Author:
 * -------
 * Ricardo Ragel de la Torre 
 * 	E-mail: 	ricardoragel4@gmail.com
 *  GitHub:		RicardoRagel
*************************************************/ 

#ifndef __COMMSMANAGER_HPP__
#define __COMMSMANAGER_HPP__

#include "SharedMemory.hpp"

// BlackBoard (Bb) data struct 
struct CBbData                   											
{
	char 	channel[256];      	// Channel name (= Shared memory reference)
	int		id;					// Channel id (Bb: 0, Channels: 1~100). The Bb is other special shared memory											
	int		size;				// Size of channel/SharedMemory in bytes 												
};
																		    
#define BLACKBOARD_ID 		0       										
#define BLACKBOARD_MAX_CH 	100                                             
#define BLACKBOARD_SIZE 	BLACKBOARD_MAX_CH*sizeof(CBbData)+sizeof(int)   
// The Bb will have in the first position one integer which value is the actual channel number pointed in the Bb and space to 100 channels (save as CBbData).

class CommsManager															
{
public:
    
    // Constructor. Just initialize blackboard and semaphores.
    // initChannel(...) must be used to initialize channel
	CommsManager(void);
	
	// Constructor. Args: Channel name (as a path), pointer to the data where write 
	// or read the data channel, and the data channel size in bytes
	CommsManager(const char *channel, void *data, int size);
									
	// Destructor
	~CommsManager(void);
	
	// Initialize a channel. Channel name (as a path), pointer to the data where write 
	// or read the data channel, and the data channel size in bytes
	bool initChannel(const char *channel, void *data, int size);
	
	// Write to the channel
	bool write(void);

	// Read from the channel.
	// Arg: true for blocking read, waiting to new data.
	bool read(bool block = false);
	
	CBbData getChannelInfo(const char *channel);

private:
	
	// Wait until receive a synch
	void waitSynch(void);
	
	// Send a synch 
	void synch(void);
	
	// BlackBoard shared memory
	SharedMemory bb;
	 
	// Channel shared memory
	SharedMemory ch;
	
	// Pointer to the data to read or write
	void *pData;
	
	// Identification data channel (as a Bb struct) 
	CBbData chData;
	
	// Semaphore variables for channel synchronization
    int   semid;		// Id semaphores set (unique id for the channel group)
    key_t key;			// Key for the semaphores set
    int   lastCount;	// Variable for the blocking read
};

#endif
