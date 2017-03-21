lws-esp32-factory
=================

This is a standalone <1MB image intended for the 1MB "factory" slot on ESP32.

It has the following capabilities:

### Initial Factory Setup Page

Allows overriding default serial, setting device options string, and uploading SSL certs

![Factory Setup](https://libwebsockets.org/setup3.png)

With an empty nvs, the first time this will come up at https://192.168.4.1 in AP mode.

After the DER format SSL certs have been uploaded, everything subsequently is in https.

### User configuration Setup Page

![lws AP mode config page](https://libwebsockets.org/setup2.png)

If no AP information is in the nvs, this page comes up automatically at https://192.168.4.1 in AP mode.

The user can reach it subsequently programmatically or by grounding a GPIO (ie, by a button), the default GPIO is IO14.

The page allows you to select an AP from a scan list and give a passphrase.

Once it connects, the DHCP information is shown, and it autonomously connects to a configurable server over https to check for updates.  The user can select to have it autonomously download the update and restart.

The user can also upload images by hand.

## Optional default peripherals

It's not required, but the default code expects

 - pushbutton to 0V connected on IO14

If the pushbutton is held down at boot, the user is forced into the factory / Setup mode rather than the OTA application.

 - LED  connected  3.3V ---|>|---- IO23

While in factory / OTA mode, the LED flashes on and off at 500ms.  When you press "ID Device" button in the UI, the LED flashes rapidly for 5s.


## Building and using

### Step 1: Clone and get lws submodule

```
     $ git clone git@github.com:warmcat/lws-esp32-factory.git
     $ git submodule update --init --recursive
```

### Step 2: Erase the whole device

Clear down the partitioning since we write a custom table and the bootloader
will choke if the OTA parts are not initialized like this one time

```
 $ make erase_flash
```

## Step 3: General build and flash

```
 $ make all flash monitor
```

## Using the Factory Config

 - From scratch, the device is in AP mode with a AP name like config-lws-SERIAL

 - From scratch, the factory app listens at http://192.168.4.1/factory.html

 - build generates a selfsigned DER acert and key in top level directory of this project each time, `ssl-cert.der` and `ssl-key.der`.  Use the form to upload these.

 - after the reboot vist https://192.168.4.1, which will take you to the user setup

## Using the User setup

 - connect your wifi to the ap "lws-config-...."

 - In a browser, go to https://192.168.4.1

 - Select your normal AP from the list

 - Give the AP password and click the button

 - Your ESP32 should associates with the AP without resetting

## Using the lws test apps

This application is just the factory / setup application.

The end-user applications are separate projects, see eg

https://github.com/warmcat/lws-esp32-test-server-demos

These are built and loaded slightly differently, ie

```
 $ make all flash_ota monitor
```

This is because they target the 2.9MB OTA flash area.

The `build/*.bin` file from the application build may also be uploaded in the setup page upload UI.
