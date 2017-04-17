External RAM
============

.. toctree::
   :maxdepth: 1

The ESP32 has the capability to address up to 4MiB of RAM connected to it over the SPI bus. It can use this memory in a way that is similar to the internal 
DRAM, optionally adding it to the heap so existing programs can easily make use of it.

ESP-IDF Support
---------------
Although in theory a wide range of chips and hardware configurations are possible, at the moment esp-idf only supports ESP-PSRAM32 or IS25WP032 chips. Such 
a chip needs to be connected in parallel with the main SPI flash, with the exception of the CS and CLK line, these need to be connected to GPIO 16 and 17
respectively. If you use a module like the ESP-WROVER, the hardware will already have this configuration out of the box.


ESP32 support
-------------
While all revisions of the ESP32 have support for external RAM, there are some bugs in the silicon of the first two revisions. Revision 0 has multiple bugs including
a severe one affecting read/modify/write behaviour; because of this bug, using PSRAM on this revision is not supported. Revision 1 solves the most
severe issues, but still needs working around some things in order to function. For ESP-IDF, these workarounds are implemented as such:

- The compiler is passed the -mfix-esp32-psram-cache-issue flag. This instructs it to recognize problematic sequences of instructions and replace it by
  safer ones.

- A version of Newlib is used that is compiled with this compiler flag. This means that none of the Newlib functions in ROM are used. For now, this
  unfortunately also means that when using PSRAM with this workaround, using the Newlib ROM stubs to redirect Newlib behaviour is not supported.
