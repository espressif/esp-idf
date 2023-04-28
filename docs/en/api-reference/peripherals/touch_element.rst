Touch Element
=============

Overview
--------

Touch Element library provides a high level abstraction for building capacitive touch applications. The library's implementation gives a unified and friendly software interface thus allows for smooth and easy capacitive touch application development. The library is implemented atop the touch sensor driver (please see :doc:`Touch sensor driver API Reference <../peripherals/touch_pad>` for more information regarding low level API usage).

Architecture
^^^^^^^^^^^^

Touch Element library configures touch sensor peripherals via touch sensor driver. While some necessary hardware parameters should be passed to :cpp:func:`touch_element_install` and will be configured automatically only after calling :cpp:func:`touch_element_start`, because it will make great influence on the run-time system.

These parameters include touch channel threshold, waterproof shield sensor driver-level and etc. Touch Element library sets touch sensor interrupt and esp-timer routine up and the hardware information of touch sensor (channel state, channel number) will be obtained in touch sensor interrupt service routine. When the specified channel event occurs, and those hardware information will be passed to the esp-timer callback routine, esp-timer callback routine will dispatch the touch sensor channel information to the touch elements(such as button, slider etc). Then runs the specified algorithm to update touch element's state or calculate its position, dispatch the result to user.

So using Touch Element library, user doesn't need to care about the implementation of touch sensor peripheral, Touch Element library will handle most of the hardware information and pass the more meaningful messages to user event handler routine.

Workflow of Touch Element library is illustrated in the picture below.

.. figure:: /../_static/touch_element/te_architecture.svg
    :scale: 100 %
    :align: center

    Touch Element architecture


The features in relation to the Touch Element library in {IDF_TARGET_NAME} are given in the table below.

+--------------------------------------------------------------+---------------------+
| Features                                                     | ESP32S2             |
+==============================================================+=====================+
| Touch Element waterproof                                     | ✔                   |
+--------------------------------------------------------------+---------------------+
| Touch Element button                                         | ✔                   |
+--------------------------------------------------------------+---------------------+
| Touch Element slider                                         | ✔                   |
+--------------------------------------------------------------+---------------------+
| Touch Element matrix button                                  | ✔                   |
+--------------------------------------------------------------+---------------------+


Peripheral
^^^^^^^^^^

{IDF_TARGET_NAME} integrates one touch sensor peripheral with several physical channels.

.. only:: esp32s2

    - 14 physical capacitive touch channels
    - Timer or software FSM trigger mode
    - Up to 5 kinds of interrupt(Upper threshold and lower threshold interrupt, measure one channel finish and measure all channels finish interrupt, measurement timeout interrupt)
    - Sleep mode wakeup source
    - Hardware internal de-noise
    - Hardware filter
    - Hardware waterproof sensor
    - Hardware proximity sensor


.. only:: esp32

    - 10 physical capacitive touch channels
    - Timer or software FSM trigger mode
    - 2 kinds of interrupt(Greater than threshold and less than threshold)
    - Sleep mode wakeup source


The channels are located as follows:

=======================  =======================
       Channel              {IDF_TARGET_NAME}
=======================  =======================
   **Channel 0**          **GPIO 0 (reserved)**
   **Channel 1**               **GPIO 1**
   **Channel 2**               **GPIO 2**
   **Channel 3**               **GPIO 3**
   **Channel 4**               **GPIO 4**
   **Channel 5**               **GPIO 5**
   **Channel 6**               **GPIO 6**
   **Channel 7**               **GPIO 7**
   **Channel 8**               **GPIO 8**
   **Channel 9**               **GPIO 9**
   **Channel 10**              **GPIO 10**
   **Channel 11**              **GPIO 11**
   **Channel 12**              **GPIO 12**
   **Channel 13**              **GPIO 13**
   **Channel 14**              **GPIO 14**
=======================  =======================


Terminology
-----------

The terms used in relation to the Touch Element library are given in the below.

=======================  =========================================================================================
         Term                                                   Definition
=======================  =========================================================================================
    **Touch sensor**     Touch sensor peripheral inside the chip
    **Touch channel**    Touch sensor channels inside the touch sensor peripheral
    **Touch pad**        Off-chip physical solder pad (Generally inside the PCB)
   **De-noise channel**  Internal de-noise channel (Is always Channel 0 and it is reserved)
    **Shield sensor**    One of the waterproof sensor, use for compensating the influence of water drop
    **Guard sensor**     One of the waterproof sensor, use for detecting the water stream
    **Shield channel**   The channel that waterproof shield sensor connected to (Is always Channel 14)
    **Guard channel**    The channel that waterproof guard sensor connected to
    **Shield pad**       Off-chip physical solder pad (Generally is grids) and is connected to shield sensor
    **Guard pad**        Off-chip physical solder pad (Is usually a ring) and is connected to guard sensor
=======================  =========================================================================================

.. figure:: /../_static/touch_element/te_component.svg
    :scale: 100 %
    :align: center

    Touch sensor application system components


Touch Sensor Signal
^^^^^^^^^^^^^^^^^^^

Each touch senor is able to provide the following types of signals:

- Raw: The Raw signal is the unfiltered signal from the touch sensor
- Smooth: The Smooth signal is a filtered version of the Raw signal via an internal hardware filter
- Benchmark: The Benchmark signal is also a filtered signal that filters out extremely low-frequency noise.

All of these signals can be obtained using touch sensor driver API.

.. figure:: /../_static/touch_element/te_signal.png
    :scale: 40 %
    :align: center

    Touch sensor signals


Touch Sensor Threshold
^^^^^^^^^^^^^^^^^^^^^^

The Touch Sensor Threshold value is a configurable threshold value used to determine when a touch sensor is touched or not. When difference between the Smooth signal and the Benchmark signal becomes greater than the threshold value (i.e., ``(smooth - benchmark) > threshold``), the touch channel's state will be changed and a touch interrupt will be triggered simultaneously.

.. figure:: /../_static/touch_element/te_threshold.svg
    :scale: 40 %
    :align: center

    Touch sensor signal threshold


Sensitivity
^^^^^^^^^^^

Important performance parameter of touch sensor, the larger it is, the better touch sensor will perform. It could be calculated by the format in below:

.. math::

    Sensitivity = \frac{Signal_{press} - Signal_{release}}{Signal_{release}} = \frac{Signal_{delta}}{Signal_{benchmark}}


Waterproof
^^^^^^^^^^

Waterproof is a hardware feature of touch sensor which has guard sensor and shield sensor (Always connect to Channel 14) that has the ability to resist a degree influence of water drop and detect the water stream.


Touch Button
^^^^^^^^^^^^

Touch button consumes one channel of touch sensor, and it looks like as the picture below:


.. figure:: /../_static/touch_element/te_button.svg
    :scale: 100 %
    :align: center

    Touch button


Touch Slider
^^^^^^^^^^^^

Touch slider consumes several channels(at least three channels) of touch sensor, the more channels consumed, the higher resolution and accuracy position it will perform. Touch slider looks like as the picture below:

.. figure:: /../_static/touch_element/te_slider.svg
    :scale: 100 %
    :align: center

    Touch slider


Touch Matrix
^^^^^^^^^^^^

Touch matrix button consumes several channels(at least 2 + 2 = 4 channels), it gives a solution to use fewer channels and get more buttons. {IDF_TARGET_NAME} supports up to 49 buttons. Touch matrix button looks like as the picture below:

.. figure:: /../_static/touch_element/te_matrix.svg
    :scale: 100 %
    :align: center

    Touch matrix


Touch Element Library Usage
---------------------------

Using this library should follow the initialization flow below:

1. To initialize Touch Element library by calling :cpp:func:`touch_element_install`
2. To initialize touch elements(button/slider etc) by calling :cpp:func:`touch_xxxx_install`
3. To create a new element instance by calling :cpp:func:`touch_xxxx_create`
4. To subscribe events by calling :cpp:func:`touch_xxxx_subscribe_event`
5. To choose a dispatch method by calling :cpp:func:`touch_xxxx_set_dispatch_method` that tells the library how to notify you while the subscribed event occur
6. (If dispatch by callback) Call :cpp:func:`touch_xxxx_set_callback` to set the event handler function.
7. To start Touch Element library by calling :cpp:func:`touch_element_start`
8. (If dispatch by callback) The callback will be called by the driver core when event happen, no need to do anything; (If dispatch by event task) create an event task and call :cpp:func:`touch_element_message_receive` to obtain messages in a loop.
9. [Optional] If user wants to suspend the Touch Element run-time system or for some reason that could not obtain the touch element message, :cpp:func:`touch_element_stop` should be called to suspend the Touch Element system and then resume it by calling :cpp:func:`touch_element_start` again.

In code, the flow above may look like as follows:

.. code-block:: c

    static touch_xxx_handle_t element_handle; //Declare a touch element handle

    //Define the subscribed event handler
    void event_handler(touch_xxx_handle_t out_handle, touch_xxx_message_t out_message, void *arg)
    {
        //Event handler logic
    }

    void app_main()
    {
        //Using the default initializer to config Touch Element library
        touch_elem_global_config_t global_config = TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG();
        touch_element_install(&global_config);

        //Using the default initializer to config Touch elements
        touch_xxx_global_config_t elem_global_config = TOUCH_XXXX_GLOBAL_DEFAULT_CONFIG();
        touch_xxx_install(&elem_global_config);

        //Create a new instance
        touch_xxx_config_t element_config = {
            ...
            ...
        };
        touch_xxx_create(&element_config, &element_handle);

        //Subscribe the specified events by using the event mask
        touch_xxx_subscribe_event(element_handle, TOUCH_ELEM_EVENT_ON_PRESS | TOUCH_ELEM_EVENT_ON_RELEASE, NULL);

        //Choose CALLBACK as the dispatch method
        touch_xxx_set_dispatch_method(element_handle, TOUCH_ELEM_DISP_CALLBACK);

        //Register the callback routine
        touch_xxx_set_callback(element_handle, event_handler);

        //Start Touch Element library processing
        touch_element_start();
    }


Initialization
^^^^^^^^^^^^^^

1. To initialize Touch Element library, user has to configure touch sensor peripheral and Touch Element library by calling :cpp:func:`touch_element_install` with :cpp:type:`touch_elem_global_config_t`, the default initializer is available in :cpp:func:`TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG` and this default configuration is suitable for the most general application scene, and users are suggested not to change the default configuration before fully understanding Touch Sensor peripheral, because some changes might bring several impacts to the system.

2. To initialize the specified element, all the elements will not work before its constructor (:cpp:func:`touch_xxxx_install`) is called so as to save memory, so user has to call the constructor of each used touch element respectively, to set up the specified element.


Touch Element Instance Startup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. To create a new touch element instance by calling :cpp:func:`touch_xxxx_create`, selects channel and passes its `Sensitivity`_ for the new element instance.

2. To subscribe events by calling :cpp:func:`touch_xxxx_subscribe_event`, there several events in Touch Element library and the event mask is available on :idf_file:`components/touch_element/include/touch_element/touch_element.h`, user could use those events mask to subscribe specified event or combine them to subscribe multiple events.

3. To configure dispatch method by calling :cpp:func:`touch_xxxx_subscribe_event`, there are two dispatch methods in Touch Element library, one is :cpp:enumerator:`TOUCH_ELEM_DISP_EVENT`, the other one is :cpp:enumerator:`TOUCH_ELEM_DISP_CALLBACK`, it means that user could use two methods to obtain the touch element message and handle it.

Events Processing
^^^^^^^^^^^^^^^^^

If :cpp:enumerator:`TOUCH_ELEM_DISP_EVENT` dispatch method is configured, user need to startup an event handler task to obtain the touch element message, all the elements raw message could be obtained by calling :cpp:func:`touch_element_message_receive`, then extract the element-class-specific message by calling the corresponding message decoder (:cpp:func:`touch_xxxx_get_message`) to get the touch element's extracted message; If :cpp:enumerator:`TOUCH_ELEM_DISP_CALLBACK` dispatch method is configured, user need to pass an event handler by calling :cpp:func:`touch_xxxx_set_callback` before the touch elem starts working, all the element's extracted message will be passed to the event handler function.

.. warning::
    Since the event handler function runs on the library driver core(The context located in esp-timer callback routine), user
    should not do something that attempts to block or delay, such as call :cpp:func:`vTaskDelay`.


In code, the events handle procedure may look like as follows:

.. code-block:: c

    /* ---------------------------------------------- TOUCH_ELEM_DISP_EVENT ----------------------------------------------- */
    void element_handler_task(void *arg)
    {
        touch_elem_message_t element_message;
        while(1) {
            if (touch_element_message_receive(&element_message, Timeout) == ESP_OK) {
                const touch_xxxx_message_t *extracted_message = touch_xxxx_get_message(&element_message); //Decode message
                ... //Event handler logic
            }
        }
    }
    void app_main()
    {
        ...

        touch_xxxx_set_dispatch_method(element_handle, TOUCH_ELEM_DISP_EVENT);  //Set TOUCH_ELEM_DISP_EVENT as the dispatch method
        xTaskCreate(&element_handler_task, "element_handler_task", 2048, NULL, 5, NULL);  //Create a handler task

        ...
    }
    /* -------------------------------------------------------------------------------------------------------------- */

    ...
    /* ---------------------------------------------- TOUCH_ELEM_DISP_CALLBACK ----------------------------------------------- */
    void element_handler(touch_xxxx_handle_t out_handle, touch_xxxx_message_t out_message, void *arg)
    {
        //Event handler logic
    }

    void app_main()
    {
        ...

        touch_xxxx_set_dispatch_method(element_handle, TOUCH_ELEM_DISP_CALLBACK);  //Set TOUCH_ELEM_DISP_CALLBACK as the dispatch method
        touch_xxxx_set_callback(element_handle, element_handler);  //Register an event handler function

        ...
    }
    /* -------------------------------------------------------------------------------------------------------------- */


Waterproof Usage
^^^^^^^^^^^^^^^^

1. To initialize Touch Element waterproof, the waterproof shield sensor is always-on after Touch Element waterproof is initialized, however the waterproof guard sensor is optional, hence if user doesn't need the guard sensor, `TOUCH_WATERPROOF_GUARD_NOUSE` has to be passed to :cpp:func:`touch_element_waterproof_install` by the configuration struct.

2. To associate the touch element with the guard sensor, pass the touch element's handle to the Touch Element waterproof's masked list by calling :cpp:func:`touch_element_waterproof_add`. By associating a touch element with the Guard sensor, the touch element will be disabled when the guard sensor is triggered by a stream of water so as to protect the touch element.

The Touch Element Waterproof example is available in `peripherals/touch_element/touch_element_waterproof` directory.

In code, the waterproof configuration may look like as follows:

.. code-block:: c

    void app_main()
    {
        ...

        touch_xxxx_install();                 //Initialize instance (button, slider, etc)
        touch_xxxx_create(&element_handle);  //Create a new Touch element

        ...

        touch_element_waterproof_install();              //Initialize Touch Element waterproof
        touch_element_waterproof_add(element_handle);   //Let a element associates with guard sensor

        ...
    }


Application Example
-------------------
All the Touch Element library examples could be found in the `peripherals/touch_element` directory of ESP-IDF examples.


API Reference - Touch Element core
----------------------------------

.. include-build-file:: inc/touch_element.inc


API Reference - Touch Button
----------------------------------

.. include-build-file:: inc/touch_button.inc


API Reference - Touch Slider
----------------------------------

.. include-build-file:: inc/touch_slider.inc


API Reference - Touch Matrix
----------------------------------

.. include-build-file:: inc/touch_matrix.inc
