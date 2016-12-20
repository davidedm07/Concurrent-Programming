	#ifndef HWC1_H  
	#define HWC1_H

	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <string.h>

	#define BUFFER_ERROR (msg_t *) NULL 

	typedef struct msg {
		void* content; // generico contenuto del messaggio
		struct msg * (*msg_init)(void*); // creazione msg
		void (*msg_destroy)(struct msg *); // deallocazione msg
		struct msg * (*msg_copy)(struct msg *); // creazione/copia msg
	} msg_t;

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

	void msg_destroy(msg_t*);
	msg_t* msg_copy(msg_t*);
	msg_t* msg_init(void*);
	void msg_destroy_string(msg_t*);
	msg_t* msg_copy_string(msg_t*);
	msg_t* msg_init_string(void*);
	int increase_index_buffer(unsigned int, unsigned int);
	buffer_t* buffer_init(unsigned int);
	void buffer_destroy(buffer_t*); 
	msg_t* put_bloccante(buffer_t*, msg_t*); 
	msg_t* put_non_bloccante(buffer_t*, msg_t*);
	msg_t* get_bloccante(buffer_t*);
	msg_t* get_non_bloccante(buffer_t*);

	#endif