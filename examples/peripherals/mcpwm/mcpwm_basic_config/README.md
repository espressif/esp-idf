# MCPWM basic config Example

This example will show you how to use each submodule of MCPWM unit
 
The example can't be used without modifying the code first
 
Edit the macros at the top of mcpwm_example_basic_config.c to enable/disable the submodules which are used in the example


## Step 1: Pin assignment
* The gpio init function initializes:
    * six MCPWM output pins
    * three MCPWM fault input pins
    * three MCPWM sync input pins
    * three MCPWM capture input pins 

	
## Step 2: Connection
* Six MCPWM output pins to motor driver input signals
* Fault, sync, capture signals can be connected to respective signals


## Step 3: Initialize MCPWM 
* You need to set the frequency and duty cycle of each three MCPWM timer along with other parameters mentioned
* You need to set the MCPWM channel you want to use, with these timers


## Step 4: Testing
* The deadtime module, set deadtime type and with value as time*100ns
* The sync module, synchonizes all the timer pulses
* The fault module when enabled takes action on MCPWM signals when fault occurs
* The capture module captures input signal(digital i.e. hall sensor value, etc), timing between two rising/falling edge
