# MCPWM brushed dc motor control Example

This example will show you how to use MCPWM module to control brushed dc motor, you need to make connection between ESP32 and motor driver
 
This code is tested with L298 motor driver, user needs to make changes according to the driver they use
 
Motor first moves forward, then backward and then stops for 2 Secs each countinuously


## Step 1: Pin assignment
* GPIO15 is assigned as the enable/input 1 for motor driver    
* GPIO16 is assigned as the enable/input 2 for motor driver


## Step 2: Connection
* connect GPIO15 with input 1 of motor driver
* connect GPIO16 with input 2 of motor driver


## Step 3: Initialize MCPWM
* You need to set the frequency and duty cycle of MCPWM timer
* You need to set the MCPWM channel you want to use, and bind the channel with one of the timers
