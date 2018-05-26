Wear Levelling Component
========================

Wear Levelling Component (WLC) it is a software component that is implemented to prevent situation when some sectors in flash memory used by erase operations more then others. The component shares access attempts between all avalible sectors.
The WLC do not have internal cache. When write operation is finished, that means that data was really stored to the flash.
As a parameter the WLC requires the driver to access the flash device. The driver has to implement Flash_Access interface.


The WLC Files
^^^^^^^^^^^^^^^
The WLC consist of few components that are implemented in different files. The list and brief description of these components written below.

 - Flash_Access - memory access interface. Used to access the memory. A classes WL_Flash, Partition, SPI_Flash are implements this interface.
 - SPI_Flash - class implements the Flash_Access interface to provide access to the flash memory.
 - Partition - class implements the Flash_Access interface to provide access to the partition.
 - WL_Flash - the main class that implements wear levelling functionality.
 - WL_State -  contains state structure of the WLC.
 - WL_Config - contains structure to configure the WLC component at startup.
 - wear_levelling - wrapper API class that provides "C" interface to access the memory through the WLC


Flash_Access Interface
^^^^^^^^^^^^^^^^^^^^^^

In the component exist virtual interface Flash_Access. This interface implement main basic functions:
 - read - read memory to the buffer.
 - write - writes buffer to the memory.
 - erase - erase one sector.
 - erase_range - erase range of memory. The address of rage must be rounded to the sector size.
 - chip_size - returns the equivalent amount of memory.
 - sector_size - returns the sector size.
 - flush - stores current state to the flash, if needed.

The WLC implements this interface for the user, and requires this interface to access the memory.

Structure wl_config_t to Configure the WLC at startup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The wl_config_t contains configuration parameters for the WLC component.
 - start_addr - offset in the flash memory. The WLC will place all data after this address.
 - full_mem_size - amount of memory that was allocated and can be used by WLC
 - sector_size - flash memory sector size
 - page_size - size of memory for relocation at once. Must be N*sector_size, where N > 0.
 - updaterate - amount of erase cycles to execute the relocation procedure.
 - wr_size - smalest possible write access size without erasing of sector.
 - version - version of the WLC component.
 - temp_buff_size - amount of memory that the WLC will allocate internally. Must be > 0.
 
Internal Memory Organization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The WLC divide the memory that are define by start_addr and full_mem_size to three regions:
 - Configuration
 - Data
 - States
 
The Configuration region used to store configuration information. The user can use it to recover the WLC from memory dump.
The Data - is a region where user data stored. 
The States - is a region where the WLC stores internal information about the WLC state. The States region contains two copies  of the WLC states. It is implemented to prevent situation when the device is shut down 
during operation when the device stores the states. If one of copies is wrong, the WLC can recover the state from another. The broken copy will be overwritten by another.

Main Idea
^^^^^^^^^
The WLC has two access addresses: virtual address and real address. The virtual address used by user to access the WLC, the real address used by the WLC to access the real memory.
The WLC makes the conversion between virtual and real addresses.
The Data region divided to N pages (page could be equal to the sector size). One page defined as dummy page. For user will be available only N-1 pages. 
The WLC has two internal counters to calculate virtual and real addresses: erase counter and move counter. 
Every erase operation will be counted by erase counter. When this counter reached the *updaterate* number the page after Dummy page will be moved to the Dummy page, and Dummy page will be changed to this one. The erase counter will 
be cleared and move counter will be incremented. This procedure will be repeated again and again.
When the Dummy page will be at last page in the memory and erase counter will reach the updaterate, the move counter will be cleared and the state will be stored to the State memory.
Bellow shown the example with 4 available memory pages. Every state after updaterate erases. The X is a Dummy page.

- X 0 1 2 - start position
- 0 X 1 2 - first move, the page 0 and Dummy page change the places
- 0 1 X 2 - second move, the page 1 and Dummy page change the places
- 0 1 2 X - 
- X 1 2 0 - state stored to the memory
- 1 X 2 0 - 
- 1 2 X 0 - 
- 1 2 0 X - 
- X 2 0 1 - state stored to the memory
- 2 X 0 1 - 
- 2 0 X 1 - 
- 2 0 1 X - 
- X 0 1 2 - state stored to the memory, the memory made full cycle.

As we see, if user will write data only to one address, amount of erase cycles will be shared between the full memory. The price for that is a one memory page that will not be used by user.


