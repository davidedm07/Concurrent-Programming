	#ifndef BUFFER_H  
	#define BUFFER_H
	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <string.h>
	#include "message.h"
	#include "poison_pill.h"

	#define BUFFER_ERROR (msg_t *) NULL 

	typedef struct {
		msg_t** messages;
		unsigned int e_index; // index for the extraction of the messages
		unsigned int i_index; // index for the insertion of the messages
		unsigned int occupied;
	 	unsigned int size;
	 	pthread_mutex_t mutex_i_index;
		pthread_mutex_t mutex_e_index;
		pthread_cond_t buffer_not_full;
		pthread_cond_t buffer_not_empty;
		pthread_mutex_t mutex_occupied;
	}buffer_t;

	int increase_index_buffer(unsigned int, unsigned int);
	buffer_t* buffer_init(unsigned int);
	void buffer_destroy(buffer_t*); 
	msg_t* put_bloccante(buffer_t*, msg_t*); 
	msg_t* put_non_bloccante(buffer_t*, msg_t*);
	msg_t* get_bloccante(buffer_t*);
	msg_t* get_non_bloccante(buffer_t*);
	void buffer_to_string(buffer_t*);
	#endif