# SPI Slave Halfduplex: Segment Mode Examples

(See the README.md file in the 'examples' directory for more information about IDF examples.)

These two projects illustrate the SPI Slave Halfduplex Segment Mode.
* ``seg_slave`` shows one of the ways to use the SPI Slave Halfduplex driver. There are 2 tasks repeating to receive/send transactions from/to SPI Master respectively.
* ``seg_master`` shows how to use the ESP Serial Slave Link APIs to communicate with SPI Slave Halfduplex driver. It receives/sends transactions from/to slave a few times.

## How to use example

### Hardware Required

These two projects are supposed to be flashed onto two seperate boards and jumped together via correctly connected SPI pins defined in both of the ``app_main.c`` files. Once they are connected and flashed, they will use the SPI Master and SPI Slave Halfduplex drivers to communicate with each other.

Following is the connection between 2 ESP32S2 boards:

| Signal    | Master | Slave  |
|-----------|--------|--------|
| MOSI      | GPIO11 | GPIO11 |
| MISO      | GPIO13 | GPIO13 |
| SCLK      | GPIO12 | GPIO12 |
| CS        | GPIO10 | GPIO10 |

Feel free to change the GPIO settings by editing the macro definations on the top of the ``app_main.c``

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to build, flash and monitor projects.

## Example Output

``seg_master``
```
---------SLAVE INFO---------

Slave MAX Send Buffer Size:       5000
Slave MAX Receive Buffer Size:    120
I (328) SEG_MASTER: RECEIVING......
Transaction No.0 from slave, length: 4640
I (338) SEG_MASTER: SENDING......
I (348) SEG_MASTER: RECEIVING......
Transaction No.1 from slave, length: 3299
I (348) SEG_MASTER: SENDING......
I (358) SEG_MASTER: RECEIVING......
Transaction No.2 from slave, length: 4836
I (368) SEG_MASTER: SENDING......
I (368) SEG_MASTER: RECEIVING......
Transaction No.3 from slave, length: 3333
I (378) SEG_MASTER: SENDING......
I (378) SEG_MASTER: RECEIVING......
Transaction No.4 from slave, length: 4965
I (388) SEG_MASTER: SENDING......
I (388) SEG_MASTER: RECEIVING......
Transaction No.5 from slave, length: 3042
I (398) SEG_MASTER: SENDING......
I (408) SEG_MASTER: RECEIVING......
Transaction No.6 from slave, length: 4892
I (408) SEG_MASTER: SENDING......
I (418) SEG_MASTER: RECEIVING......
Transaction No.7 from slave, length: 3585
I (428) SEG_MASTER: SENDING......
I (428) SEG_MASTER: RECEIVING......
Transaction No.8 from slave, length: 3348
I (438) SEG_MASTER: SENDING......
I (438) SEG_MASTER: RECEIVING......
Transaction No.9 from slave, length: 4985
I (448) SEG_MASTER: SENDING......
I (448) SEG_MASTER: RECEIVING......
Transaction No.10 from slave, length: 3710
...
```

``seg_slave``
```
77 bytes are received:
this is master's transaction 0
109 bytes are received:
this is master's transaction 1
83 bytes are received:
this is master's transaction 2
97 bytes are received:
this is master's transaction 3
47 bytes are received:
this is master's transaction 4
89 bytes are received:
this is master's transaction 5
80 bytes are received:
this is master's transaction 6
96 bytes are received:
this is master's transaction 7
83 bytes are received:
this is master's transaction 8
110 bytes are received:
this is master's transaction 9
113 bytes are received:
this is master's transaction 10
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you.
