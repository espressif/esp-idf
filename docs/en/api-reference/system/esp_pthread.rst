ESP-pthread
===========

Overview
--------

This module offers Espressif specific extensions to the pthread library that can be used to influence the behaviour of pthreads. Currently the following configuration can be tuned:
  * Stack size of the pthreads
  * Priority of the created pthreads
  * Inheriting this configuration across threads
  * Thread name
  * Core affinity / core pinning.

Example to tune the stack size of the pthread:

.. highlight:: c

::
   
   main()
   {
       pthread_t t1;

       esp_pthread_cfg_t cfg = esp_create_default_pthread_config();
       cfg.stack_size = (4 * 1024);
       esp_pthread_set_cfg(&cfg);

       pthread_create(&t1, NULL, thread_func);
   }

The API can also be used for inheriting the settings across threads. For example:

.. highlight:: c

::
   
   void * my_thread2(void * p)
   {
         /* This thread will inherit the stack size of 4K */
         printf("In my_thread2\n");
   }

   void * my_thread1(void * p)
   {
         printf("In my_thread1\n");
	 pthread_t t2;
	 pthread_create(&t2, NULL, my_thread2);
   }

   main()
   {

        pthread_t t1;

        esp_pthread_cfg_t cfg = esp_create_default_pthread_config();
        cfg.stack_size = (4 * 1024);
        cfg.inherit_cfg = true;
        esp_pthread_set_cfg(&cfg);

        pthread_create(&t1, NULL, my_thread1);
   }

API Reference
-------------

.. include:: /_build/inc/esp_pthread.inc

