	#include "dispatcher.h"


	void dispatch_messages(accepter_t* accepter, buffer_t* provider_messages) {
		int i;
		
		msg_t* poison_pill = msg_init_pill(NULL);
		
		msg_t* provider_message = get_bloccante(provider_messages);
		reader_t* reader;

		msg_t* result; 
		iterator_t* iterator;
		printf("Dispacther sending messages\n");
		pthread_mutex_lock(&accepter->mutex_readers);
		while(poison_pill->content != provider_message->content) {
			iterator = iterator_init(accepter->readers);
			while(hasNext(iterator)==1) {
				
				msg_t* copy = provider_message->msg_copy(provider_message);
				reader = ((reader_t*)(iterator->currentNode->payload));
				result = put_non_bloccante(reader->buffer,copy);
				
				if (result == BUFFER_ERROR) {
					reader->failures = 1;
					printf("Stopping a slow reader\n");
					get_non_bloccante(reader->buffer);
					put_non_bloccante(reader->buffer,poison_pill);
					removeElement(accepter->readers,iterator->currentNode->payload);
					pthread_mutex_lock(&accepter->mutex_num_readers);
					accepter->num_readers -= 1;
					pthread_mutex_unlock(&accepter->mutex_num_readers);
				} 
				
				next(iterator);
			}
			
			provider_message = get_bloccante(provider_messages);
		}
		
		//inizializza di nuove l'iteratore
		iterator = iterator_init(accepter->readers);
		printf("Dispatcher sending poison pills to readers\n");
		while (hasNext(iterator)==1) {
			reader = ((reader_t*)(iterator->currentNode->payload));
			result = put_non_bloccante(reader->buffer,poison_pill);
			next(iterator);
		}
		pthread_mutex_unlock(&accepter->mutex_readers);
		printf("Terminating Dispatcher\n");
		pthread_exit(0);

	}

