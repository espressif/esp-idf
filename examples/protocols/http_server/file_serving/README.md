| Supported Targets | ESP32 | ESP32-S2 | ESP32-C3 |
| ----------------- | ----- | -------- | -------- |

# Simple HTTP File Server Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

If the SD card option is enabled, you can access files in the SD card by the path `/sdcard` 

HTTP file server example demonstrates file serving with both upload and download capability, using the `esp_http_server` component of ESP-IDF. The following URIs are provided by the server:

| URI                  | Method  | Description                                                                               |
|----------------------|---------|-------------------------------------------------------------------------------------------|
|`index.html`          | GET     | Redirects to `/`                                                                          |
|`favicon.ico`         | GET     | Browsers use this path to retrieve page icon which is embedded in flash                   |
|`/`                   | GET     | Responds with webpage displaying list of files on SPIFFS and form for uploading new files |
|`/<file path>`        | GET     | For downloading files stored on SPIFFS                                                    |
|`/upload/<file path>` | POST    | For uploading files on to SPIFFS. Files are sent as body of HTTP post requests            |
|`/delete/<file path>` | POST    | Command for deleting a file from SPIFFS                                                   |

File server implementation can be found under `main/file_server.c` which uses SPIFFS for file storage. `main/upload_script.html` has some HTML, JavaScript and Ajax content used for file uploading, which is embedded in the flash image and used as it is when generating the home page of the file server.

## Note

`/index.html` and `/favicon.ico` can be overridden by uploading files with same pathname to SPIFFS.

## Usage

* Open the project configuration menu (`idf.py menuconfig`) go to `Example Configuration` ->
    1. WIFI SSID: WIFI network to which your PC is also connected to.
    2. WIFI Password: WIFI password

* In order to test the file server demo :
    1. compile and burn the firmware `idf.py -p PORT flash`
    2. run `idf.py -p PORT monitor` and note down the IP assigned to your ESP module. The default port is 80
    3. test the example interactively on a web browser (assuming IP is 192.168.43.130):
        1. open path `http://192.168.43.130/` or `http://192.168.43.130/index.html` to see an HTML web page with list of files on the server (initially empty)
        2. use the file upload form on the webpage to select and upload a file to the server
        3. click a file link to download / open the file on browser (if supported)
        4. click the delete link visible next to each file entry to delete them
    4. test the example using curl (assuming IP is 192.168.43.130):
        1. `myfile.html` is uploaded to `/path/on/device/myfile_copy.html` using `curl -X POST --data-binary @myfile.html 192.168.43.130:80/upload/path/on/device/myfile_copy.html`
        2. download the uploaded copy back : `curl 192.168.43.130:80/path/on/device/myfile_copy.html > myfile_copy.html`
        3. compare the copy with the original using `cmp myfile.html myfile_copy.html`

* To write to SD card, you need to:
    1. Select the `Mount the SD card to the filesystem` in the configuration menu (by calling `idf.py menuconfig` and select the `EXAMPLE_MOUNT_SD_CARD` option.
    2. If you need to format the card while the card fails to be mounted, enable the config option `The card will be formatted if mount has failed` (`EXAMPLE_FORMAT_SDCARD_IF_MOUNT_FAILED`). Be careful, all the data in the card will disappear.

    Note: You will have to access the SD card by SPI bus with sdspi driver, if you are using ESP32S2.

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
