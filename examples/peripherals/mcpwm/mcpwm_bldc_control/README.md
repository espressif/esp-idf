# MCPWM BLDC motor control(hall sensor feedback) Example

This example will show you how to use MCPWM module to control bldc motor with hall sensor feedback
 
The following examples uses MCPWM module to control bldc motor and vary its speed continuously

The bldc motor used for testing this code had hall sensor capture sequence of 6-->4-->5-->1-->3-->2-->6-->4--> and so on

IR2136 3-ph bridge driver is used for testing this example code

User needs to make changes according to the motor and gate driver ic used

 
## Step 1: Pin assignment
* The gpio init function initializes:
	* GPIO15 is assigned as the MCPWM signal for 1H(UH)
	* GPIO02 is assigned as the MCPWM signal for 1L(UL)
	* GPIO00 is assigned as the MCPWM signal for 2H(VH)
	* GPIO04 is assigned as the MCPWM signal for 2L(VL)
	* GPIO16 is assigned as the MCPWM signal for 3H(WH)
	* GPIO17 is assigned as the MCPWM signal for 3L(WL)
	* GPIO25 is assigned as the MCPWM capture signal for Hall A
	* GPIO26 is assigned as the MCPWM capture signal for HALL B
	* GPIO27 is assigned as the MCPWM capture signal for HALL C


## Step 2: Connection
* Connect GPIO15 with 1H/UH  of BLDC motor driver
* Connect GPIO02 with 1L/UL  of BLDC motor driver
* Connect GPIO00 with 2H/VH  of BLDC motor driver
* Connect GPIO04 with 2L/VL  of BLDC motor driver
* Connect GPIO16 with 3H/WH  of BLDC motor driver
* Connect GPIO17 with 3L/WL  of BLDC motor driver
* Connect GPIO25 to hall sensor A output
* Connect GPIO26 to hall sensor B output
* Connect GPIO27 to hall sensor C output


## Step 3: Initialize MCPWM
 * You need to set the frequency and duty cycle of MCPWM timer
 * You need to set the MCPWM channel you want to use, and bind the channel with one of the timers
 * You need to set the capture unit, for POS/NEG edge capture
 * Also reversing the hall sensor BIT weight, will make bldc motor rotate CW or CCW
