	#ifndef READER_H 
	#define READER_H
	#include "buffer.h"
	#include "poison_pill.h"

	#define N 5 //num messaggi contenuti nel buffer dei reader
	typedef struct read {
		pthread_t pthread_id;
		char* name;
		buffer_t* buffer;
		int failures; // speed of the reader
	}reader_t;

	void* get_messages(void*);
	reader_t* reader_init(char*);
	#endif