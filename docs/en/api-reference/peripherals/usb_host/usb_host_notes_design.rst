USB Host Maintainers Notes (Design Guidelines)
==============================================

Design Considerations
---------------------

The design of the Host Stack takes into account the following design considerations:

**Limited Hardware Resources:**

The embedded nature of Host Stack means limited hardware resources (such as memory and processing power) when compared to larger host systems.

**USB2.0 Chapter 10:**

Chapter 10 of the USB 2.0 specification specifies certain requirements of USB Host systems, in particular the required layers of the USB Host's system software.

**Diverse Use Case Complexities:**

The embedded nature of the Host Stack also means a wide range of use cases with differing complexities. Some USB Host applications aim to only support a single vendor specific device, whereas other applications require support for a wide range of devices of different classes.

Requirements
------------

Given the design considerations above, the Host Stack was designed with the following set of requirements:

DMA Support
^^^^^^^^^^^

**Requirement: The Host Stack must support DMA.**

The Host Stack must support DMA in order to reduce CPU's workload. DMA support allows the automatic copying of USB transfer data to/from the Host Controller without CPU intervention. This is especially critical given the embedded nature of the CPU (i.e., lower CPU frequencies) and large maximum data payloads of USB transfers (e.g., 1023 bytes for isochronous transfers).

Minimize Memory Copies
^^^^^^^^^^^^^^^^^^^^^^

**Requirement: The Host Stack should minimize the amount of memory copies when passing data between layers.**

Various data and objects (e.g., USB transfers) need to be passed between multiple layers of the Host Stack. The Host Stack should minimize the amount of memory copies that occur between layers by allocating the data's/object's memory once, and simply passing a pointer to that data/object between the layers. Therefore, the Host Stack requires some standardized data types shared across multiple layers (see USB2.0 Section 10.3.4).

Event Driven
^^^^^^^^^^^^

**Requirement: The Host Stack must allow for event driven operation (i.e., the Host Stack's API must not be polling).**

The Host Stack needs to support some CPU intensive use application scenarios such as video streaming (i.e., UVC class). Therefore, the Host Stack should minimize CPU usage by allowing for completely event driven operation, thus reserving the majority of CPU time for the application itself (i.e., video encoding/decoding in this case).

The Host Stack needs to communicate events across the layers using interrupts, callbacks, and FreeRTOS synchronization primitives (e.g., queues and semaphores).

No Task Creation
^^^^^^^^^^^^^^^^

**Requirement: All layers of the Host Stack below (and including) the Host Library layer must not create any tasks.**

Task stacks are generally one of the most memory intensive parts of an ESP-IDF applications. Given the wide range of applications scenarios, the number of tasks created (and their stack sizes) can vary greatly. For example...

- applications that require low latency or high throughput (such as isochronous transfers) may choose to create a dedicated task to handle those transfers in order to minimize latency.
- applications that do not have strict latency requirements (such as bulk transfers) may choose to handle those transfers from a shared task in order to save some memory.

Therefore, all layers of the Host Stack below (and including) the Host Library layer **must not** create any tasks. Instead, these layers should expose handlers functions to be called from tasks created by the upper layers. Task creation will be delegated to the class driver or application layer.

Operable at Different Layers
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Given the wide range of use case complexities, the Host Stack must be operable at different layers, allowing users to use the Host Stack at a lower layer (e.g., the HCD or HAL) or at a higher layer (e.g., a class driver).

Being operable at different layers allows the users to decide on the appropriate trade-off between convenience, control, and optimization for their application when using the Host Stack. For example...

- Host Stack applications that support a dedicated custom device may want to use a lower level of abstraction for better optimization, control, and simpler API.
- Host Stack applications that need to support a wide range of device classes requires the full Host Stack so that device enumeration is automatically handled.

Coding Conventions
------------------

The Host Stack follows the following set of coding rules/guidelines for better code readability and maintainability:

Symbols Use Layer Name As Prefix
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For each layer of the Host Stack, the symbols exposed by that layer (i.e., functions, types, macros) must be prefixed with that layer's name. For example, the symbols exposed by the HCD layer will be prefixed ``hcd_...``/``HCD_...``.

However, internal symbols (e.g., static functions) **should not** be prefixed with their layer's name. This makes it easier to differentiate between internal and external symbols when modifying that layer's source code.

Critical Section Functions Prefixed With ``_``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

In each layer of the Host Stack, there are various static functions that must be called inside a critical section. The names of these functions are prefixed with ``_`` (e.g., ``_func_called_from_crit()``) to make it easier for maintainers to differentiate which functions should be called from critical sections. For example...

.. code-block:: c

  some_func();  // Called outside critical section
  taskENTER_CRITICAL(&some_lock);
  _some_func_crit();  // Called inside critical section. _ prefix makes it easier to differentiate
  taskEXIT_CRITICAL(&some_lock);

Grouping Structure Members by Locking Mechanism
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Some layers of the Host Stack utilize multiple locking schemes (e.g., critical sections and task mutexes) to ensure thread safety, where each locking scheme offers a different level of protection. However, member variables of the same object can be protected by different locking scheme. Therefore, to clearly demarcate the different locking schemes and their associated variables, structure members are grouped by locking scheme as nested structures.

.. list-table:: Locking Scheme
    :widths: 10 10 80
    :header-rows: 1

    * - Locking Scheme
      - Nested Structure
      - Description
    * - Critical Sections
      - ``dynamic``
      - Shared data accessed from both a task context and ISR context are protected by a critical section.
    * - Task Mutexes
      - ``mux_protected``
      - Shared data accessed from only a task context are protected by a FreeRTOS Mutex
    * - Single Thread
      - ``single_thread``
      - Data that is only ever accessed by the same task do not require the use of any locks.
    * - Constant
      - ``constant``
      - Constant data is set once during the object's instantiation and never changed again. Thus, any task or ISR can freely the constant data without the use of locks, so long as the variable is never written to.

Grouping structure members by locking scheme makes the code more maintainable as it makes clear which locking scheme is required when accessing a particular member variable, as demonstrated in the code snippet below:

.. code-block:: c

    typedef struct some_obj some_obj_t;

    some_obj_t obj;

    // Accessing dynamic members requires critical section
    taskENTER_CRITICAL(&some_lock);
    obj.dynamic.varA = 1;
    taskEXIT_CRITICAL(&some_lock);

    // Accessing mutex protected members requires taking the mutex
    xSemaphoreTake(&some_mux, portMAX_DELAY);
    obj.mux_protected.varB = 1;
    xSemaphoreGive(&some_mux);

    // Accessing single thread members does not require locking so long as this is the only task to access it
    obj.single_thread.varC = 1;

    // Accessing constant members requires no locking. But only read access is allowed
    int local_var = obj.constant.varD;
