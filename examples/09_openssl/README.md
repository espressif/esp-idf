# Openssl Example

The Example contains of OpenSSL client and server demo.

First you should config the project by "menuconfig":
  Example Configuration -> 
    1. Openssl demo: select your demo (client or server)
    2. WiFi SSID: you own wifi, which you pc is connected to alse. 
    3. WiFi Password: wifi password

If you want to test the OpenSSL client demo:
  1. compile the code and load the firmware
  2. you can see it will download the "www.baidu.com" main page and print the context
    
IF you want to test the openSSL client demo: 
  1. compile the code and load the firmware 
  2. You should input the context of "https://192.168.17.128", the IP of your module may not be 192.168.17.128, you should input your module's IP
  3. You may see that it shows the website is not able to be trusted, but you should select that "go on to visit it"
  4. You should wait for a moment until your see the "OpenSSL server demo!" in your IE page

See the README.md file in the upper level 'examples' directory for more information about examples.
