	#include "buffer.h"
	#include "poison_pill.h"


	void send_messages(buffer_t* buffer, msg_t** messages, int size) {
		int i = 0;
		pthread_mutex_lock(&buffer->mutex_i_index);
		for (i=0; i< size-1; i++) 
			put_bloccante(buffer,messages[i]);

		msg_t* poison_pill = msg_init_pill(NULL);
		put_bloccante(buffer,poison_pill);
		pthread_mutex_unlock(&buffer->mutex_i_index);
		pthread_exit(0);
	}

