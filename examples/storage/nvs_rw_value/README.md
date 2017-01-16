# Non-Volatile Storage (NVS) Read and Write Example

Demonstrates how to read and write a single integer value using NVS.

The value holds the number of ESP32 module restarts. Since it is written to NVS, the value is preserved between restarts.

Example also shows how to check if read / write operation was successful, or certain value is not initialized in NVS. Diagnostic is provided in plain text to help track program flow and capture any issues on the way.

Check another example *storage/nvs_rw_blob*, that shows how to read and write variable length binary data (blob).

Detailed functional description of NVS and API is provided in [documentation](http://esp-idf.readthedocs.io/en/latest/api/nvs_flash.html).

See the README.md file in the upper level 'examples' directory for more information about examples.
