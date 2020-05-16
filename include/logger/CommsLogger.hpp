/************************************************* 
 * Description:
 * ------------
 * General purpose logging class for the CommsManager
 * 
 * Author:
 * -------
 * Ricardo Ragel de la Torre 
 * 	E-mail: 	ricardoragel4@gmail.com
 *  GitHub:		RicardoRagel
*************************************************/ 

#ifndef __COMMSLOGGER_H__
#define __COMMSLOGGER_H__ 

#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>

#include "CommsManager.hpp"

// Derive class of Communication Manager
class CommsLogger : public CommsManager 
{
  public:
  
    // Default Constructor
    CommsLogger(void);
    
    // Constructor. Args: the name of the channel to be logged
    CommsLogger(const char *channelName); 
    
    // Destructor
    ~CommsLogger(void);
    
    // Initialize logger. Args: the channel name list
    bool initLogger(const char *channelName);
    
    // Start logging data
    bool startLog(const char *fileName);
    
    // Stop logging data
    bool stopLog(void);
  
  private:
    					
    // Logging thread
    static void *logThread(void *params);
       
    // Thread handler
    pthread_t thHandler;	
    
    // Log file
    FILE *pFile;
    
    // Information about the channel
    CBbData chInfo;
    
    // Data struct
    char *data;
    
    // Running thread flag
    bool running;
    
    // Flag to avoid save the first channel data because maybe it's	old 
    bool first_read;				
};

#endif

