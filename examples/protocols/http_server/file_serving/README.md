# Simple HTTP File Server Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

HTTP file server example demonstrates file serving using the `esp_http_server` component of ESP-IDF:
  1. URI `/path/filename` for GET command downloads the corresponding file (if exists)
  2. URI `/upload` POST command for uploading the file onto the device
  3. URI `/delete` POST command for deleting the file from the device

File server implementation can be found under `main/file_server.c` which uses SPIFFS for file storage. `main/upload_script.html` has some HTML, JavaScript and Ajax content used for file uploading, which is embedded in the flash image and used as it is when generating the home page of the file server.

## Usage

* Configure the project using `make menuconfig` and goto `Example Configuration` ->
    1. WIFI SSID: WIFI network to which your PC is also connected to.
    2. WIFI Password: WIFI password

* In order to test the file server demo :
    1. compile and burn the firmware `make flash`
    2. run `make monitor` and note down the IP assigned to your ESP module. The default port is 80
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
