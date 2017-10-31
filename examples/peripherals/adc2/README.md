# Example: ADC2

This test code shows how to configure ADC2 and read the voltage connected to GPIO pin.

 
### ADC2 functions:

ADC1_CHANNEL_7: GPIO27, voltage range [0V..3.1V], the data range [0..4095],
saturates at about 1.1V.

DAC_CHANNEL_1: GPIO25, output range [0V..3.3V]

 
### Test:

Please choose the channel (GPIO) to output DAC signals and the channel to input 
signals to ADC by menuconfig:
    
    make menuconfig

Then connect two pins by a wire directly. 

E.g. Choose GPIO27 for ADC and GPIO 25 for DAC in menuconfig (default option).
Then connect them up.

Connection of ADC and DAC to the same pin is also allowed. In this case, you don't
have to connect by a wire externally. E.g. choose GPIO 26 for both ADC and DAC in
menuconfig.

