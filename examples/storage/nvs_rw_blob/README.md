# Non-Volatile Storage (NVS) Read and Write Example

Demonstrates how to read and write a single integer value and a blob (binary large object) using NVS to preserve them between ESP32 module restarts.

  * value - tracks number of ESP32 module soft and hard restarts.
  * blob - contains a table with module run times. The table is read from NVS to dynamically allocated RAM. New run time is added to the table on each manually triggered soft restart and written back to NVS. Triggering is done by pulling down GPIO0.

Example also shows how to implement diagnostics if read / write operation was successful. 

If not done already, consider checking simpler example *storage/nvs_rw_value*, that has been used as a starting point for preparing this one. 

Detailed functional description of NVS and API is provided in [documentation](https://esp-idf.readthedocs.io/en/latest/api-reference/nvs_flash.html).

See the README.md file in the upper level 'examples' directory for more information about examples.
