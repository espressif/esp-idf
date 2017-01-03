# I2C Example

 
* This example will show you how to use I2C module by running two tasks on i2c bus:
 
    * read external i2c sensor, here we use a BH1750 light sensor(GY-30 module) for instance.
    * Use one I2C port(master mode) to read or write the other I2C port(slave mode) on one ESP32 chip.
 
* Pin assignment:
 
    * slave :
        * GPIO25 is assigned as the data signal of i2c slave port
        * GPIO26 is assigned as the clock signal of i2c slave port
    * master:
        * GPIO18 is assigned as the data signal of i2c master port
        * GPIO19 is assigned as the clock signal of i2c master port
 
* Connection:
 
    * connect GPIO18 with GPIO25
    * connect GPIO19 with GPIO26
    * connect sda/scl of sensor with GPIO18/GPIO19
    * no need to add external pull-up resistors, driver will enable internal pull-up resistors.
 
* Test items:
 
    * read the sensor data, if connected.
    * i2c master(ESP32) will write data to i2c slave(ESP32).
    * i2c master(ESP32) will read data from i2c slave(ESP32).
