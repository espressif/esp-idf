# Core Dump Userdata example

The example demonstrates prepending user-specified data to the coredump written to flash.
The example defines two functions which hook into esp-idf:

   - esp_core_dump_userdata_get()
   - esp_core_dump_userdata_get_len()

The buffer returned by these functions is prepended to the coredump written to flash.
It is the user's responsibility to remove this userdata prior to feeding the coredump
into espcoredump.py for decoding.

Uses of core dump userdata include:

 1. to embed application version number and build number, to allow for correct attribution of automated crash logs,
    and to allow the corresponing ELF file to be found when attempting to decode coredumps from the field
 2. to embed error or message logs to give more context to the crash

See the README.md file in the upper level 'examples' directory for more information about examples.
