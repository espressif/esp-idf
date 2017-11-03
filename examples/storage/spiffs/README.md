# SPIFFS example

This example demonstrates how to use SPIFFS with ESP32. Example does the following steps:

1. Use an "all-in-one" `esp_vfs_spiffs_register` function to:
    - initialize SPIFFS,
    - mount SPIFFS filesystem using SPIFFS library (and format, if the filesystem can not be mounted),
    - register SPIFFS filesystem in VFS, enabling C standard library and POSIX functions to be used.
2. Create a file using `fopen` and write to it using `fprintf`.
3. Rename the file. Before renaming, check if destination file already exists using `stat` function, and remove it using `unlink` function.
4. Open renamed file for reading, read back the line, and print it to the terminal.

## Example output

Here is an example console output. In this case `format_if_mount_failed` parameter was set to `true` in the source code. SPIFFS was unformatted, so the initial mount has failed. SPIFFS was then formatted, and mounted again.

```
I (195) example: Initializing SPIFFS
E (195) SPIFFS: mount failed, -10025. formatting...
I (4525) example: Opening file
I (4635) example: File written
I (4685) example: Renaming file
I (4735) example: Reading file
I (4735) example: Read from file: 'Hello World!'
I (4735) example: SPIFFS unmounted
```

