| Supported Targets | ESP32-C3 | ESP32-S3 |
| ----------------- | -------- | -------- |

# FreeRTOS basic API SMP usages Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

FreeRTOS offers a rich array of communication objects and task notification mechanisms that facilitate interaction and synchronization between concurrent tasks. This example demonstrates the applications of some useful APIs, including task creation, queue, mutex / spinlock, and task notification, within the context of a Symmetric Multiprocessor (SMP) architecture.

## Contents of this example
Below is short explanation of remaining files in the project folder.

```
├── CMakeLists.txt
├── main
│   ├── CMakeLists.txt
│   ├── basic_freertos_smp_usage.h
│   ├── basic_freertos_smp_usage.c
│   ├── create_task_example.c
│   ├── queue_example.c
│   ├── lock_example.c
│   ├── task_notify_example.c
│   └── batch_processing_example.c
├── pytest_smp_examples.py
└── README.md                  This is the file you are currently reading
```

This example includes 5 parts: 

### Creating task example

The first part is shows how to create tasks that can be pinned (affinity with a specific core) or unpinned (no particular affinity with any core) on ESP32 series CPU cores thanks to the API function `xTaskCreatePinnedToCore()`. 

In this case, there are 4 tasks created in total:
* `pinned_task0_core0` task is created and pinned on core 0
* `pinned_task1_core0` task is also created and pinned on core 0
* `pinned_task2_core1` task is created and pinned on core 1
* `unpinned_task` task is the last one, it is unpinned, which means it can be scheduled to run on any core.
 
A task can be unpinned by setting the `xCoreID` field to `tskNO_AFFINITY` when calling `xTaskCreatePinnedToCore()`.


#### Example Output
In the task function, the API `esp_cpu_get_core_id()` is called to query on which core this task is currently running. The example should have the following console output that, "pinned_task0_core0" and "pinned_task1_core0" are running on core#0, while "pinned_task2_core1" is running on core#1, and "unpinned_task" can be running on both core#0 and core#1:

```
...
I (2123) create task example: task#0 is running on core#0
I (2133) create task example: task#1 is running on core#0
I (2133) create task example: task#2 is running on core#1
I (2153) create task example: task#3 is running on core#0
I (2283) create task example: task#0 is running on core#0
I (2293) create task example: task#1 is running on core#0
I (2313) create task example: task#2 is running on core#1
I (2323) create task example: task#3 is running on core#0
I (2453) create task example: task#0 is running on core#0
I (2463) create task example: task#1 is running on core#0
I (2483) create task example: task#3 is running on core#0
I (2483) create task example: task#2 is running on core#1
I (2623) create task example: task#0 is running on core#0
I (2633) create task example: task#1 is running on core#0
I (2643) create task example: task#3 is running on core#0
I (2653) create task example: task#2 is running on core#1
I (2793) create task example: task#0 is running on core#0
I (2803) create task example: task#1 is running on core#0
I (2803) create task example: task#3 is running on core#1
...

```

### Queue communication example
The second part is about how to use FreeRTOS built-in queue to transmit data between tasks. In this example, one task is sending a number every 250 millisecond to a msg queue by calling API `xQueueGenericSend()`, and another task receives data from this queue by calling API `xQueueReceive()`

#### Example Output
The example should have the following console output:

```
I (1737813) queue example: sent data = 0
I (1737813) queue example: received data = 0
I (1738063) queue example: sent data = 1
I (1738063) queue example: received data = 1
I (1738313) queue example: sent data = 2
I (1738313) queue example: received data = 2
I (1738563) queue example: sent data = 3
I (1738563) queue example: received data = 3
I (1738813) queue example: sent data = 4
I (1738813) queue example: received data = 4
I (1739063) queue example: sent data = 5
I (1739063) queue example: received data = 5
I (1739313) queue example: sent data = 6
I (1739313) queue example: received data = 6
I (1739563) queue example: sent data = 7
I (1739563) queue example: received data = 7
I (1739813) queue example: sent data = 8
I (1739813) queue example: received data = 8
I (1740063) queue example: sent data = 9
I (1740063) queue example: received data = 9
I (1740313) queue example: sent data = 10
I (1740313) queue example: received data = 10
...
```

### Locks example
In the third part, a simple comparison of performance between mutexes, spinlocks and atomic operations is presented, along with an instance of how to use mutexes as a mechanism for protecting shared resources. 

To highlight the differences in performance between mutexes, spinlocks and atomic operations, this example implements two tasks that share a resource, which will be protected by mutex and spinlock and declared as an atomic type variable, respectively. Note: if this example runs on single core, only 1 task of each type will be created.

The result illustrates that the spinlocks are faster because they don't trigger any context switch, but they are CPU-intensive. Using atomic operation is faster than using spinlock, because it doesn't involve entering and exiting critical sections. 

#### Example Output
The example should have the following console output:
```
I (5025) lock example: mutex task took 1562156 us on core1
I (5025) lock example: mutex task took 1567546 us on core0
I (7095) lock example: spinlock task took 73325 us on core0
I (7095) lock example: spinlock task took 68326 us on core1
I (9105) lock example: atomic task took 11806 us on core0
I (9105) lock example: atomic task took 6810 us on core1
I (10105) lock example: mutex task 0 created
I (10105) lock example: task0 read value = 0 on core #0
I (10105) lock example: mutex task 1 created
I (10605) lock example: task0 set value = 1
I (10605) lock example: task1 read value = 1 on core #1
I (11105) lock example: task1 set value = 2
I (11105) lock example: task0 read value = 2 on core #1
I (11605) lock example: task0 set value = 3
I (11605) lock example: task1 read value = 3 on core #1
I (12105) lock example: task1 set value = 4
I (12105) lock example: task0 read value = 4 on core #1
I (12605) lock example: task0 set value = 5
I (12605) lock example: task1 read value = 5 on core #1
I (13105) lock example: task1 set value = 6
...
```

### Task notification example
Two tasks communicate via FreeRTOS task notification systems: one is sending notifications while the other receives them.

#### Example Output
The example should have the following console output:
```
I (392163) task notify example: send_task sends a notification
I (392163) task notify example: 1 tasks pending
I (392163) task notify example: rcv_task is processing this task notification
I (393163) task notify example: send_task sends a notification
I (393163) task notify example: 1 tasks pending
I (393163) task notify example: rcv_task is processing this task notification
I (394163) task notify example: send_task sends a notification
I (394163) task notify example: 1 tasks pending
I (394163) task notify example: rcv_task is processing this task notification
I (395163) task notify example: send_task sends a notification
I (395163) task notify example: 1 tasks pending
I (395163) task notify example: rcv_task is processing this task notification
I (396163) task notify example: send_task sends a notification
I (396163) task notify example: 1 tasks pending
I (396163) task notify example: rcv_task is processing this task notification
...
```

### Batch processing example
In the last part, a practical demonstration is provided wherein queues, mutexes, and task notifications are integrated to implement a realistic workflow, thereby exemplifying their practical utility in real-world scenarios.

A task named **rcv_data_task** mimics receiving the irregularly arrived data. Every time a data item is received, it is pushed into a queue, and the received item number is increased by 1; once the task collects 5 data items, it sends a task notification to the **proc_data_task** to process this batch of data from the queue. When the latter task finishes processing, it will decrease the received item number by 5. Because both these 2 tasks can modify this global number, the modification action is protected by a mutex.

#### Example Output
The example should have the following console output:
```
I (2675163) batch processing example: enqueue data = 43
I (2675563) batch processing example: enqueue data = 29
I (2676013) batch processing example: enqueue data = 8
I (2676463) batch processing example: enqueue data = 56
I (2676873) batch processing example: enqueue data = 19
I (2676873) batch processing example: dequeue data = 43
I (2676873) batch processing example: dequeue data = 29
I (2676883) batch processing example: dequeue data = 8
I (2676883) batch processing example: dequeue data = 56
I (2676883) batch processing example: dequeue data = 19
I (2676893) batch processing example: decrease s_rcv_item_num to 0
I (2677413) batch processing example: enqueue data = 51
I (2677713) batch processing example: enqueue data = 5
I (2678243) batch processing example: enqueue data = 93
I (2678603) batch processing example: enqueue data = 66
I (2679213) batch processing example: enqueue data = 32
I (2679213) batch processing example: dequeue data = 51
I (2679213) batch processing example: dequeue data = 5
I (2679223) batch processing example: dequeue data = 93
I (2679223) batch processing example: dequeue data = 66
I (2679233) batch processing example: dequeue data = 32
I (2679233) batch processing example: decrease s_rcv_item_num to 0
...
```

## How to use this example

This example utilizes an interactive console component so that you can select the part you would like to run through the terminal. You can type 'help' to get the list of commands; use UP/DOWN arrows to navigate through command history; press TAB when typing command name to auto-complete. For more information on the interactive terminal console component, please refer to [console](../../console/README.md). The supported commands include:

* **help**: get the list of commands
* **create_task**: run the creating task example
* **queue**: run the queue example
* **lock**: run the locks example
* **task_notification**: run the task notification example
* **batch_processing**: run the batch processing example

Once a component starts running, it will be stopped in about 5 seconds. If you would like to extend the running time, please modify the value of macro **COMP_LOOP_PERIOD** in the header file inc.h.