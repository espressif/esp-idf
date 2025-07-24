| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- |

# HTTP Restful API Server Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates on the implementation of both the RESTful API server and web server on ESP32, with a modern UI made by Vue.js and Vuetify frameworks. Please note, using the Vue is not a must, we're just using it as an example to show how to build a modern web UI with the latest web technologies in an IoT application.

This example exposes several APIs for the clients to fetch resources as follows:

| API                        | Method | Resource Example                                      | Description                                                                              |
| -------------------------- | ------ | ----------------------------------------------------- | ---------------------------------------------------------------------------------------- |
| `/api/v1/system/info`      | `GET`  | {<br />version:"v6.0-dev",<br />cores:2<br />}        | Return system information like IDF version, CPU cores, etc                               |
| `/api/v1/temp/raw`         | `GET`  | {<br />raw:22<br />}                                  | Return temperature data (note, this API returns a random number just for illustration)   |
| `/api/v1/light/brightness` | `POST` | {<br />red:160,<br />green:160,<br />blue:160<br />}  | Set the RGB value of the LED light                                                       |

### About mDNS

The IP address of an IoT device may vary from time to time, so it’s impracticable to hard code the IP address in the webpage. In this example, we use the `mDNS` to parse the domain name `dashboard.local`, so that we can always get access to the web server by this URL no matter what the real IP address behind it. See [here](https://docs.espressif.com/projects/esp-protocols/mdns/docs/latest/en/index.html) for more information about mDNS.

**Notes: mDNS is installed by default on most operating systems or is available as separate package.**

### About frontend framework

Many famous frontend frameworks (e.g. Vue, React, Svelte) can be used in this example. Here we just take [Vue](https://vuejs.org/) as example and adopt the [vuetify](https://vuetifyjs.com/) as the UI component library.

### About developing frontend and backend independently

In this example, the webpage files (html, js, css, images, etc) are stored in the filesystem on the ESP chip (we use the littlefs as an example). You can, however, develop the frontend without flashing the filesystem to the ESP every time:

1. First, disable the `EXAMPLE_DEPLOY_WEB_PAGES` from the menuconfig, implement the endpoints in the backend (the application running on the ESP) and flash it to the device.
2. Start developing the frontend on the PC, using Vite dev mode: `pnpm dev`. In dev mode, you can edit the source code of the frontend and see the changes in the web browser immediately. The frontend will be served from your PC, while the Vite proxy will automatically forward the HTTP requests to the `/api` endpoints to the ESP chip.
3. Once the frontend development and debugging is done, build the web pages by running `pnpm build`, which will generate the static files in the `dist` directory.
4. Finally, enable the `EXAMPLE_DEPLOY_WEB_PAGES` option in the menuconfig, and flash the webpages with the backend firmware together to the ESP chip again.

This way, you can develop the frontend and backend independently, which is very convenient for web developers.

## How to use the example

### Hardware Required

To run this example, you need an ESP32 dev board (e.g. ESP32-WROVER Kit, ESP32-Ethernet-Kit) with Wi-Fi or Ethernet connection.

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Set the domain name in `mDNS Host Name` option.
* Set the mount point of the website in `Website mount point in VFS` option, the default value is `/www`.
* Enable the `Deploy web pages to device's filesystem` option to deploy the web pages to the device's filesystem. This will flash the files from `front/web-demo/dist` to the device's filesystem.

### Build the web project

After the webpage design work has been finished, you should compile them by running following commands:

```bash
cd path_to_this_example/front/web-demo
pnpm install
pnpm build
```

After a while, you will see a `dist` directory which contains all the website files (e.g. html, js, css, images).

Refer to [front/web-demo/README.md](front/web-demo/README.md) for more information about the front-end development.

### Build and Flash to ESP32 device

Then, you can Run `idf.py -p PORT flash monitor` to build and flash the project (including the webpage bundle) to ESP32;

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Check the webpage in browser

In your browser, enter the URL where the website located (e.g. `http://dashboard.local`). You can also enter the IP address that ESP32 obtained if your operating system currently don't have support for mDNS service.

Besides that, this example also enables the NetBIOS feature with the domain name `dashboard`. If your OS supports NetBIOS and has enabled it (e.g. Windows has native support for NetBIOS), then the URL `http://dashboard` should also work.

### ESP monitor output

In the *Light* page, after we set up the light color and click on the check button, the browser will send a post request to ESP32, and the RGB value will be printed in the ESP32's console.

```bash
I (422) main_task: Calling app_main()
I (422) mdns_mem: mDNS task will be created from internal RAM
I (422) example_connect: Start example_connect.
I (612) example_connect: Connecting to TP-LINK_CB59...
I (622) example_connect: Waiting for IP(s)
I (4792) esp_netif_handlers: example_netif_sta ip: 192.168.0.112, mask: 255.255.255.0, gw: 192.168.0.1
I (4792) example_connect: Got IPv4 event: Interface "example_netif_sta" address: 192.168.0.112
I (4792) example_common: Connected to example_netif_sta
I (4802) example_common: - IPv4 address: 192.168.0.112,
I (4832) example: Partition size: total: 2097152, used: 770048
I (4832) esp-rest: Starting HTTP Server
I (4832) main_task: Returned from app_main()
I (49052) esp-rest: File sending complete
I (67352) esp-rest: Light control: red = 160, green = 160, blue = 48
```

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
