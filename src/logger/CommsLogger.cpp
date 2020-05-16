// General purpose logging class 
#include "CommsLogger.hpp"

CommsLogger::CommsLogger(void)
{
	data = NULL;
	running = false;
	pFile = NULL;
	chInfo.id = -1;
	chInfo.size = 0;
	chInfo.channel[0] = '\0';
}    
	
CommsLogger::CommsLogger(const char *channelName)
{
	initLogger(channelName);
}    

CommsLogger::~CommsLogger(void)
{
	if(data != NULL)
		delete []data; 
}
		
bool CommsLogger::initLogger(const char *channelName)
{
	strcpy(chInfo.channel, channelName);
	chInfo.id = -1;
	chInfo.size = 0;
	data = NULL;
	running = false;
	pFile = NULL;
	
	return true;
}
	
bool CommsLogger::startLog(const char *fileName)
{
	int rc;
	
	// Quit if we are logging
	if(running)
		return false;
	
	// Create log's folder at home folder
	char folder[300];
	sprintf (folder, "%s/.comms_log_files", getenv("HOME"));
    if (!mkdir(folder, ACCESSPERMS))
	{
        printf("[CommsLogger] Directory created: %s\n", folder); 
	}

	// Create log file
	char PathFilename[300];
	sprintf (PathFilename, "%s/%s", folder, fileName);
	
	// Open file for logging
	pFile = fopen(PathFilename, "wb");
	if(pFile == NULL)
	{
		printf("ERROR: impossible to create %s file\n", PathFilename);
		return false;
	}
	
	// Launch log thread
	running = true;
	rc = pthread_create(&thHandler, NULL, logThread, this); 
	if(rc)
		return false;
	
	return true;
}
	
bool CommsLogger::stopLog(void)
{
	void *retVal;
	
	running = false;
	pthread_join(thHandler, &retVal);
	fclose(pFile);
	
	return true;
}
	
void * CommsLogger::logThread(void *params)
{
	CBbData chInfo;
	CommsLogger *pClass = (CommsLogger *)params;
	
	// Get the channel information
	do
	{
		chInfo = pClass->getChannelInfo(pClass->chInfo.channel);
		usleep(250000);
	}while(chInfo.channel[0] == '\0');
	
	pClass->chInfo = chInfo;
	
	// Allocate memory for data buffer
	if(pClass->data != NULL)
		delete []pClass->data; 
	pClass->data = new char[chInfo.size];
	
	// Initialize channel
	pClass->initChannel(chInfo.channel, pClass->data, chInfo.size);
	
	// Save channel header in file
	fwrite(chInfo.channel, 1, 256, pClass->pFile);
	fwrite(&chInfo.size, sizeof(int), 1, pClass->pFile);
	
	pClass->first_read = true;
	
	// Start log in file
	while(pClass->running)
	{
		pClass->read(true);
		if(!(pClass->first_read))
		{
			fwrite(pClass->data, chInfo.size, 1, pClass->pFile);
		}
		else
		{
			pClass->first_read = false;
		}
	}
	return NULL;
}