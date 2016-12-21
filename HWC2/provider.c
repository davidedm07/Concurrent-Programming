	#include "buffer.h"
	#include "poison_pill.h"

	// send messages to a shared buffer with the dispacther
	void send_messages(buffer_t* buffer, msg_t** messages, int size) {
		int i = 0;
		for (i=0; i< size-1; i++) 
			put_bloccante(buffer,messages[i]);
		// send as last message the poison pill to block the computation
		msg_t* poison_pill = msg_init_pill(NULL);
		put_bloccante(buffer,poison_pill);
		
	}
	// send single message into the buffer
	void send_message(buffer_t* buffer, msg_t* message) {
		put_bloccante(buffer,message);
	}

	//send the poison pill into the buffer
	void send_poison_pill(buffer_t* buffer) {
		msg_t* poison_pill = msg_init_pill(NULL);
		put_bloccante(buffer,poison_pill);
	}