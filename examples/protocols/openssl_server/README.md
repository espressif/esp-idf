# Openssl Example

The Example contains of OpenSSL server demo.

First you should configure the project by "make menuconfig":
  Example Configuration -> 
    1. WIFI SSID: WIFI network to which your PC is also connected to. 
    2. WIFI Password: WIFI password
    
IF you want to test the OpenSSL server demo: 
  1. compile the code and load the firmware 
  2. input the context of "https://192.168.17.128" into your web browser, the IP of your module may not be 192.168.17.128, you should input your module's IP
  3. You may see that it shows the website is not able to be trusted, but you should select that "go on to visit it"
  4. You should wait for a moment until your see the "OpenSSL server demo!" in your web browser
  
Note:
  The private key and certification at the example are not trusted by web browser, because they are not created by CA official, just by ourselves.
  You can alse create your own private key and ceritification by "openssl at ubuntu or others". 
  We have the document of "ESP8266_SDKSSL_User_Manual_EN_v1.4.pdf" at "https://www.espressif.com/en/support/download/documents". By it you can gernerate the private key and certification with the fomate of ".pem"

See the README.md file in the upper level 'examples' directory for more information about examples.
