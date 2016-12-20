	#include "reader.h"
	#include <pthread.h>

	void* get_messages(void* reader) {
		
		reader_t* reader1 = (reader_t*)reader;
		
		pthread_t pthread_id = pthread_self();
		reader1->pthread_id = pthread_id;
		printf("Thread Id: %d \n", (int)reader1->pthread_id);
		pthread_setname_np(pthread_id,reader1->name);
		char* thread_name;
		pthread_getname_np(pthread_id, thread_name);
		printf("Reader's Name  = %s\n",thread_name);
		msg_t* msg;
		while (1) {
			msg = get_bloccante(reader1->buffer);
			printf("Reader %d got message = %s\n",(int)reader1->pthread_id, (char*)msg->content);
			if (msg == POISON_PILL) {
				printf("poison pill received\n");
				break;
			}
			
		}


		printf("Terminating Reader\n");
		pthread_exit(0);

	}

	reader_t* reader_init(char* name) {
		reader_t* reader = (reader_t*)malloc(sizeof(reader_t));
      	reader-> name = name;
      	buffer_t *reader_buffer = buffer_init(N);
      	reader->buffer = reader_buffer;
      	reader->failures = 0;

	}
