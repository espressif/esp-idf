Touch Element
=============

:link_to_translation:`zh_CN:[中文]`

Overview
--------

The Touch Element Library is a highly abstracted element library designed on the basis of the touch sensor driver. The library provides a unified and user-friendly software interface to quickly build capacitive touch sensor applications. For more information about the touch sensor driver API, see :doc:`../peripherals/touch_pad`.

Architecture
^^^^^^^^^^^^

The Touch Element library configures touch sensor peripherals via the touch sensor driver. However, some necessary hardware parameters should be passed to :cpp:func:`touch_element_install` and will be configured automatically only after calling :cpp:func:`touch_element_start`. This sequential order is essential because configuring these parameters has a significant impact on the run-time system. Therefore, they must be configured after calling the start function to ensure the system functions properly.

These parameters include touch channel threshold, driver-level of waterproof shield sensor, etc. The Touch Element library sets the touch sensor interrupt and the esp_timer routine up, and the hardware information of the touch sensor (channel state, channel number) will be obtained in the touch sensor interrupt service routine. When the specified channel event occurs, the hardware information is passed to the esp_timer callback routine, which then dispatches the touch sensor channel information to the touch elements (such as button, slider, etc.). The library then runs a specified algorithm to update the touch element's state or calculate its position and dispatches the result accordingly.

So when using the Touch Element library, you are relieved from the implementation details of the touch sensor peripheral. The library handles most of the hardware information and passes the more meaningful messages to the event handler routine.

The workflow of the Touch Element library is illustrated in the picture below.

.. figure:: /../_static/touch_element/te_architecture.svg
    :scale: 100 %
    :align: center

    Touch Element architecture


The features in relation to the Touch Element library in {IDF_TARGET_NAME} are given in the table below.

.. list-table::
    :header-rows: 1
    :widths: 70 30
    :align: center

    * - Features
      - ESP32S2
    * - Touch Element waterproof
      - ✔
    * - Touch Element button
      - ✔
    * - Touch Element slider
      - ✔
    * - Touch Element matrix button
      - ✔


Peripheral
^^^^^^^^^^

{IDF_TARGET_NAME} integrates one touch sensor peripheral with several physical channels.

.. only:: esp32s2 or esp32s3

    - 14 physical capacitive touch channels
    - Timer or software FSM trigger mode
    - Up to 5 kinds of interrupt (Upper threshold and lower threshold interrupt, measure one channel finish and measure all channels finish interrupt, measurement timeout interrupt)
    - Sleep mode wakeup source
    - Hardware internal de-noise
    - Hardware filter
    - Hardware waterproof sensor
    - Hardware proximity sensor

The channels are located as follows:

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - Channel
      - {IDF_TARGET_NAME}
    * - **Channel 0**
      - **GPIO 0 (reserved)**
    * - **Channel 1**
      - **GPIO 1**
    * - **Channel 2**
      - **GPIO 2**
    * - **Channel 3**
      - **GPIO 3**
    * - **Channel 4**
      - **GPIO 4**
    * - **Channel 5**
      - **GPIO 5**
    * - **Channel 6**
      - **GPIO 6**
    * - **Channel 7**
      - **GPIO 7**
    * - **Channel 8**
      - **GPIO 8**
    * - **Channel 9**
      - **GPIO 9**
    * - **Channel 10**
      - **GPIO 10**
    * - **Channel 11**
      - **GPIO 11**
    * - **Channel 12**
      - **GPIO 12**
    * - **Channel 13**
      - **GPIO 13**
    * - **Channel 14**
      - **GPIO 14**


.. only:: esp32

    - 10 physical capacitive touch channels
    - Timer or software FSM trigger mode
    - 2 kinds of interrupt (Greater than the threshold and less than the threshold)
    - Sleep mode wakeup source

The channels are located as follows:

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - Channel
      - {IDF_TARGET_NAME}
    * - **Channel 0**
      - **GPIO 0 (effective)**
    * - **Channel 1**
      - **GPIO 1**
    * - **Channel 2**
      - **GPIO 2**
    * - **Channel 3**
      - **GPIO 3**
    * - **Channel 4**
      - **GPIO 4**
    * - **Channel 5**
      - **GPIO 5**
    * - **Channel 6**
      - **GPIO 6**
    * - **Channel 7**
      - **GPIO 7**
    * - **Channel 8**
      - **GPIO 8**
    * - **Channel 9**
      - **GPIO 9**


Terminology
-----------

The terms used in relation to the Touch Element library are given below.

.. list-table::
    :header-rows: 1
    :widths: 50 50
    :align: center

    * - Term
      - Definition
    * - **Touch sensor**
      - Touch sensor peripheral inside the chip
    * - **Touch channel**
      - Touch sensor channels inside the touch sensor peripheral
    * - **Touch pad**
      - Off-chip physical solder pad, generally inside the PCB
    * - **De-noise channel**
      - Internal de-noise channel, which is always Channel 0 and is reserved
    * - **Shield sensor**
      - One of the waterproof sensors for detecting droplets in small areas and compensating for the influence of water drops on measurements
    * - **Guard sensor**
      - One of the waterproof sensors for detecting extensive wading and to temporarily disable the touch sensor
    * - **Shield channel**
      - The channel that waterproof shield sensor connected to, which is always Channel 14
    * - **Guard channel**
      - The channel that waterproof guard sensor connected to
    * - **Shield pad**
      - Off-chip physical solder pad, generally is grids, and is connected to shield the sensor
    * - **Guard pad**
      - Off-chip physical solder pad, usually a ring, and is connected to the guard sensor

.. figure:: /../_static/touch_element/te_component.svg
    :scale: 100 %
    :align: center

    Touch sensor application system components


Touch Sensor Signal
^^^^^^^^^^^^^^^^^^^

Each touch sensor is able to provide the following types of signals:

- Raw: The Raw signal is the unfiltered signal from the touch sensor.
- Smooth: The Smooth signal is a filtered version of the Raw signal via an internal hardware filter.
- Benchmark: The Benchmark signal is also a filtered signal that filters out extremely low-frequency noise.

All of these signals can be obtained using touch sensor driver API.

.. figure:: /../_static/touch_element/te_signal.png
    :scale: 40 %
    :align: center

    Touch sensor signals


Touch Sensor Signal Threshold
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Touch Sensor Threshold value is a configurable threshold value used to determine when a touch sensor is touched or not. When the difference between the Smooth signal and the Benchmark signal becomes greater than the threshold value (i.e., ``(smooth - benchmark) > threshold``), the touch channel's state will be changed and a touch interrupt will be triggered simultaneously.

.. figure:: /../_static/touch_element/te_threshold.svg
    :scale: 40 %
    :align: center

    Touch sensor signal threshold


Sensitivity
^^^^^^^^^^^

Important performance parameter of the touch sensor, the larger it is, the better touch the sensor performs. It could be calculated by the format below:

.. math::

    Sensitivity = \frac{Signal_{press} - Signal_{release}}{Signal_{release}} = \frac{Signal_{delta}}{Signal_{benchmark}}


Waterproof
^^^^^^^^^^

Waterproof is the hardware feature of a touch sensor which has a guard sensor and shield sensor (always connect to Channel 14) that has the ability to resist a degree of influence of water drop and detect the water stream.


Touch Button
^^^^^^^^^^^^

The touch button consumes one channel of the touch sensor, and it looks like as the picture below:


.. figure:: /../_static/touch_element/te_button.svg
    :scale: 100 %
    :align: center

    Touch button


Touch Slider
^^^^^^^^^^^^

The touch slider consumes several channels (at least three channels) of the touch sensor, the more channels consumed, the higher resolution and accuracy position it performs. The touch slider looks like as the picture below:

.. figure:: /../_static/touch_element/te_slider.svg
    :scale: 100 %
    :align: center

    Touch slider


Touch Matrix
^^^^^^^^^^^^

The touch matrix button consumes several channels (at least 2 + 2 = 4 channels), and it gives a solution to use fewer channels and get more buttons. {IDF_TARGET_NAME} supports up to 49 buttons. The touch matrix button looks like as the picture below:

.. figure:: /../_static/touch_element/te_matrix.svg
    :scale: 100 %
    :align: center

    Touch matrix


Touch Element Library Usage
---------------------------

Using this library should follow the initialization flow below:

1. To initialize the Touch Element library by calling :cpp:func:`touch_element_install`.
2. To initialize touch elements (button/slider etc) by calling :cpp:func:`touch_xxxx_install`.
3. To create a new element instance by calling :cpp:func:`touch_xxxx_create`.
4. To subscribe events by calling :cpp:func:`touch_xxxx_subscribe_event`.
5. To choose a dispatch method by calling :cpp:func:`touch_xxxx_set_dispatch_method` that tells the library how to notify you while the subscribed event occurs.
6. If dispatch by callback, call :cpp:func:`touch_xxxx_set_callback` to set the event handler function.
7. To start the Touch Element library by calling :cpp:func:`touch_element_start`.
8. If dispatch by callback, the callback will be called by the driver core when an event happens, no need to do anything; If dispatch by event task, create an event task and call :cpp:func:`touch_element_message_receive` to obtain messages in a loop.
9. (Optional) If you want to suspend the Touch Element run-time system or for some reason that could not obtain the touch element message, :cpp:func:`touch_element_stop` should be called to suspend the Touch Element system and then resume it by calling :cpp:func:`touch_element_start` again.

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

1. To initialize the Touch Element library, you have to configure the touch sensor peripheral and Touch Element library by calling :cpp:func:`touch_element_install` with :cpp:type:`touch_elem_global_config_t`, the default initializer is available in :cpp:func:`TOUCH_ELEM_GLOBAL_DEFAULT_CONFIG` and this default configuration is suitable for the most general application scene, and it is suggested not to change the default configuration before fully understanding Touch Sensor peripheral because some changes might bring several impacts to the system.

2. To initialize the specified element, all the elements will not work before its constructor :cpp:func:`touch_xxxx_install` is called so as to save memory, so you have to call the constructor of each used touch element respectively, to set up the specified element.


Touch Element Instance Startup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

1. To create a new touch element instance, call :cpp:func:`touch_xxxx_create`, select a channel, and provide its `Sensitivity`_ value for the new element instance.

2. To subscribe to events, call :cpp:func:`touch_xxxx_subscribe_event`. The Touch Element library offers several events, and the event mask is available in :idf_file:`components/touch_element/include/touch_element/touch_element.h`. You can use these event masks to subscribe to specific events individually or combine them to subscribe to multiple events.

3. To configure the dispatch method, use :cpp:func:`touch_xxxx_set_dispatch_method`. The Touch Element library provides two dispatch methods: :cpp:enumerator:`TOUCH_ELEM_DISP_EVENT` and :cpp:enumerator:`TOUCH_ELEM_DISP_CALLBACK`. These methods allow you to obtain the touch element message and handle it using different approaches.

Events Processing
^^^^^^^^^^^^^^^^^

If :cpp:enumerator:`TOUCH_ELEM_DISP_EVENT` dispatch method is configured, you need to start up an event handler task to obtain the touch element message, all the elements' raw message could be obtained by calling :cpp:func:`touch_element_message_receive`, then extract the element-class-specific message by calling the corresponding message decoder with :cpp:func:`touch_xxxx_get_message` to get the touch element's extracted message; If :cpp:enumerator:`TOUCH_ELEM_DISP_CALLBACK` dispatch method is configured, you need to pass an event handler by calling :cpp:func:`touch_xxxx_set_callback` before the touch element starts working, all the element's extracted message will be passed to the event handler function.

.. warning::

    Since the event handler function runs on the core of the element library, i.e., in the esp_timer callback routine, please avoid performing operations that may cause blocking or delays, such as calling :cpp:func:`vTaskDelay`.


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

1. The waterproof shield sensor is always-on after Touch Element waterproof is initialized, however, the waterproof guard sensor is optional, hence if the you do not need the guard sensor, ``TOUCH_WATERPROOF_GUARD_NOUSE`` has to be passed to :cpp:func:`touch_element_waterproof_install` by the configuration struct.

2. To associate the touch element with the guard sensor, pass the touch element's handle to the Touch Element waterproof's masked list by calling :cpp:func:`touch_element_waterproof_add`. By associating a touch element with the Guard sensor, the touch element will be disabled when the guard sensor is triggered by a stream of water so as to protect the touch element.

The Touch Element Waterproof example is available in :example:`peripherals/touch_sensor/touch_element/touch_element_waterproof` directory.

In code, the waterproof configuration may look as follows:

.. code-block:: c

    void app_main()
    {
        ...

        touch_xxxx_install();                 //Initialize instance (button, slider, etc)
        touch_xxxx_create(&element_handle);  //Create a new Touch element

        ...

        touch_element_waterproof_install();              //Initialize Touch Element waterproof
        touch_element_waterproof_add(element_handle);   //Let an element associate with the guard sensor

        ...
    }

Wakeup from Light/Deep-sleep Mode
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Only Touch Button can be configured as a wake-up source.

Light- or Deep-sleep modes are both supported to be wakened up by a touch sensor. For the Light-sleep mode, any installed touch button can wake it up. But only the sleep button can wake up from Deep-sleep mode, and the touch sensor will do a calibration immediately, the reference value will be calibrated to a wrong value if our finger does not remove timely. Though the wrong reference value recovers after the finger removes away and has no effect on the driver logic, if you do not want to see a wrong reference value while waking up from Deep-sleep mode, you can call :cpp:func:`touch_element_sleep_enable_wakeup_calibration` to disable the wakeup calibration.

The Touch Element Wakeup example is available in `system/light_sleep` directory.

.. code-block:: c

    void app_main()
    {
        ...
        touch_element_install();
        touch_button_install();                 //Initialize the touch button
        touch_button_create(&element_handle);  //Create a new Touch element

        ...

        // ESP_ERROR_CHECK(touch_element_enable_light_sleep(&sleep_config));
        ESP_ERROR_CHECK(touch_element_enable_deep_sleep(button_handle[0], &sleep_config));
        // ESP_ERROR_CHECK(touch_element_sleep_enable_wakeup_calibration(button_handle[0], false)); // (optional) Disable wakeup calibration to prevent updating the benchmark to a wrong value

        touch_element_start();

        ...
    }

Application Examples
--------------------

- :example:`peripherals/touch_sensor/touch_element/touch_element_waterproof` demonstrates how to use the Touch Element library to set up capacitive touch sensors with waterproof protection.
- :example:`peripherals/touch_sensor/touch_element/touch_slider` demonstrates how to use the Touch Element library to set up and operate a touch slider.
- :example:`peripherals/touch_sensor/touch_element/touch_elements_combination` demonstrates how to use the Touch Element library to set up multiple types of touch elements and handle all the event messages in one task.
- :example:`peripherals/touch_sensor/touch_element/touch_matrix` demonstrates how to use the Touch Element library to set up and use a touch matrix with a capacitive touch sensor on {IDF_TARGET_NAME} development boards.
- :example:`peripherals/touch_sensor/touch_element/touch_button` demonstrates how to use the Touch Element library to set up and use a touch button on {IDF_TARGET_NAME} development boards.


API Reference - Touch Element Core
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
