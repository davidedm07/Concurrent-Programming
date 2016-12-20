	#ifndef MESSAGE_H  
	#define MESSAGE_H
	#include <stdio.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include <string.h>
	

	typedef struct msg {
		void* content; // generico contenuto del messaggio
		struct msg * (*msg_init)(void*); // creazione msg
		void (*msg_destroy)(struct msg *); // deallocazione msg
		struct msg * (*msg_copy)(struct msg *); // creazione/copia msg
	} msg_t;

	void msg_destroy(msg_t*);
	msg_t* msg_copy(msg_t*);
	msg_t* msg_init(void*);
	void msg_destroy_string(msg_t*);
	msg_t* msg_copy_string(msg_t*);
	msg_t* msg_init_string(void*);

	#endif