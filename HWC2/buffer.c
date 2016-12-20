	#include "buffer.h"


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


	void buffer_to_string(buffer_t* buffer) {
		printf("Buffer I_INDEX = %d\n",buffer->i_index);
		printf("Buffer E_INDEX = %d\n",buffer->e_index);
		printf("Buffer SIZE = %d\n",buffer->size);
		printf("Buffer OCCUPIED = %d\n",buffer->occupied);
	}