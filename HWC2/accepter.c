	#include "accepter.h"
	
	void add_reader(accepter_t* accepter, buffer_t* requests) {
		printf("Accepter buffer when called\n");	
		//buffer_to_string(requests);
		pthread_t reader_thread;
		while (1) {
			
			printf("Taking reader's name\n");
			msg_t* reader_name = get_bloccante(requests);
			printf("Message processed = %s\n", (char*)reader_name->content);
			if (strcmp((char*)reader_name->content,"end")==0)
				break;

			reader_t* reader = reader_init((char*)reader_name->content);
			pthread_mutex_lock(&accepter->mutex_readers);
			pthread_mutex_lock(&accepter->mutex_num_readers);
			accepter->num_readers = accepter->num_readers + 1;
			pthread_mutex_unlock(&accepter->mutex_num_readers);
			addElement(accepter->readers, reader);
			pthread_mutex_unlock(&accepter->mutex_readers);
			
			if (pthread_create(&reader_thread,NULL,get_messages,reader)) {
				printf("error creating accepter thread."); abort();
			}
		}
		printf ("Readers created = %d\n",accepter->num_readers);
		pthread_cond_signal(&accepter->list_readers_not_empty);
		printf("Terminating Accepter\n");
		pthread_exit(0);
	}

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