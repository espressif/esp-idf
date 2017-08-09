Logging library
===============

Overview
--------

Log library has two ways of managing log verbosity: compile time, set via menuconfig; and runtime, using ``esp_log_level_set`` function.

At compile time, filtering is done using ``CONFIG_LOG_DEFAULT_LEVEL`` macro, set via menuconfig. All logging statments for levels higher than ``CONFIG_LOG_DEFAULT_LEVEL`` will be removed by the preprocessor.

At run time, all logs below ``CONFIG_LOG_DEFAULT_LEVEL`` are enabled by default. ``esp_log_level_set`` function may be used to set logging level per module. Modules are identified by their tags, which are human-readable ASCII zero-terminated strings.

How to use this library
-----------------------

In each C file which uses logging functionality, define TAG variable like this:

.. code-block:: c

   static const char* TAG = "MyModule";

then use one of logging macros to produce output, e.g:

.. code-block:: c

   ESP_LOGW(TAG, "Baud rate error %.1f%%. Requested: %d baud, actual: %d baud", error * 100, baud_req, baud_real);

Several macros are available for different verbosity levels:

* ``ESP_LOGE`` - error
* ``ESP_LOGW`` - warning
* ``ESP_LOGI`` - info
* ``ESP_LOGD`` - debug
* ``ESP_LOGV`` - verbose

Additionally there is an _EARLY_ variant for each of these macros (e.g. ``ESP_EARLY_LOGE`` ).These variants can run in startup code, before heap allocator and syscalls have been initialized. When compiling bootloader, normal ``ESP_LOGx`` macros fall back to the same implementation as ``ESP_EARLY_LOGx`` macros. So the only place where ``ESP_EARLY_LOGx`` have to be used explicitly is the early startup code, such as heap allocator initialization code.

(Note that such distinction would not have been necessary if we would have an ``ets_vprintf`` function in the ROM. Then it would be possible to switch implementation from _EARLY_ version to normal version on the fly. Unfortunately, ``ets_vprintf`` in ROM has been inlined by the compiler into ``ets_printf``, so it is not accessible outside.)

To override default verbosity level at file or component scope, define ``LOG_LOCAL_LEVEL`` macro. At file scope, define it before including ``esp_log.h``, e.g.:

.. code-block:: c

   #define LOG_LOCAL_LEVEL ESP_LOG_VERBOSE
   #include "esp_log.h"


At component scope, define it in component makefile:

.. code-block:: make

   CFLAGS += -D LOG_LOCAL_LEVEL=ESP_LOG_DEBUG

To configure logging output per module at runtime, add calls to ``esp_log_level_set`` function:

.. code-block:: c

   esp_log_level_set("*", ESP_LOG_ERROR);        // set all components to ERROR level
   esp_log_level_set("wifi", ESP_LOG_WARN);      // enable WARN logs from WiFi stack
   esp_log_level_set("dhcpc", ESP_LOG_INFO);     // enable INFO logs from DHCP client

Logging to Host via JTAG
^^^^^^^^^^^^^^^^^^^^^^^^

By default logging library uses vprintf-like function to write formatted output to dedicated UART. In general it invloves the following steps:

1. Format string is parsed to obtain type of each argument.
2. According to its type every argument is converted to string representation.
3. Format string combined with converted arguments is sent to UART.

Though implementation of vprintf-like function can be optimised to a certain level, all steps above have to be peformed in any case and every step takes some time (especially item 3). So it is frequent situation when addition of extra logging to the program to diagnose some problem changes its behaviour and problem dissapears or in the worst cases program can not work normally at all and ends up with an error or even hangs. 
Possible ways to overcome this problem are to use faster UART bitrates (or another faster interface) and/or move string formatting procedure to the host.
ESP IDF has `Application Tracing` feature which allows to sent arbitrary application data to host via JTAG. This feature can also be used to transfer log information to host using ``esp_apptrace_vprintf`` function. This function does not perform full parsing of the format string and arguments, instead it just calculates number of arguments passed and sends them along with the format string address to the host. On the host log data are processed and printed out by a special Python script.

Config Options and Dependencies
"""""""""""""""""""""""""""""""

Using of the feature depends on two components:

1. Host side: Application tracing is done over JTAG, so it needs OpenOCD to be set up and running on host machine. For instructions how to set it up, please, see :idf:`OpenOCD setup for ESP32` section for details. **NOTE:** `in order to achieve higher data rates you may need to modify JTAG adapter working frequency in OpenOCD config file. Maximum tested stable speed is 26MHz, so you need to have the following statement in your configuration file` ``adapter_khz 26000`` `instead of default` ``adapter_khz 200``. `Actually maximum stable JTAG frequency can depend on host system configuration.`
2. Target side: Application tracing functionality can be enabled by ``CONFIG_ESP32_APPTRACE_ENABLE`` macro via menuconfig. This option enables the module and makes ``esp_apptrace_vprintf`` available for users.

Limitations
"""""""""""

Curent implmentation of logging over JTAG has several limitations:

1. Tracing from ``ESP_EARLY_LOGx`` macros is not supported.
2. No support for printf arguments which size exceeds 4 bytes (e.g. ``double`` and ``uint64_t``).
3. Only strings from .rodata section are supported as format strings and arguments.
4. Maximum number of printf arguments is 256.

How To Use It
"""""""""""""

To use logging via JTAG user needs to perform the following steps:

1. On target side special vprintf-like function needs to be installed. As it was mentioned earlier this function is ``esp_apptrace_vprintf``. It sends log data to the host via JTAG. Example code is shown below.

.. code-block:: c

    #include "esp_app_trace.h"
    ...
    void app_main()
    {
        // set log vprintf handler
        esp_log_set_vprintf(esp_apptrace_vprintf);
        ...
        // user code using ESP_LOGx starts here
        // all data passed to ESP_LOGx are sent to host
        ...
        // restore log vprintf handler
        esp_log_set_vprintf(vprintf);
        // flush last data to host
        esp_apptrace_flush(ESP_APPTRACE_DEST_TRAX, 100000 /*tmo in us*/);
        ESP_LOGI(TAG, "Tracing is finished."); // this will be printed out to UART
        while (1);
    }

2. Build the program image and download it to target as described in :idf:`Developing With the ESP-IDF` section.
3. Run OpenOCD (see :idf:`OpenOCD setup for ESP32` section).
4. Connect to OpenOCD telnet server. On Linux it can be done using the following command in terminal ``telnet <oocd_host> 4444``. If telnet session is opened on the same machine which runs OpenOCD you can use `localhost` as `<oocd_host>` in the command.
5. Run the following command in OpenOCD telnet session: ``esp108 apptrace start /path/to/trace/file 0 -1 -1 1``. This command will wait for board reset and transfer tracing data at the highest possible rate.
6. Reset the board. Logging to host will start automatically.
7. ``esp108 apptrace`` command with given arguments will never return (see other command options below), so you must stop it manually by resetting the board or pressing CTRL+C in OpenOCD window (not one with the telnet session).
8. Reset board or press CTRL+C in OpenOCD window (not one with the telnet session) when tracing is completed (for the example code above after the message `"Tracing is finished."` appears on UART).
9. To print out collected log records run the following command in terminal: ``$IDF_PATH/tools/esp_app_trace/logtrace_proc.py /path/to/trace/file /path/to/program/elf/file``.

OpenOCD Application Tracing Command Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Command usage:
``esp108 apptrace [start <outfile> [options] | [stop] | [status] | [dump <outfile>]``

Sub-commands:
  * ``start``.  Start tracing (continuous streaming).
  * ``stop``.   Stop tracing.
  * ``status``. Get tracing status.
  * ``dump``.   Dump as much data as possible without waiting for trace memory block switch (post-mortem dump).

Start command syntax:
  ``start <outfile> [trace_size [stop_tmo [skip_size [poll_period [wait4halt]]]]]``

  .. list-table::
    :widths: 20 80
    :header-rows: 1

    * - Argument
      - Description
    * - outfile
      - Path to log trace file to save data
    * - trace_size
      - Maximum size of data to collect (in bytes). Tracing is stopped after specified amount of data is received. By default -1 (trace size stop trigger is disabled).
    * - stop_tmo
      - Idle timeout (in ms). Tracing is stopped if there is no data for specified period of time. By default 10 s (-1 to disable this stop trigger).
    * - skip_size
      - Number of bytes to skip at the start. By default 0.
    * - poll_period
      - Data polling period (in ms). If greater then 0 then command runs in non-blocking mode, otherwise command line will not be avalable until tracing is stopped. By default 1 ms.
    * - wait4halt
      - If 0 start tracing immediately, otherwise command waits for the target to be halted (after reset, by breakpoint etc) and then automatically resumes it and starts tracing. By default 0.    

Log Trace Processor Command Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Command usage:
``logtrace_proc.py [-h] [--no-errors] <trace_file> <elf_file>``

Positional arguments:

  .. list-table::
    :widths: 20 80
    :header-rows: 1

    * - Argument
      - Description
    * - trace_file
      - Path to log trace file
    * - elf_file
      - Path to program ELF file

Optional arguments:

  .. list-table::
    :widths: 20 80
    :header-rows: 1

    * - Argument
      - Description
    * - -h, --help
      - show this help message and exit
    * - --no-errors, -n
      - Do not print errors

