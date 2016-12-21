   #include <CUnit/CUnit.h>
   #include <CUnit/Basic.h>
   #include <unistd.h>
   #include <stdio.h>
   #include "buffer.h"
   #include "accepter.h"
   #include "dispatcher.h"  
   
   /* Structure that contains the parameters to pass
   to the dispacther/accepter*/
   typedef struct param {
      buffer_t* buffer;
      accepter_t* accepter;
   } parameters_t;

   // This function creates a random string of the desired length
   void rand_str(char *dest, size_t length) {
      char charset[] = "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

      while (length-- > 0) {
         size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
         *dest++ = charset[index];
      }
      *dest = '\0';
   }

   // richiedi la creaz. di un reader
   void submitRequest(buffer_t *requests, char name[]) {
      msg_t* new_request = msg_init_string(name);
      put_bloccante(requests,new_request);  
   }

   // test of the method submit requests
   void test_submit_request() {
      buffer_t* requests = buffer_init(5);
      char name[] = "Test Thread";
      submitRequest(requests,name);
      CU_ASSERT_TRUE(requests->occupied == 1);
      CU_ASSERT_STRING_EQUAL(requests->messages[0]->content,name);
      free(requests);

   }

   // function executed by the provider thread
   // it puts messages in a shared buffer with the dispatcher who has
   // the task to deliver them to all the readers
   void* provider_procedure(void* req) {
      printf("Provider Created\n");
      buffer_t* requests = (buffer_t*)req;
      char* text_message;
      msg_t** messages = (msg_t**)calloc(requests->size,sizeof(msg_t*));
      int i;
      
      for (i=0; i<requests->size; i++) {
         text_message = (char*)calloc(6,sizeof(char));
         rand_str(text_message,5);
         msg_t* single_message = msg_init_string(text_message);
         put_bloccante(requests,single_message);
      }
      msg_t *poison_pill = msg_init_pill(NULL);
      put_bloccante(requests,poison_pill);
      printf("PROVIDER THREAD : Terminating Provider\n");
      pthread_exit(0);                                                                             
   }

   // procedure executed by the dispacther
   // it basically calls the function dispatch_messages of the dispacther
   void* dispatcher_procedure(void* parameters) {
      printf("DISPATCHER THREAD : Dispatcher Created\n");
      parameters_t* dispatcher_parameters = (parameters_t*)parameters;
      dispatch_messages(dispatcher_parameters->accepter,dispatcher_parameters->buffer);
      printf("DISPATCHER THREAD : Terminating Dispatcher\n");
      pthread_exit(0);
   }

   // procedure executed by the accepter
   // it basically calls the function add_reader of the accepter
   void* accepter_procedure(void** parameters) {
      printf("ACCEPTER THREAD: Accepter Created\n");
      parameters_t* accepter_parameters = *(parameters_t**)parameters;
      add_reader(accepter_parameters->accepter,accepter_parameters->buffer);
      buffer_to_string(accepter_parameters->buffer);
      CU_ASSERT_TRUE(accepter_parameters->accepter->num_readers == accepter_parameters->buffer->size-1 );
      printf("ACCEPTER THREAD: Terminating Accepter\n");
      pthread_exit(0);
   }
   
   void test_dispacther() {
      pthread_t dispatcher_thread;
      pthread_t provider_thread;
      pthread_t accepter_thread;
      int size = 1;
      int i;
      const char* end =  "end";
      //initialize the number of messages to send to the dispacther
      buffer_t *requests = buffer_init(5);
      // initialize the number of readers to be created
      buffer_t *readers = buffer_init(5);
      accepter_t* accepter = accepter_init();
      parameters_t* accepter_parameters = (parameters_t*)malloc(sizeof(parameters_t));
      parameters_t* parameters = (parameters_t*)malloc(sizeof(parameters_t));
      parameters-> buffer = requests;
      parameters-> accepter = accepter;
      accepter_parameters-> buffer = readers;
      accepter_parameters-> accepter = accepter;

      // in the test the number of readers created corresponds to the size of the buffer -1
      // the last cell is used to put the string end that communicates to the accepter to terminate
      for (i = 0; i< readers->size-1; i++) {
         char* text_message = (calloc(6,sizeof(char)));
         rand_str(text_message,5);
         printf ("Reader's Name = %s\n", text_message);
         msg_t* msg = msg_init_string(text_message);
         put_bloccante(readers,msg);
      }
      msg_t *end_message = msg_init_string((void*)end);
      put_bloccante (readers,end_message);
      printf("MAIN THREAD : Inserted all the name of the readers\n");
      
      if (pthread_create(&accepter_thread,NULL,accepter_procedure,&accepter_parameters)) {
         printf("error creating accepter thread."); abort();
      }

      sleep(5);      
      
      if (pthread_create(&provider_thread,NULL,provider_procedure,requests)) {
         printf("error creating provider thread."); abort();
      }

      if (pthread_create(&dispatcher_thread,NULL,dispatcher_procedure,parameters)) {
         printf("error creating dispatcher thread."); abort();
      }

      if (pthread_join(dispatcher_thread,NULL)) {
         printf("error joining dispatcher thread"); abort();
      }

      
      if (pthread_join(accepter_thread,NULL)) {
         printf("error joining accepter thread."); abort();
      }
     
      
      if (pthread_join(provider_thread,NULL)) {
         printf("error joining provider thread"); abort();
      }

      free(requests);
      free(readers);
      free(accepter_parameters);
      free(parameters);

   }
   //test the creation of a single reader
   void test_accepter_add_reader() {
      accepter_t* accepter = accepter_init();
      buffer_t* requests = buffer_init(2);
      pthread_t accepter_thread;
      char name[] = "Test Thread";
      char end[] = "end";
      submitRequest(requests,name);
      submitRequest(requests,end);
      // create and pass the structure with the parameters to the accepter procedure
      parameters_t* parameters = (parameters_t*)malloc(sizeof(parameters_t));
      parameters-> buffer = requests;
      parameters-> accepter = accepter;

      if (pthread_create(&accepter_thread,NULL,accepter_procedure,&parameters)) {
         printf("error creating accepter thread."); abort();
      }

      if (pthread_join(accepter_thread,NULL)) {
         printf("error joining accepter thread."); abort();
      }
      
      CU_ASSERT_TRUE(size(accepter->readers)==1);
      free(requests);
      free(accepter);
      
   }

   int main() {
		CU_pSuite pSuiteSimple = NULL;
      CU_pSuite pSuiteComplex = NULL;
		if (CUE_SUCCESS !=CU_initialize_registry())
      		return CU_get_error();
      	pSuiteSimple = CU_add_suite("Simple Scenario",NULL, NULL);
      	pSuiteComplex = CU_add_suite("Complex Scenario", NULL,NULL);
      	if ((CU_add_test(pSuiteSimple, "test of submit_request()", test_submit_request)== NULL)||
          (CU_add_test(pSuiteSimple, "test of add_reader()", test_accepter_add_reader)== NULL)) {
      		CU_cleanup_registry();
        	return CU_get_error();
   		}
         
         if ((CU_add_test(pSuiteComplex, "test of dispatcher", test_dispacther)== NULL)) {
            CU_cleanup_registry();
         return CU_get_error();
         } 
         
        

   		CU_basic_set_mode(CU_BRM_VERBOSE);
		CU_basic_run_tests();
		CU_cleanup_registry();
		return CU_get_error();
}