	#include <CUnit/CUnit.h>
	#include <CUnit/Basic.h>
	#include "HWC1.h"
	#include <unistd.h>

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

	void not_blocking_get_test() {
		buffer_t* buffer = buffer_init(1);
		const char* string= "Hi";
		msg_t *msg = msg_init_string((void*)string);
		buffer->messages[buffer->i_index] = msg;
		buffer->occupied +=1;
		msg_t* test = get_non_bloccante(buffer);
		CU_ASSERT_EQUAL(test,msg);
		CU_ASSERT_TRUE(buffer->occupied==0);
		CU_ASSERT_EQUAL(buffer->messages[0],NULL);
		free(test);
		free(buffer);
	}

	void* single_blocking_get_call(void* arg) {
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t* msg = get_bloccante(b);
		printf("Thread unlocked\n");
		CU_ASSERT_STRING_EQUAL(msg->content,"Hi");
		pthread_exit(0);
	}

	void blocking_get_test() {
		buffer_t* buffer = buffer_init(1);
		pthread_t consumer;
		if (pthread_create(&consumer,NULL,&single_blocking_get_call,&buffer)) {
			printf("error creating consumer thread."); abort();
		}
		sleep(7);
		const char* string= "Hi";
		msg_t *msg = msg_init_string((void*)string);
		put_non_bloccante(buffer,msg);
		if (pthread_join(consumer,NULL)) {
			printf("error joining consumer thread."); abort();
		}
		CU_ASSERT_TRUE(buffer->occupied==0);
	}

	void* single_blocking_put_call(void* arg) {
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		const char* string= "Hi";
		msg_t *msg = msg_init_string((void*)string);
		msg_t* test = put_bloccante(b,msg);
		printf("Thread unlocked\n");
		CU_ASSERT_STRING_EQUAL(test->content,msg->content);
		pthread_exit(0);
	}

	void blocking_put_test() {
		buffer_t* buffer = buffer_init(1);
		const char* string= "Hi";
		msg_t *msg = msg_init_string((void*)string);
		pthread_t producer;
		// filling the buffer
		put_non_bloccante(buffer,msg);
		CU_ASSERT_TRUE(buffer->occupied==1);
		if (pthread_create(&producer,NULL,&single_blocking_put_call,&buffer)) {
			printf("error creating consumer thread."); abort();
		}
		sleep(7);
		// emptying the buffer
		get_non_bloccante(buffer);
		CU_ASSERT_TRUE(buffer->occupied==0);
		if (pthread_join(producer,NULL)) {
			printf("error joining producer thread."); abort();
		}
		CU_ASSERT_TRUE(buffer->occupied==1);
	}



	void* producer(void *arg) {
		int i = 0;
		pthread_t thread_id;
		thread_id = pthread_self();
		printf("Thread Producer %d\n",(int)thread_id);
		const char* string = "Hi";
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t *msg = msg_init_string((void*)string);
		msg_t* test;		
		while (i<5) {
			test = put_bloccante(*buffer, msg);
			i++;
		}
		pthread_exit(0);
		
	}


	void* multiple_producers(void *arg) {
		int i = 1;
		pthread_t thread_id;
		thread_id = pthread_self();
		printf("Thread Producer %d\n",(int)thread_id);
		const char* string = "Hi";
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t *msg = msg_init_string((void*)string);
		msg_t* test;		
		while (i<=b->size/2) {
			test = put_bloccante(*buffer, msg);
			//printf("Message put by Thread Producer %d\n",(int)thread_id);
			i++;
		}
		pthread_exit(0);
		
	}

	void* consumer(void* arg) {
		int i = 0;
		pthread_t thread_id = pthread_self();
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t* msg;
		while (i<5) {
			msg = get_bloccante(*buffer);
			if (msg!= NULL)
				printf("\nMessage taken by Thread %d\n",(int)thread_id);
			i += 1;
		}
		pthread_exit(0);
	}

	void* multiple_consumers(void* arg) {
		pthread_t thread_id = pthread_self();
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t* msg;
		msg = get_non_bloccante(*buffer);
		if (msg!= NULL)
			printf("\nMessage taken by Thread %d\n",(int)thread_id);
	
		pthread_exit(0);
	}

	void* multiple_consumers_blocking(void* arg) {
		pthread_t thread_id = pthread_self();
		buffer_t **buffer = (buffer_t**)arg;
		buffer_t* b = *buffer;
		msg_t* msg;
		msg = get_bloccante(*buffer);
		if (msg!= NULL)
			printf("\nMessage taken by Thread %d\n",(int)thread_id);
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