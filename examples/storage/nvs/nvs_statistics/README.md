| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |

# Non-Volatile Storage (NVS) Statistics Example

This example demonstrates the usage of obtaining and interpreting statistics about the a given NVS partition, namely free/used/available/total entries and namespace count.

The default "nvs" partition is first erased, then a mock string data configuration is written to 2 different namespaces, followed by checking the changed statistics and mainly the number of newly used NVS entries.

Statistics obtained via [nvs_get_stats()](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/nvs_flash.html#_CPPv413nvs_get_statsPKcP11nvs_stats_t) are the following:
|                |                     |
| -------------- | ------------------- |
| `available entries` | Essential statistic, specifies free non-reserved entries available for data storage. |
| `used entries` | Includes 1 overhead entry per namespace + actual data entries used by data storage. |
| `free entries` | Free entries are both the free available to the user entries, and free but internally reserved (not available to the user). |
| `total entries` | Number of all entries (free + used). |


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
I (265) nvs_statistics_example: Erasing the contents of the default NVS partition...
I (475) nvs_statistics_example: Opening Non-Volatile Storage (NVS) handle for namespace '_mock_data'...
I (485) nvs_statistics_example: Getting NVS statistics...
I (485) nvs_statistics_example: NVS statistics:
I (485) nvs_statistics_example: Used NVS entries: 1
I (485) nvs_statistics_example: Free NVS entries: 755
I (495) nvs_statistics_example: Available NVS entries: 629
I (495) nvs_statistics_example: Total NVS entries: 756
I (505) nvs_statistics_example: Namespace count: 1
I (505) nvs_statistics_example: Writing mock data key-value pairs to NVS namespace '_mock_data'...
I (525) nvs_statistics_example: Committing data in NVS namespace '_mock_data'...
I (525) nvs_statistics_example: Getting post-commit NVS statistics...
I (525) nvs_statistics_example: NVS statistics:
I (535) nvs_statistics_example: Used NVS entries: 30
I (535) nvs_statistics_example: Free NVS entries: 726
I (545) nvs_statistics_example: Available NVS entries: 600
I (545) nvs_statistics_example: Total NVS entries: 756
I (555) nvs_statistics_example: Namespace count: 1
I (555) nvs_statistics_example: Newly used entries match expectation.
I (565) nvs_statistics_example: Newly used entries: 29, expected: 29.
I (575) nvs_statistics_example: NVS handle for namespace '_mock_data' closed.
I (575) nvs_statistics_example: Opening Non-Volatile Storage (NVS) handle for namespace '_mock_data'...
I (585) nvs_statistics_example: Reading stored data from NVS namespace '_mock_data'...
I (595) nvs_statistics_example: Read key-value pair from NVS: 'wifi_ssid':'HomeNetwork'
I (605) nvs_statistics_example: Read key-value pair from NVS: 'wifi_pass':'MySecretPass'
I (605) nvs_statistics_example: Read key-value pair from NVS: 'dev_name':'LivingRoomThermostat'
I (615) nvs_statistics_example: Read key-value pair from NVS: 'temp_unit':'Celsius'
I (625) nvs_statistics_example: Read key-value pair from NVS: 'target_temp':'22'
I (635) nvs_statistics_example: Read key-value pair from NVS: 'eco_mode':'false'
I (635) nvs_statistics_example: Read key-value pair from NVS: 'fw_version':'1.2.3'
I (645) nvs_statistics_example: Read key-value pair from NVS: 'led_bright':'80'
I (655) nvs_statistics_example: Read key-value pair from NVS: 'auto_update':'true'
I (665) nvs_statistics_example: Read key-value pair from NVS: 'last_sync':'2025-01-01T08:00:00Z'
I (665) nvs_statistics_example: Read key-value pair from NVS: 'user_lang':'en'
I (675) nvs_statistics_example: Read key-value pair from NVS: 'long_token':'2f8c1e7b5a4d9c6e3b0f1a8e5d7c2b6f4e1a9c7b'
I (685) nvs_statistics_example: Read key-value pair from NVS: 'very_long_token':'7e2b1c9f5a4d8e3b0f1a6c7e2d9b5a4c8e1f7b2d6c3a9e5b0f1a8c7e2d9b5a4c8e1f7b2d6c3a9e5b'
I (705) nvs_statistics_example: NVS handle for namespace '_mock_data' closed.
I (705) nvs_statistics_example: Opening Non-Volatile Storage (NVS) handle for namespace '_mock_backup'...
I (715) nvs_statistics_example: Getting NVS statistics...
I (725) nvs_statistics_example: NVS statistics:
I (725) nvs_statistics_example: Used NVS entries: 31
I (735) nvs_statistics_example: Free NVS entries: 725
I (735) nvs_statistics_example: Available NVS entries: 599
I (745) nvs_statistics_example: Total NVS entries: 756
I (745) nvs_statistics_example: Namespace count: 2
I (755) nvs_statistics_example: Writing mock data key-value pairs to NVS namespace '_mock_backup'...
I (765) nvs_statistics_example: Committing data in NVS namespace '_mock_backup'...
I (765) nvs_statistics_example: Getting post-commit NVS statistics...
I (775) nvs_statistics_example: NVS statistics:
I (775) nvs_statistics_example: Used NVS entries: 60
I (785) nvs_statistics_example: Free NVS entries: 696
I (785) nvs_statistics_example: Available NVS entries: 570
I (795) nvs_statistics_example: Total NVS entries: 756
I (795) nvs_statistics_example: Namespace count: 2
I (805) nvs_statistics_example: Newly used entries match expectation.
I (805) nvs_statistics_example: Newly used entries: 29, expected: 29.
I (815) nvs_statistics_example: NVS handle for namespace '_mock_backup' closed.
I (825) nvs_statistics_example: Opening Non-Volatile Storage (NVS) handle for namespace '_mock_backup'...
I (835) nvs_statistics_example: Reading stored data from NVS namespace '_mock_backup'...
I (835) nvs_statistics_example: Read key-value pair from NVS: 'wifi_ssid':'HomeNetwork'
I (845) nvs_statistics_example: Read key-value pair from NVS: 'wifi_pass':'MySecretPass'
I (855) nvs_statistics_example: Read key-value pair from NVS: 'dev_name':'LivingRoomThermostat'
I (865) nvs_statistics_example: Read key-value pair from NVS: 'temp_unit':'Celsius'
I (865) nvs_statistics_example: Read key-value pair from NVS: 'target_temp':'22'
I (875) nvs_statistics_example: Read key-value pair from NVS: 'eco_mode':'false'
I (885) nvs_statistics_example: Read key-value pair from NVS: 'fw_version':'1.2.3'
I (895) nvs_statistics_example: Read key-value pair from NVS: 'led_bright':'80'
I (895) nvs_statistics_example: Read key-value pair from NVS: 'auto_update':'true'
I (905) nvs_statistics_example: Read key-value pair from NVS: 'last_sync':'2025-01-01T08:00:00Z'
I (915) nvs_statistics_example: Read key-value pair from NVS: 'user_lang':'en'
I (925) nvs_statistics_example: Read key-value pair from NVS: 'long_token':'2f8c1e7b5a4d9c6e3b0f1a8e5d7c2b6f4e1a9c7b'
I (935) nvs_statistics_example: Read key-value pair from NVS: 'very_long_token':'7e2b1c9f5a4d8e3b0f1a6c7e2d9b5a4c8e1f7b2d6c3a9e5b0f1a8c7e2d9b5a4c8e1f7b2d6c3a9e5b'
I (945) nvs_statistics_example: NVS handle for namespace '_mock_backup' closed.
I (955) nvs_statistics_example: Returning from app_main().
I (955) main_task: Returned from app_main()
...
```