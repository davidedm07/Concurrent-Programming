	#include "accepter.h"
	
	// creates a reader and puts it in the list 
	void add_reader(accepter_t* accepter, buffer_t* requests) {	
		pthread_t reader_thread;
		
		while (1) {
			printf("ACCEPTER THREAD: Taking reader's name\n");
			//get the message from the buffer of the requests
			msg_t* reader_name = get_bloccante(requests);
			printf("Message processed = %s\n", (char*)reader_name->content);
			// if the message read it's the end string it means there are no more
			//readers to create
			if (strcmp((char*)reader_name->content,"end")==0)
				break;
			//initialize a reader
			reader_t* reader = reader_init((char*)reader_name->content);

			pthread_mutex_lock(&accepter->mutex_readers);
			//update the number of readers
			pthread_mutex_lock(&accepter->mutex_num_readers);
			accepter->num_readers = accepter->num_readers + 1;
			pthread_mutex_unlock(&accepter->mutex_num_readers);
			// put the the new reader on the list
			addElement(accepter->readers, reader);
			pthread_mutex_unlock(&accepter->mutex_readers);
			
			if (pthread_create(&reader_thread,NULL,get_messages,reader)) {
				printf("ACCEPTER THREAD: error creating accepter thread."); abort();
			}
		}
		printf ("ACCEPTER THREAD: Readers created = %d\n",accepter->num_readers);
		pthread_cond_signal(&accepter->list_readers_not_empty);
		
	}

	// initialization of the struct accepter 
	accepter_t* accepter_init() {
		accepter_t* accepter = (accepter_t*)malloc(sizeof(accepter_t));
		list_t* readers = list_init();
		accepter->num_readers = 0;
		accepter->readers = (list_t*)malloc(sizeof(list_t));
		accepter->readers = readers;
		pthread_mutex_init(&accepter->mutex_readers,NULL);
		pthread_mutex_init(&accepter->mutex_num_readers,NULL);
		pthread_cond_init(&accepter->list_readers_not_empty,NULL);
		return accepter;
	}