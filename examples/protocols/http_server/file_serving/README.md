# Simple HTTP File Server Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

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

## Note

Browsers often send large header fields when an HTML form is submit. Therefore, for the purpose of this example, `HTTPD_MAX_REQ_HDR_LEN` has been increased to 1024 in `sdkconfig.defaults`. User can adjust this value as per their requirement, keeping in mind the memory constraint of the hardware in use.
