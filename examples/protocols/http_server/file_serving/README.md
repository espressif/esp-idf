| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Simple HTTP File Server Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

HTTP file server example demonstrates file serving with both upload and download capability, using the `esp_http_server` component of ESP-IDF. This example can use one of the following options for data storage:

- SPIFFS filesystem in SPI Flash. This option works on any ESP development board without any extra hardware.

- FAT filesystem on an SD card. Both SDSPI and SDMMC drivers are supported. You need a development board with an SD card slot to use this option.

The following URIs are provided by the server:

| URI                  | Method  | Description                                                                               |
|----------------------|---------|-------------------------------------------------------------------------------------------|
|`index.html`          | GET     | Redirects to `/`                                                                          |
|`favicon.ico`         | GET     | Browsers use this path to retrieve page icon which is embedded in flash                   |
|`/`                   | GET     | Responds with webpage displaying list of files on the filesystem and form for uploading new files |
|`/<file path>`        | GET     | For downloading files stored on the filesystem                                                    |
|`/upload/<file path>` | POST    | For uploading files on to the filesystem. Files are sent as body of HTTP post requests            |
|`/delete/<file path>` | POST    | Command for deleting a file from the filesystem                                                   |

File server implementation can be found under `main/file_server.c`. `main/upload_script.html` has some HTML, JavaScript and Ajax content used for file uploading, which is embedded in the flash image and used as it is when generating the home page of the file server.

Note that the default `/index.html` and `/favicon.ico` files can be overridden by uploading files with same name to the filesystem.

## How to use the example

### Wi-Fi/Ethernet connection
```
idf.py menuconfig
```
Open the project configuration menu (`idf.py menuconfig`) to configure Wi-Fi or Ethernet. See "Establishing Wi-Fi or Ethernet Connection" section in [examples/protocols/README.md](../../README.md) for more details.

### SD card (optional)

By default the example uses SPIFFS filesystem in SPI flash for file storage.

To use an SD card for file storage instead, open the project configuration menu (`idf.py menuconfig`) and enter "File_serving example menu". Then enable "Use SD card for file storage" (`CONFIG_EXAMPLE_MOUNT_SD_CARD`) option.

SD cards can be used either over SPI interface (on all ESP chips) or over SDMMC interface (on ESP32 and ESP32-S3). To use SDMMC interface, enable "Use SDMMC host" (`CONFIG_EXAMPLE_USE_SDMMC_HOST`) option. To use SPI interface, disable this option.

GPIO pins used to connect the SD card can be configured for the SPI interface (on all chips), or for SDMMC interface on chips where it uses GPIO matrix (ESP32-S3). This can be done in "SD card pin configuration" submenu.

The example will be able to mount only cards formatted using FAT32 filesystem. If the card is formatted as exFAT or some other filesystem, you have an option to format it in the example code — "Format the card if mount failed" (`CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED`).

For more information on pin configuration for SDMMC and SDSPI, check related examples: [sdmmc](../../../storage/sd_card/sdmmc/README.md), [sdspi](../../../storage/sd_card/sdmmc/README.md).

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Working with the example

1. Note down the IP assigned to your ESP module. The IP address is logged by the example as follows:

   ```
   I (5424) example_connect: - IPv4 address: 192.168.1.100
   I (5424) example_connect: - IPv6 address:    fe80:0000:0000:0000:86f7:03ff:fec0:1620, type: ESP_IP6_ADDR_IS
   ```

   The following steps assume that IP address 192.168.1.100 was assigned.

2. Test the example interactively in a web browser. The default port is 80.

    1. Open path http://192.168.1.100/ or http://192.168.1.100/index.html to see an HTML page with list of files on the server. The page will initially be empty.
    2. Use the file upload form on the webpage to select and upload a file to the server.
    3. Click a file link to download / open the file on browser (if supported).
    4. Click the delete link visible next to each file entry to delete them.

3. Test the example using curl:

    1. `myfile.html` can be uploaded to `/path/on/device/myfile_copy.html` using:
       ```
       curl -X POST --data-binary @myfile.html 192.168.43.130:80/upload/path/on/device/myfile_copy.html
       ```

    2. Download the uploaded file back:
       ```
       curl 192.168.43.130:80/path/on/device/myfile_copy.html > myfile_copy.html`
       ```

    3. Compare the copy with the original using `cmp myfile.html myfile_copy.html`


## Note

Browsers often send large header fields when an HTML form is submit. Therefore, for the purpose of this example, `HTTPD_MAX_REQ_HDR_LEN` has been increased to 1024 in `sdkconfig.defaults`. User can adjust this value as per their requirement, keeping in mind the memory constraint of the hardware in use.

## Example Output

```
I (5583) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:266f:28ff:fe80:2c74, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5583) example_connect: Connected to example_connect: sta
I (5593) example_connect: - IPv4 address: 192.168.194.219
I (5593) example_connect: - IPv6 address: fe80:0000:0000:0000:266f:28ff:fe80:2c74, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5603) example: Initializing SPIFFS
I (5723) example: Partition size: total: 896321, used: 0
I (5723) file_server: Starting HTTP Server on port: '80'
I (28933) file_server: Receiving file : /test.html...
I (28933) file_server: Remaining size : 574
I (28943) file_server: File reception complete
I (28993) file_server: Found file : test.html (574 bytes)
I (35943) file_server: Sending file : /test.html (574 bytes)...
I (35953) file_server: File sending complete
I (45363) file_server: Deleting file : /test.html
```
