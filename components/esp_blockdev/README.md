# ESP IDF Block Device Layer component

ESP-IDF framework contains drivers for several physical storage devices, such as internal/external SPI Flash memory modules or SD/eMMC cards. Additional drivers are implemented as managed components: USB MSC host, SPI NAND Flash, and similar. Each such a driver has its own unique API.
Furthermore, ESP-IDF supports several filesystems: FAT, SPIFFS, LittleFS, NVS (soft of pseudo file-system) and more are likely to be added in the future. These filesystems are available mostly as managed components, and each of them uses a porting layer to interact with the physical storage device.
Since each physical storage driver also has its own API, every filesystem component naturally needs a porting layer for every connected driver. This leads to a lot of duplicated code and makes it difficult to add new filesystems or new storage device drivers.

The above-mentioned is actually a general problem - for instance, the FATFS library attempts to solve the issue by providing a "diskio" layer which allows registering callbacks for various operations on the target physical device. However, there is no standard and flexible interface commonly supported by widely used filesystems, so the core issue remains unresolved.
To make things even more complex, there are also middleware libraries which often "sit" between the filesystem and the physical device driver: wear levelling, software encryption or journaling to name a few. This said, it is obvious such libraries also need their porting layers for each storage driver. Furthermore, they can't be "stacked" or combined since each one has a unique API.

ESP-IDF solution to the problem is `esp_blockdev` component, which introduces Block Device Layer (BDL) - C-language interface defined in `include/esp_blockdev.h` file. 
It covers all the generic operations like Read, Write or Erase, allows setting device specific geometry, defines device's important properties via flsgs, provides access to debug information, statistics and other commonly required features.
Implementing the interface allows ESP-IDF based components and applications to combine the filesystems, libraries and drivers in nearly any way they want.

'Block-device' in this interface context refers to the basic principle of data processing within any BDL operation - all the data are manipulated in chunks of device-specific size (defined per operation type in the device's geometry section), ie processed in blocks. 
Though the BDL declares all the APIs with byte-aligned parameters, the values actually passed are to be aligned to integral multiples of corresponding block size wherever eligible (addressing, buffer sizes, data lengths).
This approach should be thoroughly deployed by both callers and API function implementations to avoid undefined behaviour.

Block device from coding perspective is defined as an instance of `esp_blockdev_t` structure, typically associated with existing component (eg `esp_partition` provides BDL interface on demand, but it is actually independent on any BDL stack).
BDL instance is allocated by its owning component's BDL-create API named `<component>_get_blockdev`, which returns `esp_blockdev_handle_t` handle on success (and which is the only access link for given BDL instance).
Optionally, components can provide BDL-release API named `<component>_release_blockdev'.

Block-device stacks are then built by chaining single BDL instances through their handles.

The BDL interface follows the Open-Closed Principle (open for extension, closed for modification), so new features can be added only without altering the existing function.

## BDL internals

The BDL interface structure is described by the following pseudo-code:

```
struct esp_blockdev_t {

    //DEVICE FLAGS
    esp_blockdev_flags_t device_flags
    {
        read_only;               
        encrypted;               
        erase_before_write;      
        and_type_write;          
        default_val_after_erase; 
    }

    //DEVICE GEOMETRY
    esp_blockdev_geometry_t
    {
        disk_size;
        read_size;
        write_size;
        erase_size;
        recommended_write_size;
        recommended_read_size;
        recommended_erase_size;    
    }

    //DEVICE API
    esp_err_t (*read)(esp_blockdev_handle_t dev_handle, uint8_t* dst_buf, size_t dst_buf_size, uint64_t src_addr, size_t data_read_len);
    esp_err_t (*write)(esp_blockdev_handle_t dev_handle, const uint8_t* src_buf, uint64_t dst_addr, size_t data_write_len);
    esp_err_t (*erase)(esp_blockdev_handle_t dev_handle, uint64_t start_addr, size_t erase_len);
    esp_err_t (*sync)(esp_blockdev_handle_t dev_handle);
    esp_err_t (*ioctl)(esp_blockdev_handle_t dev_handle, const esp_blockdev_ioctl_cmd_t cmd, void* args);

} esp_blockdev_t;
```

### BDL device flags

Block Device Layer instance flags cover various properties and characteristics of given BDL device, ranging from general attributes to hardware specific behaviour. 
Every BDL object contains one `esp_blockdev_flags_t` instance initialised exclusively by the owning device during its creation, and its flags setup stays fixed for the whole object's lifetime.
This implies all the sanity checks are to happen during device's initialisation, and possible failures should result in the device not being instantiated.
  
The flags can be handled as device specific (ie defined or overwritten during the device init) or they can be ignored by the device code once not needed internally. 
Still, the flags should always be transported to components higher in each BDL stack. 
Ignoring this recommendation increases chances of silently implanted errors as BDL devices are expected to work in various contexts (stacks), given stack items order can be changed by later update, components above given device may need the flags after their code updates, and similar.

Most cases, the flags (typically hardware related ones) are just propagated up actual BDL stack - they are simply copied by each intermediate device from its lower neighbour.
`and_type_write` is a good example: required by the topmost components (usually file systems), and defined only by the bottom device hardware/driver. 
Thus, all intermediate BDL components in such a stack only relay the flag values and expose them to the nearest higher BDL device.

BDL device flags should always be handled carefully as they may critically influence desired function of the target BDL device package.

### BDL device geometry

Block device geometry defines all important block size parameters (in bytes), ie the smallest operation disk space or memory units for each data operation type.
`disk_size` defines a size of whole BDL device space and is not necessarily related to any physical disk - it can be a partition size, SD card capacity, memory block or any sort of "logical disk".
`recommended_<oper>_size` are optional fields provided for possible performance optimizations, in case given BDL device has specific properties which allow better efficiency than required defaults. 
For instance, NOR flash memories are supposed to provide random byte access for all reading operations, but specific flash chips require 4-byte alignment of the target address and retrieve data in 4-byte chunks anyway. 
Single byte access is still doable but at a cost of the performance drop due to internal buffering, flash driver overhead, and similar.
In such a case, the BDL stack device installed one-level higher can make use of the `recommended_read_size` parameter as long as it fits into the device's internal logic. If not, the default read size is still guaranteed to work.

Device geometry section is set by owning device during it's start up phase and cannot be changed during it's filetime. Other BDL device access the section in read-only mode, though it is technically possible to change the values.

### BDL API

Data manipulation and device handling API functions prototyped by function pointers. `read()`, `write()` and `erase()` process their corresponding workflow on given BDL device,
`sync()` forces flushing all work buffers and finishing all pending 'write' operations and `ioctl()` provides a general way to process both specific commands (like 'TRIM' or 'SANITIZE' for eMMC)
and auxiliary operations like getting device statistics or debugging information.

All the APIs are optional, so if any API function is not available for given device the corresponding pointer is set NULL.

### Error handling

BDL interface doesn't define specific requirements for return values and/or error codes of the API functions declared below. The only expectation is returning ESP_OK on successful run and any sort of ESP_ERR_* on failure (to stay compatible with ESP_ERROR_CHECK and other standard IDF error validation helpers).

Recommended scheme of error propagation is to let the errors "bubble-up" through given BDL stack from the error-source device to the topmost level, and let application decide about further processing.
