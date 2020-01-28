#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h> 
#include <queue> 
#include <semaphore.h>

using namespace std;

#define NUM_THREADS 5
#define MEMORY_SIZE 1000

struct node
{
	int id;
	int size;
};

int array[5];
queue<node> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t sharedLock2 = PTHREAD_MUTEX_INITIALIZER; 																		// mutex
pthread_t server; // server thread handle
sem_t semlist[NUM_THREADS]; // thread semaphores
int cap = 1000;
int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size
bool of = true;


void release_function()
{
	of = false;
}

void my_malloc(int thread_id, int size)
{
	node x;
	x.id = thread_id;
	x.size = size;
	pthread_mutex_lock(&sharedLock2);
	myqueue.push(x);
	pthread_mutex_unlock(&sharedLock2);
	

}

void* server_function(void*)
{ 
	
	while (of)
	{
		
		if (!myqueue.empty())
		{
			
			pthread_mutex_lock(&sharedLock2);
			node request;
			request.size = myqueue.front().size;
			request.id = myqueue.front().id;
			myqueue.pop();
			if (cap < request.size)
			{
				
				thread_message[request.id] = -1;

			}
			else
			{

				cap = cap - request.size;
				int a = 0;
				while (memory[a] == '1')
				{
					a++;
				}
				thread_message[request.id] = a;
				for (int i = 0; i < request.size; i++)
				{				
					memory[a] = '1';
					a++;
				}
			}

			array[request.id] = 0;
			pthread_mutex_unlock(&sharedLock2);
		}
		
	}
	

	return 0;
}

void* thread_function(void* id)
{
	
	int* b = (int*)id;
	
	int a = rand() % 1000;
	my_malloc(*b, a);
	array[*b] = 1;
	pthread_mutex_lock(&sharedLock);
	while (array[*b]);
	
	if (thread_message[*b] == -1)
	{
		
		cout << "Thread " << *b << " :  Not  enough memory" << endl;
	}
	pthread_mutex_unlock(&sharedLock);
	
	return 0;
}

void init()
{
	pthread_mutex_lock(&sharedLock);	//lock
	for (int i = 0; i < NUM_THREADS; i++) //initialize semaphores
	{
		sem_init(&semlist[i], 0, 0);
	}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory 
	{
		char zero = '0'; memory[i] = zero;
	}
	pthread_create(&server, NULL, server_function, NULL); //start server 
	pthread_mutex_unlock(&sharedLock); //unlock
}



void dump_memory()
{
	pthread_mutex_lock(&sharedLock);
	for (int i = 0; i < MEMORY_SIZE; i++)
	{
		cout << memory[i];
	}
	pthread_mutex_unlock(&sharedLock);
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

	//You need to create a thread ID array here
	int id = 0;
	int id1 = 1;
	int id2 = 2;
	int id3 = 3;
	int id4 = 4;

	init();	// call init
	
	pthread_t thread1, thread2, thread3, thread4 , thread5;
	pthread_create(&thread1, NULL, thread_function, (void*)& id);
	pthread_create(&thread2, NULL, thread_function, (void*)& id1);
	pthread_create(&thread3, NULL, thread_function, (void*)& id2);
	pthread_create(&thread4, NULL, thread_function, (void*)& id3);
	pthread_create(&thread5, NULL, thread_function, (void*)& id4);
	//You need to create threads with using thread ID array, using pthread_create()

	//You need to join the threads
	pthread_join(thread1, NULL);

	pthread_join(thread2, NULL);

	pthread_join(thread3, NULL);

	pthread_join(thread4, NULL);

	pthread_join(thread5, NULL);

	release_function();
	pthread_join(server, NULL);

	dump_memory(); // this will print out the memory
	printf("\nMemory Indexes:\n");
	for (int i = 0; i < NUM_THREADS; i++)
	{
		printf("[%d]", thread_message[i]); // this will print out the memory indexes
	}
	int x;
	cin >> x;
	printf("\nTerminating...\n");
}