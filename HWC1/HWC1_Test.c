	#include <CUnit/CUnit.h>
	#include <CUnit/Basic.h>
	#include "HWC1.h"
	#include <unistd.h>

	/* Test of the put_non_bloccante method*/
	void not_blocking_put_test() {
		buffer_t* buffer = buffer_init(1);
		const char* string= "Hi";
		msg_t *msg = msg_init_string((void*)string);
		//test empty buffer and message not null
		msg_t* test = put_non_bloccante(buffer,msg);
		CU_ASSERT_EQUAL(test,msg);
		CU_ASSERT_EQUAL(buffer->occupied,1);
		//test full buffer and message not null
		test = put_non_bloccante(buffer,msg);
		CU_ASSERT_NOT_EQUAL(test,msg);
		// test message null and full buffer
		test = NULL;
		test = put_non_bloccante(buffer,msg);
		CU_ASSERT_NOT_EQUAL(test,msg);
		//CU_ASSERT_STRING_EQUAL(test->content,BUFFER_ERROR);
		free(test);
		free(buffer);

	}

	/* Test of the get_non_bloccante method*/
	void not_blocking_get_test() {
		// creation of the buffer
		buffer_t* buffer = buffer_init(1);
		// creation of a string and the message containing that string
		const char* string= "Hi";
		msg_t *msg = msg_init_string((void*)string);
		//filling the buffer with the message
		buffer->messages[buffer->i_index] = msg;
		// increasing the occupied index of the buffer
		buffer->occupied +=1;
		// retrieving the message from the buffer
		msg_t* test = get_non_bloccante(buffer);
		// chechking if the string taken from the buffer is the
		// same that was put for the test
		CU_ASSERT_STRING_EQUAL(test->content,msg->content);
		// checking if the index was updated during the get call
		CU_ASSERT_TRUE(buffer->occupied==0);
		//checking if the buffer is now empty
		CU_ASSERT_EQUAL(buffer->messages[0],NULL);
		//freeing the memory used for the test
		free(test);
		free(buffer);
	}

	// helping function for testing a single call of
	// the function get_non_bloccante
	void* single_blocking_get_call(void* arg) {
		//casting the parameter passed as a void pointer
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		//calling the function to test
		msg_t* msg = get_bloccante(b);
		// print a message after the thread being unlocked
		printf("\nThread unlocked\n");
		// check if the message retrieved is the same put in the buffer
		CU_ASSERT_STRING_EQUAL(msg->content,"Hi");
		pthread_exit(0);
	}

	// test of get_bloccante function
	void blocking_get_test() {
		// initialize the buffer
		buffer_t* buffer = buffer_init(1);
		// thread for the consumer
		pthread_t consumer;
		// creation of the thread of the consumer
		// this thread will run the function single_blocking_get_call
		if (pthread_create(&consumer,NULL,&single_blocking_get_call,&buffer)) {
			printf("error creating consumer thread."); abort();
		}
		// put the main thread in a pause to test if the consumer thread
		// is really blocked --> the buffer is empty
		sleep(7);
		//creating the message to put in the buffer
		const char* string= "Hi";
		msg_t *msg = msg_init_string((void*)string);
		put_non_bloccante(buffer,msg);
		//wait for the consumer thread
		if (pthread_join(consumer,NULL)) {
			printf("error joining consumer thread."); abort();
		}
		//check if the buffer was emptied by the consumer thread
		CU_ASSERT_TRUE(buffer->occupied==0);
	}
	// function called for the test of the put_bloccante method
	void* single_blocking_put_call(void* arg) {
		//casting the parameter passed as a void pointer
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		// creating the message to put into the buffer
		const char* string= "How are you?";
		msg_t *msg = msg_init_string((void*)string);
		//calling the put_bloccante method, the buffer is full so
		// the producer will stay blocked
		msg_t* test = put_bloccante(b,msg);
		//print a message when the thread is unlocked
		printf("\nThread unlocked\n");
		//check if the method put the message in the buffer correctly
		CU_ASSERT_STRING_EQUAL(test->content,msg->content);
		// end this thread
		pthread_exit(0);
	}

	
	void blocking_put_test() {
		// creating the buffer
		buffer_t* buffer = buffer_init(1);
		const char* string= "Hi";
		//creating the message to put in the buffer in order to fill it
		msg_t *msg = msg_init_string((void*)string);
		pthread_t producer;
		// filling the buffer
		put_non_bloccante(buffer,msg);
		// checking if the buffer is full 
		CU_ASSERT_TRUE(buffer->occupied==1);
		// creating the thread producer
		if (pthread_create(&producer,NULL,&single_blocking_put_call,&buffer)) {
			printf("error creating consumer thread."); abort();
		}
		// stopping the main thread in order to make the producer thread block
		sleep(7);
		// emptying the buffer
		get_non_bloccante(buffer);
		// testing if the buffer is empty now
		CU_ASSERT_TRUE(buffer->occupied==0);
		// waiting for the producer thread 
		if (pthread_join(producer,NULL)) {
			printf("error joining producer thread."); abort();
		}
		// checking if the message in the buffer is the one put by the producer thread
		CU_ASSERT_STRING_EQUAL(buffer->messages[0]->content, "How are you?");
		// checking if the buffer is full
		CU_ASSERT_TRUE(buffer->occupied==1);
	}


	// function for a single producer that needs to put i messages
	// in the buffer, 5 for the test
	void* producer(void *arg) {
		int i = 0;
		// id of the thread
		pthread_t thread_id;
		thread_id = pthread_self();
		// printing the id of the producer
		printf("\nThread Producer %d\n",(int)thread_id);
		const char* string = "Hi";
		// casting the buffer passed as parameter
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		// creating the message
		msg_t *msg = msg_init_string((void*)string);
		msg_t* test;
		// putting 5 messages in the buffer		
		while (i<5) {
			test = put_bloccante(*buffer, msg);
			i++;
		}
		//ending the thread
		pthread_exit(0);
		
	}

	// function for the test of multiple producers
	void* multiple_producers(void *arg) {
		int i = 1;
		// id of the thread
		pthread_t thread_id;
		thread_id = pthread_self();
		// printing the id of the producer
		printf("\nThread Producer %d\n",(int)thread_id);
		const char* string = "Hi";
		// casting the buffer passed as parameter
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		// creating the message
		msg_t *msg = msg_init_string((void*)string);
		msg_t* test;
		// putting concurrently the messages in the buffer		
		while (i<=b->size/2) {
			test = put_bloccante(*buffer, msg);
			i++;
		}
		// ending the thread
		pthread_exit(0);
		
	}

	// function for a single consumer 
	void* consumer(void* arg) {
		int i = 0;
		pthread_t thread_id = pthread_self();
		// casting the buffer passed as parameter
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t* msg;
		// retrieving 5 messages from the buffer
		while (i<5) {
			msg = get_bloccante(*buffer);
			i += 1;
		}
		//ending the thread
		pthread_exit(0);
	}
	// function called by multiple consumers that
	// retrieve messages from the buffer concurrently
	void* multiple_consumers(void* arg) {
		// id of the thread 
		pthread_t thread_id = pthread_self();
		// casting the buffer passed as parameter
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t* msg;
		// getting the message from the buffer
		msg = get_non_bloccante(*buffer);
		if (msg!= NULL)
			printf("\nMessage taken by Thread %d\n",(int)thread_id);
		//ending the thread
		pthread_exit(0);
	}


	// function called by multiple consumers, 
	void* multiple_consumers_blocking(void* arg) {
		// id of the thread 
		pthread_t thread_id = pthread_self();
		// casting the buffer passed as parameter
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t* msg;
		// getting the message from the buffer
		msg = get_bloccante(*buffer);
		if (msg!= NULL)
			printf("\nMessage taken by Thread %d\n",(int)thread_id);
		// ending the thread
		pthread_exit(0);
	}
	
	void consumer_producer_test() {
		pthread_t producers;
		pthread_t consumers;
		buffer_t* buffer = buffer_init(1);
		if (pthread_create(&producers,NULL,producer,&buffer)) {
			printf("error creating producer thread."); abort();
		}
		
		if (pthread_create(&consumers,NULL,consumer,&buffer)) {
			printf("error creating consumer thread."); abort();
		}
		
		if (pthread_join(producers,NULL)) {
			printf("error joining producers thread."); abort();
		}

		if (pthread_join(consumers,NULL)) {
			printf("error joining consumers thread."); abort();
		}

		CU_ASSERT_TRUE(buffer->messages[0]==NULL);
		CU_ASSERT_TRUE(buffer->occupied == 0);
		free(buffer);		

	}

	void multiple_consumers_test() {

		buffer_t* buffer = buffer_init(1);
		const char* string = "Hi";
		msg_t *msg = msg_init_string((void*)string);
		buffer->messages[0] = msg;
		buffer->occupied +=1;
		pthread_t consumer1;
		pthread_t consumer2;
		pthread_t consumer3;
		pthread_t consumer4;

		if (pthread_create(&consumer1,NULL,multiple_consumers,&buffer)) {
			printf("error creating consumer1 thread."); abort();
		}
		if (pthread_create(&consumer2,NULL,multiple_consumers,&buffer)) {
			printf("error creating consumer2 thread."); abort();
		}
		if (pthread_create(&consumer3,NULL,multiple_consumers,&buffer)) {
			printf("error creating consumer3 thread."); abort();
		}
		if (pthread_create(&consumer4,NULL,multiple_consumers,&buffer)) {
			printf("error creating consumer4 thread."); abort();
		}
		if (pthread_join(consumer1,NULL)) {
			printf("error joining consumer1 thread."); abort();
		}
		if (pthread_join(consumer2,NULL)) {
			printf("error joining consumer2 thread."); abort();
		}
		if (pthread_join(consumer3,NULL)) {
			printf("error joining consumer3 thread."); abort();
		}
		if (pthread_join(consumer4,NULL)) {
			printf("error joining consumer4 thread."); abort();
		}

		CU_ASSERT_TRUE(buffer->occupied == 0);
		free(buffer);

	}


	void multiple_consumers_one_producer_test() {

		buffer_t* buffer = buffer_init(5);
		pthread_t consumer1;
		pthread_t consumer2;
		pthread_t consumer3;
		pthread_t consumer4;
		pthread_t single_producer;

		if (pthread_create(&consumer1,NULL,multiple_consumers_blocking,&buffer)) {
			printf("error creating consumer1 thread."); abort();
		}
		if (pthread_create(&consumer2,NULL,multiple_consumers_blocking,&buffer)) {
			printf("error creating consumer2 thread."); abort();
		}
		if (pthread_create(&consumer3,NULL,multiple_consumers_blocking,&buffer)) {
			printf("error creating consumer3 thread."); abort();
		}
		if (pthread_create(&consumer4,NULL,multiple_consumers_blocking,&buffer)) {
			printf("error creating consumer4 thread."); abort();
		}
		if (pthread_create(&single_producer,NULL,producer,&buffer)) {
			printf("error creating producer thread."); abort();
		}
		if (pthread_join(consumer1,NULL)) {
			printf("error joining consumer1 thread."); abort();
		}
		if (pthread_join(consumer2,NULL)) {
			printf("error joining consumer2 thread."); abort();
		}
		if (pthread_join(consumer3,NULL)) {
			printf("error joining consumer3 thread."); abort();
		}
		if (pthread_join(consumer4,NULL)) {
			printf("error joining consumer4 thread."); abort();
		}
		if (pthread_join(single_producer,NULL)) {
			printf("error joining producer thread."); abort();
		}

		CU_ASSERT_TRUE(buffer->occupied == 1);
		free(buffer);

	}

	void multiple_consumers_producers_test() {

		buffer_t* buffer = buffer_init(4);
		pthread_t consumer1;
		pthread_t consumer2;
		pthread_t consumer3;
		pthread_t consumer4;
		pthread_t producer1;
		pthread_t producer2;

		if (pthread_create(&consumer1,NULL,multiple_consumers_blocking,&buffer)) {
			printf("error creating consumer1 thread."); abort();
		}
		if (pthread_create(&consumer2,NULL,multiple_consumers_blocking,&buffer)) {
			printf("error creating consumer2 thread."); abort();
		}
		if (pthread_create(&consumer3,NULL,multiple_consumers_blocking,&buffer)) {
			printf("error creating consumer3 thread."); abort();
		}
		if (pthread_create(&consumer4,NULL,multiple_consumers_blocking,&buffer)) {
			printf("error creating consumer4 thread."); abort();
		}
		if (pthread_create(&producer1,NULL,multiple_producers,&buffer)) {
			printf("error creating producer1 thread."); abort();
		}
		if (pthread_create(&producer2,NULL,multiple_producers,&buffer)) {
			printf("error creating producer2 thread."); abort();
		}
		if (pthread_join(consumer1,NULL)) {
			printf("error joining consumer1 thread."); abort();
		}
		if (pthread_join(consumer2,NULL)) {
			printf("error joining consumer2 thread."); abort();
		}
		if (pthread_join(consumer3,NULL)) {
			printf("error joining consumer3 thread."); abort();
		}
		if (pthread_join(consumer4,NULL)) {
			printf("error joining consumer4 thread."); abort();
		}
		if (pthread_join(producer1,NULL)) {
			printf("error joining producer1 thread."); abort();
		}
		if (pthread_join(producer2,NULL)) {
			printf("error joining producer2 thread."); abort();
		}

		CU_ASSERT_TRUE(buffer->occupied == 0);
		free(buffer);

	}


	int main() {
		CU_pSuite pSuite = NULL;
		if (CUE_SUCCESS !=CU_initialize_registry())
      		return CU_get_error();
      	pSuite = CU_add_suite("Suite_1",NULL, NULL);
      	
      	if ((NULL == CU_add_test(pSuite, "test of put_non_bloccante()", not_blocking_put_test))||
      		(NULL == CU_add_test(pSuite, "test of get_non_bloccante()", not_blocking_get_test))||
      		(NULL == CU_add_test(pSuite, "test of get_bloccante()", blocking_get_test))||
      		(NULL == CU_add_test(pSuite, "test of put_bloccante()", blocking_put_test))||
      		(NULL == CU_add_test(pSuite, "test of multiple_consumers()",multiple_consumers_test))||
      		(NULL == CU_add_test(pSuite, "test of consumer_producer()",consumer_producer_test))||
      		(NULL == CU_add_test(pSuite, "test of multiple_consumers_one_producer_test()",multiple_consumers_one_producer_test))||
      		(NULL == CU_add_test(pSuite, "test of multiple_consumers_producers_test()",multiple_consumers_producers_test))){
      		CU_cleanup_registry();
        	return CU_get_error();
   		}

   		CU_basic_set_mode(CU_BRM_VERBOSE);
		CU_basic_run_tests();
		CU_cleanup_registry();
		return CU_get_error();
}