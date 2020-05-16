/************************************************* 
 * Description:
 * ------------
 * General purpose player class for the CommsManager
 * 
 * Author:
 * -------
 * Ricardo Ragel de la Torre 
 * 	E-mail: 	ricardoragel4@gmail.com
 *  GitHub:		RicardoRagel
*************************************************/ 

#ifndef __COMMSPLAYER_H__
#define __COMMSPLAYER_H__ 

#include "CommsManager.hpp"
#include <pthread.h>
#include <unistd.h>

// Derive class of Communication Manager
class CommsPlayer: public CommsManager 
{
  public:

    // Default Constructor
	CommsPlayer(void);

	// Constructor. Args binary file path 
	CommsPlayer(char *binary);
	
	// Destructor
	~CommsPlayer(void);
	
	// Initialization. It create or subscribe to the channel conteined in the binary file header given as argument 
	bool initPlayer(char *binary);
	
	// Start reproduction
	bool startPlayer(void);
	
	// Stop playing data
	bool stopPlayer(void);
	
	// Play all data in the binary file (It can not be stopped by signal interruption)
	void play(void);
	
	// Play only one data from the binary file (for it can be stopped by signal interruption)
	int playone(void);
	
	/// Threads function
	static void *threadfunction(void *params);

  private:
		
	// Channel_name to take from binary file. Fix 256 bytes
	char channel_name[256];
	
	// Channel size. Only the data, without blocking integer 
	int channel_size;
			
	// Binary file name
	char *binary_name;
	
	// Char buffer to reading data from the binary file
	char *PlayerData;
	
	// Pointer to binary file
	FILE *pFile;
	
	// Thread handler
    pthread_t thHandler;
	
	// Channel data for CCommsManager constructor (SIZE)
	CBbData DataChannel;
	
	// Microseconds to simulate the player time
	unsigned long micro;
	unsigned long last_micro;
	
	// Flag to the first read --> dont wait to write
	int first_read;
	
	// Flag to stop the reproduction if do a Ctrl+C in the main process
	bool th_isRunning;
};

#endif
