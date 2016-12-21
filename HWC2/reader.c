	#define _GNU_SOURCE
	#include "reader.h"
	#include <pthread.h>

	void* get_messages(void* reader) {
		// get the reader passed as parameter (void pointer)
		reader_t* reader1 = (reader_t*)reader;
		// get the id of the thread
		pthread_t pthread_id = pthread_self();
		reader1->pthread_id = pthread_id;
		printf("READER THREAD: Thread Id: %d \n", (int)reader1->pthread_id);
		// set the name of the thread
		pthread_setname_np(pthread_id,reader1->name);
		char thread_name[40];
		int i= pthread_getname_np(pthread_id, thread_name,40);
		printf("Result pthread get name = %d\n",i);
		printf("READER THREAD: Reader's Name  = %s\n",thread_name);
		msg_t* msg;
		printf("\n");
		// get messages from the reader buffer until it receives a poison pill
		while (1) {
			msg = get_bloccante(reader1->buffer);
			printf("Reader %d got message = %s\n",(int)reader1->pthread_id, (char*)msg->content);
			if (msg == POISON_PILL) {
				printf("poison pill received\n");
				break;
			}
			
		}
		//end the thread
		printf("READER THREAD %d : Terminating Reader\n",(int)reader1->pthread_id);
		free(reader1);
		pthread_exit(0);

	}

	//initialize the reader 
	reader_t* reader_init(char* name) {
		reader_t* reader = (reader_t*)malloc(sizeof(reader_t));
      	reader-> name = name;
      	buffer_t *reader_buffer = buffer_init(N);
      	reader->buffer = reader_buffer;
      	reader->failures = 0;

	}
