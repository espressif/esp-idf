ESP-TEE: Advanced Topics
========================

.. _tee-detailed-architecture:

Detailed Architecture
---------------------

The ESP-TEE framework on {IDF_TARGET_NAME} utilizes the inherent features of the RISC-V architecture, along with Espressif's proprietary hardware peripherals (given below), to ensure system isolation and enable a secure execution environment.

- Inherent RISC-V CPU features

  * Machine (M) and User (U) privilege levels
  * PMP (Physical Memory Protection)
  * PMA (Physical Memory Attributes)

- Espressif proprietary hardware

  * APM (Access Permission Management) peripheral

Together, these components enable the {IDF_TARGET_NAME} SoC to allocate the chip's hardware resources (internal memory, external memory, and peripherals) and software resources into two modes - Machine (M) mode and User (U) mode. The CPU can switch between these modes, with the TEE running in the higher privilege M-mode and the REE running in the lower privilege U-mode.

    .. figure:: ../../../_static/esp_tee/{IDF_TARGET_PATH_NAME}/esp_tee_arch.png
        :align: center
        :scale: 90%
        :alt: ESP TEE Architecture for {IDF_TARGET_NAME}
        :figclass: align-center

        ESP-TEE: Architecture for {IDF_TARGET_NAME}

Boot-up Flow
^^^^^^^^^^^^

The :doc:`Application Startup Flow <../../api-guides/startup>` is slightly modified when ESP-TEE is enabled. After a reset, the second stage bootloader loads both the TEE image and the application image from the boot device. Control is then handed over to the TEE, which configures memory, interrupts, and peripheral access settings before switching to the REE in U-mode to execute the application image.


    .. figure:: ../../../_static/esp_tee/esp_tee_boot_seq.png
        :align: center
        :scale: 65%
        :alt: ESP TEE Boot sequence for {IDF_TARGET_NAME}
        :figclass: align-center

        ESP-TEE: Boot Sequence for {IDF_TARGET_NAME}

Isolation: Hardware
^^^^^^^^^^^^^^^^^^^

RISC-V: Privilege Levels
~~~~~~~~~~~~~~~~~~~~~~~~

In RISC-V architecture, each hardware thread (hart) operates at a specific privilege level, defined as a mode within one or more Control Status Registers (CSRs). These privilege levels serve as a protective measure, segregating different components within the software stack. Any attempts to execute operations or access assets that are not permitted by the current privilege mode will trigger an exception. The M-mode has the highest privileges and code running in M-mode is usually inherently trusted, as it has low-level access to the machine implementation.

For more information, please refer to the *RISC-V Instruction Set Manual Volume II: Privileged Architecture, Version 1.10*.

Physical Memory Protection (PMP) and Physical Memory Attributes (PMA) Checker
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ESP-TEE framework for {IDF_TARGET_NAME} leverages the RISC-V architecture's features, such as Physical Memory Protection (PMP) and Physical Memory Attributes (PMA) Checker, to manage access to system memory (internal SRAM and external Flash) within the TEE and REE contexts.

The PMP unit allows M-mode software to configure access privileges (read, write, execute) for required memory regions. This mechanism enforces resource isolation between the TEE and REE, adhering to the system's predefined policies and ensuring secure access control.

The PMAC unit enhances security by providing additional permission checks, including configurable memory types and customizable attributes for required memory regions.

For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *Physical Memory Protection* [`PDF <{IDF_TARGET_TRM_EN_URL}#subsection.1.8>`__] and *Physical Memory Attribute (PMA) Checker* [`PDF <{IDF_TARGET_TRM_EN_URL}#subsection.1.9>`__].

Access Permission Management (APM)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Access Permission Management (APM) module is a {IDF_TARGET_NAME}-hardware peripheral designed to enforce isolation between the TEE and REE, both for the system memory and peripherals. It is composed of two primary components: the TEE controller and the APM controller, each with its dedicated registers. The TEE-related registers are used to configure the security mode of each master (e.g., HP CPU or DMA acting as a master accessing memory), and the APM-related registers are used to specify the access permission and access address range of each security mode.

APM enhances system isolation in areas where the PMP and PMA mechanisms fall short. The number of entries for PMP/PMA are limited, and these mechanisms cannot govern accesses by the low-power core or other masters, such as the GDMA.

For more details, see *{IDF_TARGET_NAME} Technical Reference Manual* > *Permission Control (PMS)* [`PDF <{IDF_TARGET_TRM_EN_URL}#permctrl>`__].

.. note::

  For {IDF_TARGET_NAME}, the Access Permission Management (APM) and the Physical Memory Protection (PMP) together play the role of Permission Control (PMS).

Isolation: Entities
^^^^^^^^^^^^^^^^^^^

Internal Memory (SRAM)
~~~~~~~~~~~~~~~~~~~~~~

A region at the top of the HP SRAM is reserved for the TEE, allocated for TEE code execution and runtime data storage, including stack and heap. Permissions for all memory regions within the SRAM are managed by the PMP and configured by the TEE.

The REE memory is partitioned into IRAM (text: Read/Execute) and DRAM (data: Read/Write) sections, with the division controlled by the PMP.
However, the TEE memory is divided into IRAM and DRAM sections, with division enforced by the PMA.

    .. figure:: ../../../_static/esp_tee/{IDF_TARGET_PATH_NAME}/esp_tee_memory_layout.png
        :align: center
        :scale: 80%
        :alt: ESP TEE Memory Map for {IDF_TARGET_NAME}
        :figclass: align-center

        ESP-TEE: Memory Map for {IDF_TARGET_NAME}

External Memory (Flash)
~~~~~~~~~~~~~~~~~~~~~~~

Designated partitions in the external flash are reserved for the TEE, serving various purposes, including TEE code execution via XIP, secure storage, and OTA data. The PMS safeguards these partitions from unauthorized access, with the APM module protecting the MMU and SPI1 controller registers, and the PMP securing the cache.

.. figure:: ../../../_static/esp_tee/{IDF_TARGET_PATH_NAME}/esp_tee_flash_layout.png
    :align: center
    :scale: 80%
    :alt: ESP TEE Flash Memory Map for {IDF_TARGET_NAME}
    :figclass: align-center

    ESP-TEE: Flash Memory Map for {IDF_TARGET_NAME}

.. _tee-flash-prot-scope:

**Flash Protection - Virtual and Physical Access**

The key interfaces for flash memory protection are the cache connected to SPI0, which provides virtual access to flash memory, and the SPI1 controller, which provides physical access. By default, the cache and the MMU registers are secured by the PMS, preventing virtual access to the TEE-related flash partitions from the REE.

When :doc:`Flash Encryption <../flash-encryption>` is enabled, the REE can still access TEE flash regions via SPI1, but read operations will return encrypted data. Since neither the REE nor TEE has direct access to the flash encryption key, this prevents attackers from inferring TEE contents through direct reads.

Additionally with :ref:`Secure Boot <secure_boot-guide>` enabled, any unauthorized modifications to the TEE firmware will be detected during boot, causing signature verification to fail. Thus, the combination of Flash Encryption and Secure Boot provides a robust level of protection suitable for most applications.
However, do note that while the TEE firmware integrity is protected, other TEE partitions (e.g., :doc:`Secure Storage <tee-sec-storage>`, :ref:`TEE OTA data <tee-ota-data-partition>`) can be modified through direct writes.

For stronger isolation, you can enable :ref:`CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1`, which completely blocks access to all TEE flash regions via SPI1 for the REE. With this setting, all SPI flash read, write, and erase operations are routed through service calls to the TEE. While this option provides enhanced security, it introduces some performance overhead.

The table below shows the rough time taken to read and write to a 1MB partition in 256B chunks with :doc:`../../api-reference/storage/partition`, highlighting the impact of ESP-TEE and the :ref:`CONFIG_SECURE_TEE_EXT_FLASH_MEMPROT_SPI1` configuration.

.. list-table:: Flash Protection: Performance Impact
   :header-rows: 1

   * - Case
     - Read (ms)
     - Read Δ (ms)
     - Read Δ (%)
     - Write (ms)
     - Write Δ (ms)
     - Write Δ (%)
   * - ESP-TEE disabled
     - 262.01
     - -
     - -
     - 3394.23
     - -
     - -
   * - ESP-TEE enabled
     - 279.86
     - +17.85
     - +6.81%
     - 3415.64
     - +21.41
     - +0.63%
   * - ESP-TEE + SPI1 protected
     - 359.73
     - +97.72
     - +37.33%
     - 3778.65
     - +384.42
     - +11.32%

Peripherals
~~~~~~~~~~~

The following peripherals are protected using the APM module and accessible only by the TEE. Any direct access to these peripherals from the REE will trigger an exception. These peripherals are chosen for protection because access from the REE could compromise system security or interfere with secure services like secure storage and attestation.

.. list::

    - Access Permission Management (APM) peripheral
    - AES, SHA accelerators
    - ECC accelerator
    - Hash-Based Message Authentication Code (HMAC) module
    - Digital Signature module
    - eFuse Controller
    - Interrupt Controller
    - Brownout Detector
    - Super Watchdog Timer (SWDT)

.. note::

  -  The following peripherals will be secured in future releases -

     - MPI accelerator (RSA)

Firmware
^^^^^^^^

The TEE firmware is primarily loaded and executed from the internal secure SRAM. However, due to SRAM's limited capacity, some portions of the TEE firmware are stored and executed from a dedicated external flash partition, which is protected from access by the REE. This firmware is responsible for securely configuring the system, ensuring isolation and protection of both internal and external memory, as well as peripherals.

System Initialization
~~~~~~~~~~~~~~~~~~~~~

The TEE handles the secure initialization of the SoC's resources, including:

    * Setting the TEE vector table, the designated secure interrupt and the interrupts to be delegated to the REE
    * Configuring the isolation boundaries between TEE and REE by setting up -

        - RISC-V PMP/PMA for internal memory (I/DRAM) and external memory (Flash)
        - APM for peripherals (AES, SHA, eFuse, MMU, etc.)

    * Registering the TEE heap

Interrupts
~~~~~~~~~~

The TEE and REE each have their own vector tables, with one interrupt pin dedicated to secure interrupts. TEE interrupts are always given higher priority over REE interrupts. The TEE firmware provides APIs for registering secure, peripheral-specific interrupt handlers.

Depending on the interrupt state, an interrupt may either be handled within the current execution environment's vector table or trigger a privilege switch, transferring the handling to the other environment's vector table. There are four possible scenarios for interrupt handling, as outlined in the table below.

Once the interrupt is handled in the other execution environment, execution returns to the environment where the interrupt was initially triggered.

+-------------------------------------------+---------------------------------------------------------+
| Scenario                                  | Description                                             |
+===========================================+=========================================================+
| TEE interrupts while operating in TEE     | Trapped in the TEE at the M-mode exception handler      |
+-------------------------------------------+---------------------------------------------------------+
| REE interrupts while operating in REE     | Trapped in the REE at the U-mode exception handler      |
+-------------------------------------------+---------------------------------------------------------+
| TEE interrupts while operating in REE     | Trapped in the TEE at the M-mode exception handler and  |
|                                           | return to the REE after processing                      |
+-------------------------------------------+---------------------------------------------------------+
| REE interrupts while operating in TEE     | Trapped in the TEE at the M-mode exception handler,     |
|                                           | jump to the U-mode exception handler in the REE and     |
|                                           | return to the TEE after processing                      |
+-------------------------------------------+---------------------------------------------------------+

.. seqdiag::
    :caption: ESP-TEE: Handling TEE interrupts occurring in REE
    :align: center

    seqdiag esp_tee_m_u_intr {
        activation = none;
        node_width = 120;
        node_height = 80;
        edge_length = 500;
        default_shape = roundedbox;
        default_fontsize = 12;

        REE  [label = "REE\n(User mode)"];
        TEE  [label = "TEE\n(Machine mode)"];

        === TEE interrupts in REE ===
        REE -> REE[label = "REE software\nexecution"];
        REE -> TEE [label = "TEE interrupt", rightnote = "Jump to the M-mode\nexception handler"];
        TEE -> TEE [rightnote = "Save the U-mode context\nSwitch to the M-mode\ninterrupt stack"];
        TEE -> TEE [label = "Service Routine"];
        TEE -> REE [label = "Return to REE", rightnote = "Restore the U-mode context\n(mret instruction)"];
        REE -> REE[label = "REE software\nexecution"];
    }


.. figure:: ../../../_static/esp_tee/{IDF_TARGET_PATH_NAME}/esp_tee_intr_handling.png
    :align: center
    :alt: ESP-TEE: Interrupt Handling
    :figclass: align-center

    ESP-TEE: Handling REE interrupts occurring in TEE


Secure Services
---------------

Execution Flow
^^^^^^^^^^^^^^

The secure service call interface allows the REE application to request trusted (TEE) operations, triggering the secure service dispatcher function. The dispatcher parses the input parameters, identifies the correct service to invoke, and forwards the request to the appropriate service handler.

Serving as the entry point to the TEE, the dispatcher manages the secure service call. Upon receiving a request, it processes the input arguments and executes the requested service.

After the secure service completes, the dispatcher handles the return process and initiates a privilege switch, restoring control to the REE at the point of the original call. Note that the privilege levels are switched as a part of the pre and post processing routines of the secure service dispatcher.

.. seqdiag::
    :caption: ESP-TEE: Secure Service Call Trace
    :align: center

    seqdiag esp_tee_ss_call_trace {
        activation = none;
        node_width = 120;
        node_height = 80;
        edge_length = 500;
        default_shape = roundedbox;
        default_fontsize = 12;


        REE  [label = "REE\n(User mode)"];
        TEE  [label = "TEE\n(Machine mode)"];

        === Secure Service Call Interface ===
        REE -> REE[label = "REE software\nexecution"];
        REE -> TEE [label = "Secure service call", rightnote = "(ecall instruction)\nJump to M-mode exception handler"];
        TEE -> TEE [leftnote = "Save the U-mode context\nSwitch to the M-mode stack\nDisable U-mode interrupt delegation"];
        TEE -> TEE [label = "Secure service\nexecution"];
        TEE -> REE [label = "Return to REE", rightnote = "Restore the U-mode context\nEnable U-mode interrupt delegation\n(mret instruction)"];
        REE -> REE[label = "REE software\nexecution"];
    }

The convention for the secure service call interface API :cpp:func:`esp_tee_service_call` is defined as follows.

.. list-table:: Secure Service calling convention
         :widths: 15 20 40
         :header-rows: 1

         * - **Argument**
           - **Type**
           - **Description**
         * - ``arg0``
           - ``uint8_t``
           - Input: Number of input arguments to the secure service
         * - ``arg1``
           - ``tee_secure_service_t``
           - Input: Secure Service ID
         * - ``arg2`` - ``arg9``
           - ``uint32_t``
           - Input: Arguments to the secure service
         * - ``val``
           - ``uint32_t``
           - Output: Return value from the secure service

.. note::

    - An IRAM-safe variant of the API, :cpp:func:`esp_tee_service_call_with_noniram_intr_disabled`, suspends the scheduler and disables all non-IRAM resident interrupts before switching execution environments.

    - This is essential during operations like flash access through the SPI1 bus, where the flash cache may be disabled, to prevent concurrent access to flash from multiple entities.


.. _custom-secure-services:

Adding Custom Secure Services
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To extend the ESP-TEE framework with custom service calls, follow the steps outlined below.

1. Create a Custom Service Call Table
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Define a component for defining custom service calls and create a ``.yml`` file within the component.

.. code-block:: bash

   touch <path/to/yml/file>/custom_srvcall.yml

Add your custom service call entries to the ``.yml`` file in the following format:

.. code-block:: yaml

  secure_services:
  - family: <api_family>
    entries:
      - id: <service_call_number>
        type: custom
        function: <function_name>
        args: <arguments_count>

**Example Entry**

.. code-block:: yaml

  secure_services:
    - family: example
      entries:
        - id: 300
          type: custom
          function: example_sec_serv_aes_op
          args: 5


- ``300``: Unique service call number
- ``custom``: Custom service call type
- ``example_sec_serv_aes_op``: Function name
- ``5``: Number of arguments

Ensure that the custom service call numbers does not conflict with the :component_file:`default service call table<esp_tee/scripts/{IDF_TARGET_PATH_NAME}/sec_srv_tbl_default.yml>`. The ESP-TEE framework parses the custom service call table along with the default table to generate relevant header files used in applications.

2. Define the Service Call Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Define the function corresponding to the custom service call in the TEE. This function will execute whenever the REE invokes the service with the associated number.

**Example Implementation**

.. code-block:: c

   int _ss_custom_sec_srv_op(void *arg)
   {
       // Perform the intended task
       return 0;
   }

The function name should have the prefix ``_ss_`` before the name and must match the name specified in the ``.yml`` file.

For reference, all default service call functions are defined in the :component_file:`file<esp_tee/subproject/main/core/esp_secure_services.c>`.

3. Build-system changes
~~~~~~~~~~~~~~~~~~~~~~~

**Custom component-level**

Define a CMake file (e.g., ``custom_sec_srv.cmake``) in the component that defines the custom service calls. Add the following configurations.

#. Append the service call table to the default table

   .. code-block:: cmake

     idf_build_set_property(CUSTOM_SECURE_SERVICE_YAML ${CMAKE_CURRENT_LIST_DIR}/custom_srvcall.yml APPEND)

#. Set the custom component directory and name so that the ``esp_tee`` subproject can use it

   .. code-block:: cmake

     get_filename_component(directory "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE DIRECTORY)
     idf_build_set_property(CUSTOM_SECURE_SERVICE_COMPONENT_DIR ${directory} APPEND)

     get_filename_component(name ${CMAKE_CURRENT_LIST_DIR} NAME)
     idf_build_set_property(CUSTOM_SECURE_SERVICE_COMPONENT ${name} APPEND)

#. Define the ``CMakeLists.txt`` for the custom component

   .. code-block:: cmake

      idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

      if(esp_tee_build)
      ## Headers, sources and dependent components for the TEE-build
      else()
      ## Headers, sources and dependent components for the REE-build
      endif()

      idf_component_register(...)

**Project-level**

Modify the top-level ``CMakeLists.txt`` of your project to include the ``custom_sec_srv.cmake`` file before calling the ``project()`` command.

.. code-block:: cmake

  include(<path/to/component>/custom_sec_srv.cmake)

  project(your_project_name)

.. note::

  For more details, refer to the :example:`tee_basic <security/tee/tee_basic>` example.

Unit Testing
------------

The ESP-TEE framework utilizes the :doc:`pytest in ESP-IDF <../../contribute/esp-idf-tests-with-pytest>` framework for executing the dedicated unit tests on the target. The test application includes cases spanning the modules listed below and can be located in the ``components`` directory at :component:`esp_tee/test_apps`.

- Secure service call interface
- Interrupts and exception handling
- Privilege violation
- Cryptographic operations
- TEE OTA updates
- Secure storage
- Attestation

Please follow the steps given :ref:`here <run_the_tests_locally>` for running the tests locally. For example, if you want to execute the TEE test suite for {IDF_TARGET_NAME} with all the available ``sdkconfig`` files (assuming you have ESP-IDF setup), run the following steps.

.. code-block:: bash

  $ cd components/esp_tee/test_apps/tee_test_fw
  $ idf.py build
  $ pytest --target <target>
