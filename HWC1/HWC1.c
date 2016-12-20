	#include "HWC1.h"


	void msg_destroy(msg_t *msg) {
		free(msg->content);
		free(msg);

	}

	msg_t* msg_copy(msg_t* msg) {
		return msg->msg_init(msg->content);
	}

	msg_t* msg_init(void *content) {
		msg_t* msg = (msg_t*)malloc(sizeof(msg_t));
		msg->content = content;
		msg->msg_init = msg_init;
		msg->msg_destroy = msg_destroy;
		msg->msg_copy = msg_copy;
		return msg;
	}


	void msg_destroy_string(msg_t* msg) {
		free(msg->content); // free copia privata
		free(msg); // free struct
	}

	msg_t* msg_copy_string(msg_t* msg) {
		return msg->msg_init( msg->content );
	}

	msg_t* msg_init_string(void* content) {
		//viene creata una copia "privata" della stringa
		msg_t* new_msg = (msg_t*)malloc( sizeof(msg_t) );
		char* string = (char*)content;
		char* new_content = (char*)malloc(strlen(string)+1); // +1 per \0 finale
		strcpy(new_content, string);
		new_msg->content = new_content;
		new_msg->msg_init = msg_init_string;
		new_msg->msg_destroy = msg_destroy_string;
		new_msg->msg_copy = msg_copy_string;
		return new_msg;
	}

	int increase_index_buffer(unsigned int size, unsigned int index) {
		if (index == size-1)
			index = 0;
		else
			index += 1;

		return index;
		
	}

	buffer_t* buffer_init(unsigned int maxsize) {
		buffer_t* buffer = (buffer_t*)malloc (sizeof(buffer_t));
		buffer->messages = (msg_t**)calloc(maxsize,sizeof(msg_t*));
		buffer-> size = maxsize;
		buffer->i_index = 0;
		buffer->e_index = 0;
		buffer-> occupied = 0;
		pthread_mutex_init(&buffer->mutex_i_index,NULL);
		pthread_mutex_init(&buffer->mutex_occupied,NULL);
		pthread_mutex_init(&buffer->mutex_e_index,NULL);
		pthread_cond_init(&buffer->buffer_not_full,NULL);
		pthread_cond_init(&buffer-> buffer_not_empty,NULL);
		return buffer;

	}

	void buffer_destroy(buffer_t* buffer) {
		free(buffer->messages);
		free(buffer);
	}

	/* operazioni sul buffer */
	// inserimento bloccante: sospende se pieno, quindi
	// effettua l’inserimento non appena si libera dello spazio
	// restituisce il messaggio inserito; N.B.: msg!=null
	msg_t* put_bloccante(buffer_t* buffer, msg_t* msg) {
		pthread_mutex_lock(&buffer->mutex_i_index);
		while (buffer->occupied == buffer->size) {
			pthread_cond_wait(&buffer->buffer_not_full,&buffer->mutex_i_index);
		}
		if (msg!= NULL) {
			buffer->messages[buffer->i_index] = msg;
			buffer->i_index = increase_index_buffer(buffer->size, buffer->i_index);
			pthread_mutex_lock(&buffer->mutex_occupied);
			buffer->occupied +=1;
			pthread_mutex_unlock(&buffer->mutex_occupied);

		}
		else {
			pthread_mutex_unlock(&buffer->mutex_i_index);
			return BUFFER_ERROR;
		}

		pthread_cond_signal(&buffer->buffer_not_empty); 
		pthread_mutex_unlock(&buffer->mutex_i_index); 
		return msg;
	}

	// inserimento non bloccante: restituisce BUFFER_ERROR se pieno,
	// altrimenti effettua l’inserimento e restituisce il messaggio
	// inserito; N.B.: msg!=null
	msg_t* put_non_bloccante(buffer_t* buffer, msg_t* msg){
		pthread_mutex_lock(&buffer->mutex_i_index);
		if (buffer->occupied == buffer->size || msg == NULL ) {
			pthread_mutex_unlock(&buffer->mutex_i_index);
			return BUFFER_ERROR;
		}

		buffer->messages[buffer->i_index] = msg;
		buffer->i_index = increase_index_buffer(buffer->size, buffer->i_index);
		pthread_mutex_lock(&buffer->mutex_occupied);
		buffer->occupied +=1;
		pthread_mutex_unlock(&buffer->mutex_occupied);
		pthread_cond_signal(&buffer->buffer_not_empty);
		pthread_mutex_unlock(&buffer->mutex_i_index);
		return msg;
	}

	// estrazione bloccante: sospende se vuoto, quindi
	// restituisce il valore estratto non appena disponibile
	msg_t* get_bloccante(buffer_t* buffer) {
		pthread_mutex_lock(&buffer->mutex_e_index);
		while (buffer->occupied == 0) {
			pthread_cond_wait(&buffer->buffer_not_empty,&buffer->mutex_e_index);
		}
		msg_t *msg = buffer->messages[buffer->e_index];
		buffer->messages[buffer->e_index] = NULL; 	
		buffer->e_index = increase_index_buffer(buffer->size, buffer->e_index);
		pthread_mutex_lock(&buffer->mutex_occupied);
		buffer->occupied -=1;
		pthread_mutex_unlock(&buffer->mutex_occupied);
		pthread_cond_signal(&buffer->buffer_not_full);
		pthread_mutex_unlock(&buffer->mutex_e_index);
		return msg;

	}

	// estrazione non bloccante: restituisce BUFFER_ERROR se vuoto
	// ed il valore estratto in caso contrario
	msg_t* get_non_bloccante(buffer_t* buffer) {
		pthread_mutex_lock(&buffer->mutex_e_index);
		if (buffer-> occupied == 0) {
			pthread_mutex_unlock(&buffer->mutex_e_index);
			return BUFFER_ERROR;
		}
		msg_t *msg = buffer->messages[buffer->e_index];
		buffer->messages[buffer->e_index] = NULL;
		buffer->e_index = increase_index_buffer(buffer-> size, buffer->e_index);
		pthread_mutex_lock(&buffer->mutex_occupied);
		buffer->occupied -=1;
		pthread_mutex_unlock(&buffer->mutex_occupied);
		pthread_cond_signal(&buffer->buffer_not_full);
		pthread_mutex_unlock(&buffer->mutex_e_index);
		return msg;
	}