### Modbus Master-Slave Example

## Overview

These two projects illustrate the communication between Modbus master and slave device in the segment.
Master initializes Modbus interface driver and then reads parameters from slave device in the segment.
After several successful read attempts slave sets the alarm relay (end of test condition). 
Once master reads the alarm it stops communication and destroy driver. See README.md for each project for more information.

## Wiring

The master and slave boards should be connected to each other through the RS485 interface line driver.
See the connection schematic in README.md files of each example.

## Configure the project

This example test requires communication mode setting for master and slave be the same and slave address set to 1.

## About common_component in this example

The folder "common_components" includes definitions of parameter structures for master and slave device (share the same parameters).
However, currently it is for example purpose only.

## Start example test

This test requires to set ```TEST_FW_PATH``` environment variable and other required tools and python packages.  
1. Build the master and slave examples in appropriate folders as described in README.md file of each example.
2. Set current folder to ```examples/protocols/modbus/serial/``` and start the example test script ```python example_test.py``` 
3. The script has to find two ESP32 connected boards and flash master and slave applications, checks its configuration parameters then start test.
4. Once boards flashed it starts applications and captures master and slave boards output through the terminal.
5. The script uses regular expressions to check correctness of initialization and quality of communication between boards over RS485 interface.
