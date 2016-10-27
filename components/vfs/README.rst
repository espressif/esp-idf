Virtual filesystem component
============================

Overview
--------

Virtual filesystem (VFS) component provides a unified interface for drivers which can perform operations on file-like objects. This can be a real filesystems (FAT, SPIFFS, etc.), or device drivers which exposes file-like interface.

This component allows C library functions, such as fopen and fprintf, to work with FS drivers. At high level, each FS driver is associated with some path prefix. When one of C library functions needs to open a file, VFS component searches for the FS driver associated with the file's path, and forwards the call to that driver. VFS also forwards read, write, and other calls for the given file to the same FS driver.

For example, one can register a FAT filesystem driver with ``/fat`` prefix, and call ``fopen("/fat/file.txt", "w")``. VFS component will then call ``open`` function of FAT driver and pass ``/file.txt`` argument to it (and appropriate mode flags). All subsequent calls to C library functions for the returned ``FILE*`` stream will also be forwarded to the FAT driver.

FS registration
---------------

To register an FS driver, application needs to define in instance of esp_vfs_t structure and populate it with function pointers to FS APIs::

    esp_vfs_t myfs = {
        .fd_offset = 0,
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &myfs_write,
        .open = &myfs_open,
        .fstat = &myfs_fstat,
        .close = &myfs_close,
        .read = &myfs_read,
        .lseek = NULL,
        .stat = NULL,
        .link = NULL,
        .unlink = NULL,
        .rename = NULL
    };

    ESP_ERROR_CHECK(esp_vfs_register("/data", &myfs, NULL));

Depending on the way FS driver declares its APIs, either ``read``, ``write``, etc., or ``read_p``, ``write_p``, etc. should be used.

Case 1: API functions are declared without an extra context pointer (FS driver is a singleton)::

    size_t myfs_write(int fd, const void * data, size_t size);

    // In definition of esp_vfs_t:
        .flags = ESP_VFS_FLAG_DEFAULT,
        .write = &myfs_write,
    // ... other members initialized
    
    // When registering FS, context pointer (third argument) is NULL:
    ESP_ERROR_CHECK(esp_vfs_register("/data", &myfs, NULL));

Case 2: API functions are declared with an extra context pointer (FS driver supports multiple instances)::

    size_t myfs_write(myfs_t* fs, int fd, const void * data, size_t size);

    // In definition of esp_vfs_t:
        .flags = ESP_VFS_FLAG_CONTEXT_PTR,
        .write_p = &myfs_write,
    // ... other members initialized
    
    // When registering FS, pass the FS context pointer into the third argument
    // (hypothetical myfs_mount function is used for illustrative purposes)
    myfs_t* myfs_inst1 = myfs_mount(partition1->offset, partition1->size);
    ESP_ERROR_CHECK(esp_vfs_register("/data1", &myfs, myfs_inst1));

    // Can register another instance:
    myfs_t* myfs_inst2 = myfs_mount(partition2->offset, partition2->size);
    ESP_ERROR_CHECK(esp_vfs_register("/data2", &myfs, myfs_inst2));

Paths
-----

Each registered FS has a path prefix associated with it. This prefix may be considered a "mount point" of this partition.

Registering mount points which have another mount point as a prefix is not supported and results in undefined behavior. For instance, the following is correct and supported:

- FS 1 on /data/fs1
- FS 2 on /data/fs2

This **will not work** as expected:

- FS 1 on /data
- FS 2 on /data/fs2

When opening files, FS driver will only be given relative path to files. For example:

- ``myfs`` driver is registered with ``/data`` as path prefix
- and application calls ``fopen("/data/config.json", ...)``
- then VFS component will call ``myfs_open("/config.json", ...)``.
- ``myfs`` driver will open ``/config.json`` file

VFS doesn't impose a limit on total file path length, but it does limit FS path prefix to ``ESP_VFS_PATH_MAX`` characters. Individual FS drivers may have their own filename length limitations.


File descriptors
----------------

It is suggested that filesystem drivers should use small positive integers as file descriptors. VFS component assumes that ``CONFIG_MAX_FD_BITS`` bits (12 by default) are sufficient to represent a file descriptor.

If filesystem is configured with an option to offset all file descriptors by a constant value, such value should be passed to ``fd_offset`` field of ``esp_vfs_t`` structure. VFS component will then remove this offset when working with FDs of that specific FS, bringing them into the range of small positive integers.

While file descriptors returned by VFS component to newlib library are rarely seen by the application, the following details may be useful for debugging purposes. File descriptors returned by VFS component are composed of two parts: FS driver ID, and the actual file descriptor. Because newlib stores file descriptors as 16-bit integers, VFS component is also limited by 16 bits to store both parts. 

Lower ``CONFIG_MAX_FD_BITS`` bits are used to store zero-based file descriptor. If FS driver has a non-zero ``fd_offset`` field, this ``fd_offset`` is subtracted FDs obtained from the FS ``open`` call, and the result is stored in the lower bits of the FD. Higher bits are used to save the index of FS in the internal table of registered filesystems.

When VFS component receives a call from newlib which has a file descriptor, this file descriptor is translated back to the FS-specific file descriptor. First, higher bits of FD are used to identify the FS. Then ``fd_offset`` field of the FS is added to the lower ``CONFIG_MAX_FD_BITS`` bits of the fd, and resulting FD is passed to the FS driver.

::

       FD as seen by newlib                                    FD as seen by FS driver
                                                  +-----+
    +-------+---------------+                     |     |    +------------------------+
    | FS id | Zero—based FD |     +---------------> sum +---->                        |
    +---+---+------+--------+     |               |     |    +------------------------+
        |          |              |               +--^--+
        |          +--------------+                  |
        |                                            |
        |       +-------------+                      |
        |       | Table of    |                      |
        |       | registered  |                      |
        |       | filesystems |                      |
        |       +-------------+    +-------------+   |
        +------->  entry      +----> esp_vfs_t   |   |
        index   +-------------+    | structure   |   |
                |             |    |             |   |
                |             |    | + fd_offset +---+
                +-------------+    |             |
                                   +-------------+



