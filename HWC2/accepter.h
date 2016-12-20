	#ifndef ACCEPTER_H
	#define ACCEPTER_H

	#include <stdio.h>
	#include <stdlib.h>
	#include "list.h"
	#include "buffer.h"
	#include "reader.h"

	typedef struct acc {
		list_t* readers;
		unsigned int num_readers;
		pthread_mutex_t mutex_readers;
		pthread_cond_t list_readers_not_empty;
		pthread_mutex_t mutex_num_readers;
	} accepter_t;

	void add_reader(accepter_t*, buffer_t*);
	accepter_t* accepter_init();
	#endif