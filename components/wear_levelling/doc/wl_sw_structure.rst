Wear Levelling Component
========================

Wear Levelling Component (WLC) is a software component, implemented to prolong the service life of a storage media. This is done by preventing frequent erase operations at specific locations of the storage media memory space, which cause wear out of that particular memory while other locations are possibly not used at all. The WLC controls access attempts to all the media sectors available.
The WLC do not have internal cache. When write operation is finished, that means that data was really stored to the flash.
The component requires the storage media access driver as its parameter, in order to manipulate target sectors. As the WLC implements wear-levelling for the SPI Flash device, the driver is controlled through the Flash_Access interface.

The WLC Versioning and Compatibility
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The WLC accepts data formats from older version. Latest version of the WLC will update data format from older versions to the current one.
Current implementation of WLC has version 2. The data format from current version incompatible with data format from previous versions, and could not be 
used with previous versions.

The WLC Files
^^^^^^^^^^^^^^^
The WLC consists of several components that are implemented in various files. The list and brief description of these components follows:

 - Flash_Access - SPI Flash memory access interface. Implemented in classes WL_Flash, Partition and SPI_Flash.
 - SPI_Flash - class implements the Flash_Access interface to provide access to the flash memory.
 - Partition - class implements the Flash_Access interface to provide access to the partition.
 - WL_Flash - the main class that implements wear levelling functionality.
 - WL_State - contains state structure of the WLC.
 - WL_Config - contains structure to configure the WLC at startup.
 - wear_levelling - wrapper API class that provides "C" interface to access the memory through the WLC


Flash_Access Interface
^^^^^^^^^^^^^^^^^^^^^^

Virtual interface Flash_Access defines the following basic functions:
 - read - reads from the flash memory to a data buffer.
 - write - writes to the flash memory from a data buffer.
 - erase - erases a sector of the flash memory.
 - erase_range - erases a range of sectors of the flash memory. The range length must be rounded to a multiple of the sector size.
 - flash_size - returns the available flash memory (in bytes).
 - sector_size - returns the flash sector size.
 - flush - stores current state to the flash, if needed.

WLC startup configuration (wl_config_t structure)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The wl_config_t contains configuration parameters for the WLC component.
 - wl_partition_start_addr - wear levelled partition starting address (in the flash memory)
 - wl_partition_size - size of the wear-levelled partition
 - flash_sector_size - flash memory sector size
 - wl_page_size - one WLC page size (in bytes). wl_page_size >= (N * sector_size), where N > 0
 - wl_update_rate - erase operation count after which the physical sector and the WLC dummy sector swap their addresses
 - wl_pos_update_record_size - number of bytes for storing position-update record, appended to the WLC state sector data after each expiration of wl_update_rate.
 - version - version of the WLC component.
 - temp_buff_size - size of a temporary buffer to copy data from one flash memory area to another. This value should be equal to the flash sector size.
 
Internal Memory Organization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The WLC divides the memory given by wl_partition_start_addr and wl_partition_size to three regions:
 - Configuration
 - States
 - Data
 
The Configuration - a region used to store the WLC configuration information. User can use this information to restore the WL partition from a memory dump.
The States - a region to store internal WLC status information. The States region contains two copies of the WLC status record - this is to help safely recover last-known WLC status in case of power outage.
The Data - a region where the user data is stored. 

Main Idea
^^^^^^^^^
The WLC uses two types of flash memory-access addresses: virtual (logical) address and real (physical) address. The virtual address is used by users as a common target address, while the real address is used internally by the WLC to access the actual memory block.
The Data region is divided into N pages (page could be equal to the sector size, see above). One WLC page is defined as a "dummy" page, which implies the users can use only N-1 pages to store the data.
The WLC has two internal counters to calculate virtual and real addresses: erase counter (access_count) and move counter (move_count).
Every erase operation increments the erase counter. When the counter reaches the *wl_update_rate* number, the contents of the page next after the actual "dummy" page is copied to the dummy page and the next page itself becomes a new WLC dummy page (the last partition page swaps its position with the first one). This way, the WLC dummy page will traverses across whole partition, ensuring the wear burden is distributed evenly. After a completion of one cycle, the move counter is incremented too.
On the increment of the move counter, the WLC status record is stored to the State sector in the flash memory.
The example below shows 4 available memory pages and their corresponding states after each wl_update_rate change. The 'X' is the WLC dummy page.

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

As can be seen, erase cycles of the sectors get distributed across the entire flash at the cost of small memory part used by the WLC, which is thus not available to the users.

