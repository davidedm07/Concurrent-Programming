	#include "dispatcher.h"

	// Function that sends messages to all the readers in a list
	// The list is created by the accepter in an asynchronous way
	void dispatch_messages(accepter_t* accepter, buffer_t* provider_messages) {
		// initialize the poison pill, message to send when the dispacther
		// needs to stop/kill a reader
		msg_t* poison_pill = msg_init_pill(NULL);
		// get the first message from the buffer filled by the provider
		msg_t* provider_message = get_bloccante(provider_messages);
		// initialization of the components 
		reader_t* reader;
		msg_t* result; 
		iterator_t* iterator;
		printf("Dispacther sending messages\n");
		// lock the list of readers
		pthread_mutex_lock(&accepter->mutex_readers);
		// the while is ended when the dispacther reads a poison pill
		while(poison_pill->content != provider_message->content) {
			// initialize the iterator on the list of readers
			iterator = iterator_init(accepter->readers);
			//the dispacther sends messages at all the elements of the list
			while(hasNext(iterator)==1) {
				//get a copy of the message of the provider
				msg_t* copy = provider_message->msg_copy(provider_message);
				// take the current reader in the list node
				reader = ((reader_t*)(iterator->currentNode->payload));
				// send the copy of the message to the reader
				result = put_non_bloccante(reader->buffer,copy);
				// if the dispacther finds a full buffer when sending a message to the reader
				// it considers the reader too slow and send it a poison pill in order to terminate it
				if (result == BUFFER_ERROR) {
					// field that indicates that the reader was too slow for the dispacther
					reader->failures = 1;
					printf("Stopping a slow reader\n");
					// take the last message from the full buffer and replace it with the poison pill
					get_non_bloccante(reader->buffer);
					put_non_bloccante(reader->buffer,poison_pill);
					// remove the reader from the list 
					removeElement(accepter->readers,iterator->currentNode->payload);
					// update the number of readers
					pthread_mutex_lock(&accepter->mutex_num_readers);
					accepter->num_readers -= 1;
					pthread_mutex_unlock(&accepter->mutex_num_readers);
				} 
				// goes to the next element
				next(iterator);
			}
			// read a new message from the provider buffer
			provider_message = get_bloccante(provider_messages);
		}
		
		//initialize again the buffer to send the poison pills to all the readers
		iterator = iterator_init(accepter->readers);
		printf("Dispatcher sending poison pills to readers\n");
		// send the poison pills to stop alll the readers
		while (hasNext(iterator)==1) {
			reader = ((reader_t*)(iterator->currentNode->payload));
			result = put_non_bloccante(reader->buffer,poison_pill);
			next(iterator);
		}
		// unlock the mutex on the list
		pthread_mutex_unlock(&accepter->mutex_readers);
		
	}

