# MCPWM servo motor control Example

This example will show you how to use MCPWM module to control servo motor
 
Assign pulse width range and the maximum degree, accordingly the servo will move from 0 to maximum degree continuously
 

## Step 1: Pin assignment
* GPIO15 is assigned as the MCPWM signal for servo motor 


## Step 2: Connection
* connect GPIO15 with servo pwm signal
* other two wires of servo motor are VCC and GND


## Step 3: Initialize MCPWM
* You need to set the frequency(generally 50 Hz) and duty cycle of MCPWM timer
* You need to set the MCPWM channel you want to use, and bind the channel with one of the timers
