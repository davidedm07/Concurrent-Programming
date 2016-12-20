	#ifndef DISPATCHER_H  
	#define DISPATCHER_H
	
	#include "buffer.h"
	#include "accepter.h"
	#include "list.h"

	void dispatch_messages(accepter_t*,buffer_t*);

	#endif