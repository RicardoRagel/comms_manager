// General purpose player class 
#include "CommsPlayer.hpp"

CommsPlayer::CommsPlayer(void)
{
	th_isRunning = false;
	PlayerData = NULL;
	pFile = NULL;
}

CommsPlayer::CommsPlayer(char *binary)
{
	th_isRunning = false;
	pFile = NULL;
	initPlayer(binary);
}

CommsPlayer::~CommsPlayer(void)
{
	if(PlayerData != NULL)
		delete []PlayerData;
}

bool CommsPlayer::initPlayer(char *binary)
{
	// Save binary file name
	binary_name = binary;
	
	// Path to the binary file
	char Filename[256];
	sprintf (Filename, "%s/.comms_log_files/%s.log", getenv("HOME"), binary_name);

	// Open binary file
	pFile = fopen (Filename, "rb");
	if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
	
	// Read channel name and size from binary file (Always in the beginning of the file)
	size_t result = fread (channel_name, 1, 256, pFile);
	if(result != 256) {printf ("Error reading chanel_name from binary file"); exit (1);}
	result = fread ((int *)&channel_size, 1, sizeof(int), pFile);
	if(result != sizeof(int)) {printf ("Error reading data size from binary file"); exit (1);}
	printf("Playing channel: %s\n", channel_name);	
		
	// Book memory to the player data
	PlayerData = new char[channel_size];
		
	// Call CommsManager initialization to create or subscribe to the channel
	initChannel(channel_name, PlayerData, channel_size);
	
	// Init flag for first read
	first_read = 1;
	
	return true;
}

bool CommsPlayer::startPlayer(void)
{
	th_isRunning = true;
	int rc = pthread_create(&thHandler, NULL, threadfunction, this); 
	if (rc)
	{
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		return false;
	}
	return true;
}

bool CommsPlayer::stopPlayer(void)
{
	void *retVal;
	
	th_isRunning = false;
	pthread_join(thHandler, &retVal);
	fclose(pFile);
	
	return true;
}

void CommsPlayer::play(void)
{
	// Number of bytes read.
	int result = channel_size;
	
	// Time written in the message
	struct  timespec timestamp;
	
	// Read and write the first message out of the 'while' to can simulate the firts elapsed time
	result = fread(PlayerData, 1, channel_size, pFile);
	if(result != channel_size) {printf ("Error reading chanel_name from binary file"); exit (1);}
	memcpy (&timestamp, PlayerData, sizeof(timespec));
	last_micro = ((timestamp.tv_sec)*1e6 + (timestamp.tv_nsec)*1e-3); //microsec
	write();
	
	while(result == channel_size)
	{
		// Read binary file
		result = fread(PlayerData, 1, channel_size, pFile);
		if (result == channel_size)
		{
			// First data in each menssage is the time as a timespec
			memcpy (&timestamp, PlayerData, sizeof(timespec));
			//cout << "Time:" << "\n" << "Sec:   " << timestamp.tv_sec << "\n" << "Nsec:   " << timestamp.tv_nsec << "\n";
			micro = ((timestamp.tv_sec)*1e6 + (timestamp.tv_nsec)*1e-3); //microsec
			
			// Wait to simulate elapsed time between messages 
			usleep(micro - last_micro);
							
			// Update last simulated time
			last_micro = micro;
			
			// Write in the channel
			write();
		}
	}
}

int CommsPlayer::playone(void)
{	
	// Number of bytes read.
	int result = channel_size;
	
	// Time written in the message
	struct  timespec timestamp;
	
	// Read binary file
	result = fread(PlayerData, 1, channel_size, pFile);
	
	if (result == channel_size)
	{
		// First data in each menssage is the time as a timespec
		memcpy (&timestamp, PlayerData, sizeof(timespec));
		//cout << "Time:" << "\n" << "Sec:   " << timestamp.tv_sec << "\n" << "Nsec:   " << timestamp.tv_nsec << "\n";
		micro = ((timestamp.tv_sec)*1e6 + (timestamp.tv_nsec)*1e-3); //microsec
		
		if(first_read == 0)
		{
			// Wait to simulate elapsed time between messages 
			usleep(micro - last_micro);
		}
		else
		{
			first_read = 0;
		}
		
		// Update last simulated time
		last_micro = micro;
		
		// Write in the channel
		write();
	}
	
	return result;
}

void *CommsPlayer::threadfunction(void *params)
{	
	// Pointer to THIS class object
	CommsPlayer *pClass = (CommsPlayer *)params;
	
	// Bytes read
	int result = 1; // different to zero
	
	while((pClass->th_isRunning) && (result!=0))
	{
		// Play binary data file in the channel
		result = pClass->playone();
	}
	if(pClass->th_isRunning)
	{  
		printf("%s.log reproduccion completed\n", pClass->binary_name);
	}
	// Thread exit
	pthread_exit(NULL);
}