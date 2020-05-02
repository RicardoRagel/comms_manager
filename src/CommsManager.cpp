#include "CommsManager.hpp"

CommsManager::CommsManager(void)
{			
	// Create the blakboard shared memory if not exist. After the creation in full of zeros
	bb.init(BLACKBOARD_ID, BLACKBOARD_SIZE, BLACKBOARD_MAX_CH);                            

	// Create a set of BLACKBOARD_MAX_CH semaphores, one for each 
	// channel for the read/write synchronization, by the first process. 
	// Other processes dont need to create the semaphore, they will use 
	// the sem_num corresponding in the semid. 
	key = ftok("/", 1); 															
	if((semid = semget(key, BLACKBOARD_MAX_CH , IPC_CREAT|IPC_EXCL|0666)) == -1)
	{
		// Semaphore probably already exists - try as a client 
		if((semid = semget(key, BLACKBOARD_MAX_CH, 0666)) == -1) 					
			return;
	}
	else
	{
		for(int i=0; i<BLACKBOARD_MAX_CH; i++)
			semctl(semid, i, SETVAL, 1);		// Init all semaphores to 1
	}	
}

CommsManager::CommsManager(const char *channel, void *data, int size)
{
	// Channel number and pointer to channel datas in the Bb
	int *pNChannels;													
	CBbData *pChData;
		
	// Get channel data                                                 
	strcpy(chData.channel, channel);									
	chData.size = size;
	chData.id = -1; 		// If at last of the search id is -1, the channel isn't pointed in the Bb
	pData = data;			
		
	// Create the blakboard shared memory if not exist. After the creation in full of zeros
	bb.init(BLACKBOARD_ID, BLACKBOARD_SIZE, BLACKBOARD_MAX_CH);                            
	
	// Search the channel into the blackboard
bb.lock();
	pNChannels = (int *)bb.pBuf;    				// Total channel number                              
	pChData = (CBbData *)(bb.pBuf+sizeof(int));		// Jump the first field of channel number				
	for(int i=0; i<*pNChannels; i++)
	{			
		// If the channel already exist (same channel name and size data)
		if(strcmp(pChData->channel, chData.channel) == 0 && pChData->size == chData.size) 
		{
			// Take the same channel id (then use the same shared memory)
			chData.id = pChData->id;
		}
		pChData++;
	}
	
	// If the channel doesn't exist
	if(chData.id == -1) 																	
	{
		// Increment channel counter
		(*pNChannels)++; 																	
		
		// Assign it the correlative channel id
		chData.id = *pNChannels; 															
		
		// Save the identification data channel in the Bb  (size without blocking int)
		*pChData = chData;			
	}
bb.unlock();
	

	// Create a set of BLACKBOARD_MAX_CH semaphores, one for each 
	// channel for the read/write synchronization, by the first process. 
	// Other processes dont need to create the semaphore, they will use 
	// the sem_num corresponding in the semid. 
	key = ftok("/", 1); 															
	if((semid = semget(key, BLACKBOARD_MAX_CH , IPC_CREAT|IPC_EXCL|0666)) == -1)
	{
		// Semaphore probably already exists - try as a client 
		if((semid = semget(key, BLACKBOARD_MAX_CH, 0666)) == -1) 					
			return;
	}
	else
	{
		for(int i=0; i<BLACKBOARD_MAX_CH; i++)
			semctl(semid, i, SETVAL, 1);		// Init all semaphores to 1
	}		
	
	// Initialize counter for new data detection
	lastCount = 0;
	
	// Create the communication channel	
	ch.init(chData.id, chData.size+sizeof(int), BLACKBOARD_MAX_CH); 
}
									
CommsManager::~CommsManager(void)
{
	// Don't destroy the semaphore, can be another process using that. You need to reebot the later
	//semctl(semid, 0, IPC_RMID, 0);  		
}

bool CommsManager::initChannel(const char *channel, void *data, int size)
{
	// Channel number and pointer to channel datas in the Bb
	int *pNChannels;													
	CBbData *pChData;
		
	// Get channel data                                                 
	strcpy(chData.channel, channel);									
	chData.size = size;
	chData.id = -1; 		// If at last of the search id is -1, the channel isn't pointed in the Bb
	pData = data;			

	// Search the channel into the blackboard
bb.lock();
	pNChannels = (int *)bb.pBuf;                                  
	pChData = (CBbData *)(bb.pBuf+sizeof(int));		// Jump the channel number				
	for(int i=0; i<*pNChannels; i++)
	{			
		// If the channel already exist (same channel name and size data)
		if(strcmp(pChData->channel, chData.channel) == 0 && pChData->size == chData.size) 
		{
			// Take the same channel id (then use the same shared memory)
			chData.id = pChData->id; 												
		}
		pChData++;
	}
	
	// If the channel doesn't exist
	if(chData.id == -1) 																	
	{
		// Increment channel counter
		(*pNChannels)++; 																	
		
		// Assign it the correlative channel id
		chData.id = *pNChannels; 															
		
		// Save the identification data channel in the Bb  (size without blocking int)
		*pChData = chData;			
	}
bb.unlock();
	
	// Initialize counter for new data detection
	lastCount = 0;
	
	// Create the communication channel	
	ch.init(chData.id, chData.size+sizeof(int), BLACKBOARD_MAX_CH); 
	
	return true;
}
	
bool CommsManager::write(void)
{
	int *pCount;
		
ch.lock();
	pCount = (int *)ch.pBuf;
	(*pCount)++;
	memcpy(ch.pBuf+sizeof(int), pData, chData.size);     
ch.unlock();

	// Send a synch for the processes that are waiting for data in this channel
	synch();
	
	return true;
}

// [default] block = false
bool CommsManager::read(bool block)
{
	int *pCount;
	bool res = false;
	
	// Wait until synch if the user set block to true
	pCount = (int *)ch.pBuf;
	if(block)
		waitSynch();
	
ch.lock();
	pCount = (int *)ch.pBuf;
	if(*pCount != lastCount)
	{
		memcpy(pData, ch.pBuf+sizeof(int), chData.size);
		lastCount=*pCount; 
		res = true;
	}
ch.unlock();
	
	return res;
}
	
CBbData CommsManager::getChannelInfo(const char *channel)
{
	int *pNChannels;													
	CBbData *pChData, info;
	
	info.id = -1;
	info.size = 0;
	info.channel[0] = '\0';
	
bb.lock();
	pNChannels = (int *)bb.pBuf;                                  
	pChData = (CBbData *)(bb.pBuf+sizeof(int));		// Jump the channel number				
	for(int i=0; i<*pNChannels; i++)
	{			
		// If the channel already exist (same channel name)
		if(strcmp(pChData->channel, channel) == 0) 
			info = *pChData; 													
		pChData++;
	}
bb.unlock();

	return info;
}

void CommsManager::waitSynch(void)
{
	struct sembuf sem1 = { chData.id -1, 0, 0}, sem2 = { chData.id -1, 1, 0};	// chData.id = 1 ~ 100, y sem_num  = 0 ~ 99

	if(semop(semid, &sem1, 1) == -1)
	{
		fprintf(stderr, "Wait-synch failed\n");
		exit(1);
	}
	if(semop(semid, &sem2, 1) == -1)
	{
		fprintf(stderr, "Wait-synch failed\n");
		exit(1);
	}		
}
	
void CommsManager::synch(void)
{
	struct sembuf sem1={chData.id - 1, -1, 0};  // chData.id = 1 ~ 100, y sem_num  = 0 ~ 99

	semctl(semid, chData.id - 1, SETVAL, 1);
	if((semop(semid, &sem1, 1)) == -1)
	{
		fprintf(stderr, "Synch failed\n");
		exit(1);
	}
}