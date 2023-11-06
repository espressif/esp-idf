| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Device Provisioning Protocol (Enrollee) Example

This example shows how to configure ESP devices as an enrollee using Device Provisioning Protocol(DPP) also known as Wi-Fi Easy Connect.

DPP provides a simple and secure way to onboard ESP devices to a network.
We now support DPP in Responder-Enrollee mode with AKM types PSK and DPP.

You need a Wi-Fi Easy Connect with Initiator mode capable device to make use of this example. Some Android 10+ devices have this capability. (Vendor specific)

To run the example with an Android 10+ device follow below steps -
1. Compile and flash the example on ESP device, a QR code will appear on your console.
2. Connect your phone to the network, say named "Example-AP".
3. Now go to Settings->WiFi & Internet->Wi-Fi->Example-AP->Advanced->Add Device.
4. Scan QR Code using the scanner, which will make ESP device connect to Example-AP.

Optional configuration available

*Note:*
- QR Code should be displayed as dark on a white/light background to work properly.
- If displayed QR Code had line gaps, try switching to a new font or a different Terminal program. See below QR Code for checking beforehand.

### Example output

Here is an example of the console output.
```
I (807) wifi:mode : sta (24:0a:c4:23:da:20)
I (807) wifi dpp-enrollee: Started listening on Channel 11 for DPP Authentication
I (1157) wifi dpp-enrollee: Scan below QR Code to configure the enrollee:


  █▀▀▀▀▀█ ██▄▄▄█▄▀██▄▄█▄   ▀ ▀▄ █▄▄ █▀▀▀▀▀█
  █ ███ █ ██▀█▀ ▀▀██▀█▄█▀▄▀ ██▀▀█ ▄ █ ███ █
  █ ▀▀▀ █ ▄█▀▄▄ ▄▄▀ █▄▀ ▄ ▄ ▄▀▄ ██  █ ▀▀▀ █
  ▀▀▀▀▀▀▀ ▀ █▄▀ ▀ ▀▄▀▄▀▄▀ █ ▀ ▀▄█ ▀ ▀▀▀▀▀▀▀
  █▀ ▄██▀ ▄█ ▀█ ▄▀▄▄▄ ▀▀█▄ ▄▀█▄█▀▀▄▄▄▀▄██▀█
  █▄▀ ▄ ▀▄█▄ ▀▀█▀▀█ ▀▄ ▄█▀▀▀▀█▀▄▄▄ ██▄   ▄█
  ▀█▀█▀ ▀▀ ▀  ▄▀▄▀▀ ▄ ▄▀▀▀ █▄ ▄▄  ▀█▄▀▄  █
  ▀ ▀  ▀▀▀█▄ █▀▀ █▄▄▄ █▄ █▄▀ ██▄ ▄▄▀█▄▀  ▄█
  ▀██▀▄█▀▄ ▄█ ▀▄▀ █ ▄  ▄█▄▀▄▀▄▄▀▄ ▄▄▄▀▄▄
    ▀▀▄█▀█▄▀▀█▄ ▄▀ █▄ ▀█▄█▄▀ ▀█▄▄ ▄▀▄ █▄▀ █
  ▄▀▀ ▀█▀▀▀ ▄ ▀█▀▀▄ ▀ ▄▄█▄ █ ██▀▄▀▀▄▄▄▄█▀▄
   ▀ ███▀▀▄ ▄ ▄   ▀█▄▄▀█▀▀▀  ▀▀▄▄  ▀  █▄ ▄█
  █ ▀▄▄ ▀▀▀▀▄▀▀▀▄█▄▄ ▄▀▄▀ ▀▄▀▄▀█▀▀▄▀  ▄█▄▀
   ███ ▄▀▄▀▀▄▀▀█▀▀▄ ▀▄ ████ █▀▄█▄▄ ▀█▄ ▀▀ ▀
  ▄▀█▀▀▀▀█▀   ▄█▄▀▀ ▄ ▀█▀▀ ▀ ▄▀▀ ▀▄█  ▄  ▀
    █ ▀▀▀▄██▄█▀ ▀█▄█▄ ▀██▀▄▀▄▀ █▀ ▀ ▄▄▀█ ▄█
  ▀▀▀ ▀▀▀▀▄▄█▄▀█▄ ▄  ▄  ▀▀▀█▄▄▀▀▀ █▀▀▀██▀▀▄
  █▀▀▀▀▀█ ▄▄▀█▀ ▄█▄█▄▄█▄ ▀ ▀▀▀█▄ ▀█ ▀ █ ▀ █
  █ ███ █ ▀█▀ ▀█▀▀▄▄▀ ▀▄█▀▀   ██▀█▀▀▀█▀▄▄▄█
  █ ▀▀▀ █ ▄▀█ ▄ ▄ ▀█▄ ▀▄▀█ ▀▄██▄  ▀ ▄█ ▄▀▄█
  ▀▀▀▀▀▀▀ ▀  ▀ ▀ ▀    ▀▀▀   ▀▀▀▀▀ ▀ ▀    ▀

I (6357) wifi dpp-enrollee: DPP Authentication successful, connecting to AP : DigitalFortress
I (6477) wifi:new:<1,0>, old:<1,0>, ap:<255,255>, sta:<1,0>, prof:1
I (7277) wifi:state: init -> auth (b0)
I (7277) wifi:state: auth -> assoc (0)
I (7287) wifi:state: assoc -> run (10)
I (7317) wifi:connected with DigitalFortress, aid = 4, channel 1, BW20, bssid = 04:d4:c4:5e:22:f0
I (7317) wifi:security type: 3, phy: bgn, rssi: -60
I (7427) wifi:pm start, type: 1

I (7427) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (11617) esp_netif_handlers: sta ip: 192.168.1.216, mask: 255.255.255.0, gw: 192.168.1.1
I (11617) wifi dpp-enrollee: got ip:192.168.1.216
I (11617) wifi dpp-enrollee: connected to ap SSID:DigitalFortress password:password
```
