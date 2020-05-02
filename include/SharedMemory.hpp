/************************************************* 
 * Description:
 * ------------
 * Class to create a shared memory, with a desired ID 
 * and SIZE, or to subscribe to it if exist, creating 
 * all necessary semaphores.
 *  
 * Author:
 * -------
 * Ricardo Ragel de la Torre 
 * 	E-mail: 	ricardoragel4@gmail.com
 *  GitHub:		RicardoRagel
*************************************************/ 

#ifndef __SHAREDMEMORY_HPP__
#define __SHAREDMEMORY_HPP__

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <ctype.h>
#include <string.h>

class SharedMemory
{
public:
	// Class construtor
	SharedMemory(void)
	{
	}
	SharedMemory(unsigned char memId, unsigned int memSize, unsigned int maxCh = 100)
	{
		init(memId, memSize, maxCh);
	}
	
	// Class destructor
	~SharedMemory(void)
	{
		//shmctl(shmid, IPC_RMID, 0);  		Don't destroy the shared memory (Bb and channels)
		//semctl(semid, 0, IPC_RMID, 0); 	Don't destroy the semaphore
	}
		
	// Init shared memory access
	void init(unsigned char memId, unsigned int memSize, unsigned int maxCh = 100)
	{
		bool newBuff = false;
		
		// Save memId to get acces in rest of function
		memid = memId;                                 // memid = 0(Bb) ~ 101
		
		// Create unique key via call to ftok()
		semkey = ftok("/", 2);
		
		// Create a set of maxCh + 1 semaphore or open it if 
		// already exists (Now, we need semaphores for maxCh 
		// channels and one for the blackboard)
		if((semid = semget(semkey, maxCh + 1, IPC_CREAT|IPC_EXCL|0666)) == -1) 
		{
			// Semaphore probably already exists - try as a client 
			if((semid = semget(semkey, maxCh + 1, 0666)) == -1) 					
				return;
		}
		else
		{
			for(int i=0; i<(maxCh+1); i++)
				semctl(semid, i, SETVAL, 1);	// Init all semaphores to 1	
		}
		
	lock();
		// Open the shared memory segment - create if necessary 
		pBuf = NULL;
		shkey = ftok("/", 3 + memId);
		if((shmid = shmget(shkey, memSize, IPC_CREAT|IPC_EXCL|0666)) == -1) 
		{
			// Segment probably already exists - try as a client 
			if((shmid = shmget(shkey, memSize, 0)) == -1) 
			{
				perror("shmget");
				return;
			}
		}
		else
			newBuff = true;
		
		// Attach (map) the shared memory segment into the current process 
		if((pBuf = (unsigned char *)shmat(shmid, 0, 0)) == (unsigned char *)-1)
		{
			perror("shmat");
			return;
		}
		
		// Set the whole buffer to 0 if new created
		if(newBuff)
		{
			memset(pBuf, 0, memSize);
		}
	unlock();
	}
	
	// Lock the access to the shared memory
	void lock(void)
	{
		struct sembuf sem_lock = { memid, -1, 0};  // memid = 0(Bb) ~ 101, y sem_num  = 0 ~ 101

		if((semop(semid, &sem_lock, 1)) == -1)
		{
			fprintf(stderr, "Lock failed\n");
			exit(1);
		}
	}

	// Unlock the access to the shared memory
	void unlock(void)
	{
		struct sembuf sem_unlock = { memid, 1, 0};

		if(semctl(semid, memid, GETVAL, 0) == 1) 
			return;

		if((semop(semid, &sem_unlock, 1)) == -1)
		{
			fprintf(stderr, "Unlock failed\n");
			exit(1);
		}
	}
	
	// Shared memory pointer
	unsigned char  	*pBuf;
	
private:
	key_t			semkey;		// Semaphore set key
	key_t 			shkey;		// Shared memory key
    int   			shmid;		// Shared memory ID
    int   			semid;		// Semaphore set ID
    unsigned int 	memSize;	// Shared memory size
    int				memid;		// Shared memory ID in the Bb 
};



#endif

