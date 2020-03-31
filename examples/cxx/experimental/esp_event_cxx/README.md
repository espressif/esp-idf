# ESP Event synchronous example

(See the README.md file in the upper level 'examples' directory for more information about examples.)


## How to use example

### Configure the project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (409) ESP Event C++: started event loop
event base test, ID: 0; called first round
event base test, ID: 1; called first round
received timeout
event base test, ID: 0; called second round
event base test, ID: 1; called second round
event base test, ID: 0; called second round
event base test, ID: 1; called second round
event base test, ID: 0; called second round
event base test, ID: 1; called second round
event base test, ID: 0; called second round
event base test, ID: 1; called second round
I (10419) ESP Event C++: Missed: 0 events
```
