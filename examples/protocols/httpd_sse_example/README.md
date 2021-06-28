# Server-Sent Event (SSE) Example

This example shows an HTTP server implementing the simplest configuration to demonstrate
	the use of the SSE technology. The overall configuration is :
	
* A physical ESP32 board with a switch and a LED.
	The board is configured as a WiFi Access Point.
	
* Some browser to view the client side.
	
Either clicking on the computer's led icon or on the board's switch will toggle
	the board's led status and the icon. The browser has opened a permanent socket
	where the server can send messages when the board's led toggles. In the other
	direction the browser sends a PUT request when the icon is clicked on.

## How to use this example

### Hardware Required

A physical ESP32 board with a switch on ``GPIO 0`` and a LED on ``GPIO 5`` ; should be quite common.
	Tested with SparkFun's ESP32 Thing.
	
A computer with a decently recent browser (I read about restrictions with older I.E.).
	Tested with Firefox 75.0 + 57.0.1. It also works with many computers at a time. 

### Build and Flash

Build the project and flash it to the board, eventually run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

Connect the computer's Wifi to your board (SSID ``ESP32SSID``, password ``secret123``).

Open a browser and specify the URI ``192.168.4.1/`` or ``192.168.4.1/index.html``.

One can open many browser on many computers as long as the server has enougth available
	sockets. The limit should be 3 instances with the setted parameters.
	
See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Under the hood
* The ESP32's wifi is started in Acess Point mode.

* A task scans the physical switch and queues an event when the switch is depressed and
	then released.
	
* An other task checks the queue and toggles the physical led when the corresponding event
	was found. It also informs the http server.
	
* The http server is configured to serve a GET at ``/`` and responds with some javascript and
	the html to show the (minimal) page. 
	
* The browser's javascript instantiates an ``EventSource`` that calls ``/demo_sse`` (GET). 

* The http server queues an event asking for an hardware status to be
	sent (initial state) and registers the connection as opened.

* Either because of a refresh event or a toggle one, the hardware manager will send the
	http server the actual or new status.

* The status will be sent over all the registered connections.

* The browser's ``EventSource`` object will parse the received JSON data and update the icon accordingly.

* When ever the browser's icon is clicked it will send a PUT message with the name of the clicked icon.

* The http server parse the data and queues an event, the same as when the switch was activated.

* At the time a browser closes its page the http server will unregister the connection. 

* [Here](https://www.w3schools.com/html/html5_serversentevents.asp) are details of the principle.
