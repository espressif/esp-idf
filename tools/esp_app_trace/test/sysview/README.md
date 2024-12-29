# Steps to Generate Test Input Files

1. `sysview_tracing_heap_log.elf`

    Build example `sysview_tracing_heap_log` project.

    ```
    cd $IDF_PATH/examples/system/sysview_tracing_heap_log
    idf.py set target esp32
    idf.py build
    cp build/sysview_tracing_heap_log.elf $IDF_PATH/tools/esp_app_trace/test/sysview/
    cd $IDF_PATH/tools/esp_app_trace/test/sysview/
    chmod -x sysview_tracing_heap_log.elf
    ```

2. `heap_log0.svdat` and `heap_log1.svdat`

    Flash the example and run OpenOCD and GDB using the following command in a separate console

    ```
    cd $IDF_PATH/examples/system/sysview_tracing_heap_log
    idf.py openocd
    ```

    ```
    cd $IDF_PATH/examples/system/sysview_tracing_heap_log
    xtensa-esp32-elf-gdb -x gdbinit build/sysview_tracing_heap_log.elf
    ```
    When program stops at `heap_trace_stop`  quit GDB and OpenOCD

    Copy generated svdat files
    ```
    cp heap_log0.svdat $IDF_PATH/tools/esp_app_trace/test/sysview/
    cp heap_log1.svdat $IDF_PATH/tools/esp_app_trace/test/sysview/
    ```

3. `heap_log_mcore.svdat`

    Flash the example and run OpenOCD and GDB using the following command in a separate console

    ```
    cd $IDF_PATH/examples/system/sysview_tracing_heap_log
    idf.py openocd
    ```

    ```
    cd $IDF_PATH/examples/system/sysview_tracing_heap_log
    xtensa-esp32-elf-gdb -x gdbinit-mcore build/sysview_tracing_heap_log.elf
    ```
    When program stops at `heap_trace_stop`  quit GDB and OpenOCD

    Copy generated svdat file
    ```
    cp heap_log_mcore.svdat $IDF_PATH/tools/esp_app_trace/test/sysview/
    ```

4. `expected_output` and `expected_output_json` files

    You can use the commands from the `test.sh` to generate expected result files

    ```
    $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -d -p -b sysview_tracing_heap_log.elf heap_log0.svdat heap_log1.svdat &> expected_output
    $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -j -b sysview_tracing_heap_log.elf heap_log0.svdat heap_log1.svdat &> expected_output.json
    $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -d -p -b sysview_tracing_heap_log.elf heap_log_mcore.svdat &> expected_output_mcore
    $IDF_PATH/tools/esp_app_trace/sysviewtrace_proc.py -j -b sysview_tracing_heap_log.elf heap_log_mcore.svdat &> expected_output_mcore.json
    ```
