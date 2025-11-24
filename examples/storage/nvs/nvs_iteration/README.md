| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- |

# Non-Volatile Storage (NVS) Iteration Example

This example showcases how to iterate and obtain info about NVS entries of a specific (or any) NVS datatype.

Default "nvs" partition is first erased to allow for clean example run, followed by writing 2 sets of key value pairs of different types to NVS storage.
After that, iteration is performed over the individual data types, as well as the generic `NVS_TYPE_ANY`, and relevant entry info gained from iteration is verbosely logged.

Iterator creation in this example is performed by [nvs_entry_find()](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html#_CPPv414nvs_entry_findPKcPKc10nvs_type_tP14nvs_iterator_t) and accessing entry info by [nvs_entry_info()](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html#_CPPv414nvs_entry_infoK14nvs_iterator_tP16nvs_entry_info_t).


Detailed functional description of NVS and API is provided in [documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_flash.html).

## How to use example

### Hardware required

This example does not require any special hardware, and can be run on any common development board.

### Build and flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
...
I (265) nvs_iteration_example: Erasing the contents of the default NVS partition...
I (595) nvs_iteration_example: Opening Non-Volatile Storage (NVS) handle for namespace '_mock_data'...
I (605) nvs_iteration_example: Writing u32 mock data key-value pairs to NVS namespace '_mock_data'...
I (605) nvs_iteration_example: Wrote 7 u32 mock data key-value pairs to NVS namespace '_mock_data'.
I (615) nvs_iteration_example: Writing i8 mock data key-value pairs to NVS namespace '_mock_data'...
I (625) nvs_iteration_example: Wrote 7 i8 mock data key-value pairs to NVS namespace '_mock_data'.
I (625) nvs_iteration_example: Committing data in NVS namespace '_mock_data'...
I (635) nvs_iteration_example: NVS handle for namespace '_mock_data' closed.
I (645) nvs_iteration_example: Iterating over NVS_TYPE_U32 data in namespace '_mock_data'...
I (655) nvs_iteration_example: Entry info: key 'system_startup' for data type type NVS_TYPE_U32 in namespace '_mock_data'
I (665) nvs_iteration_example: Entry info: key 'user_login' for data type type NVS_TYPE_U32 in namespace '_mock_data'
I (675) nvs_iteration_example: Entry info: key 'data_backup' for data type type NVS_TYPE_U32 in namespace '_mock_data'
I (685) nvs_iteration_example: Entry info: key 'sched_update' for data type type NVS_TYPE_U32 in namespace '_mock_data'
I (695) nvs_iteration_example: Entry info: key 'db_cleanup' for data type type NVS_TYPE_U32 in namespace '_mock_data'
I (705) nvs_iteration_example: Entry info: key 'security_scan' for data type type NVS_TYPE_U32 in namespace '_mock_data'
I (715) nvs_iteration_example: Entry info: key 'system_shutdown' for data type type NVS_TYPE_U32 in namespace '_mock_data'
I (725) nvs_iteration_example: Iterated over 7 entries.
I (725) nvs_iteration_example: Iterating over NVS_TYPE_I8 data in namespace '_mock_data'...
I (735) nvs_iteration_example: Entry info: key 'temperature' for data type type NVS_TYPE_I8 in namespace '_mock_data'
I (745) nvs_iteration_example: Entry info: key 'humidity' for data type type NVS_TYPE_I8 in namespace '_mock_data'
I (755) nvs_iteration_example: Entry info: key 'light_level' for data type type NVS_TYPE_I8 in namespace '_mock_data'
I (765) nvs_iteration_example: Entry info: key 'motion_detected' for data type type NVS_TYPE_I8 in namespace '_mock_data'
I (775) nvs_iteration_example: Entry info: key 'battery_level' for data type type NVS_TYPE_I8 in namespace '_mock_data'
I (785) nvs_iteration_example: Entry info: key 'signal_strength' for data type type NVS_TYPE_I8 in namespace '_mock_data'
I (795) nvs_iteration_example: Entry info: key 'error_code' for data type type NVS_TYPE_I8 in namespace '_mock_data'
I (805) nvs_iteration_example: Iterated over 7 entries.
I (815) nvs_iteration_example: Iterating over NVS_TYPE_ANY data in namespace '_mock_data'...
I (825) nvs_iteration_example: Iterated over 14 entries.
I (825) nvs_iteration_example: Returning from app_main().
I (835) main_task: Returned from app_main()
...
```