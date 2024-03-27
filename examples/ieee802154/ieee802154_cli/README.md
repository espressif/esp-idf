| Supported Targets | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- |

# IEEE802.15.4 Command Line Example

This example demonstrates an IEEE802.15.4 Command Line. The IEEE802.15.4 CLI exposes configuration and management APIs via a command line interface.

## How to use example

### Hardware Required

To run this example, a board with IEEE 802.15.4 module (for example ESP32-H2) is required.

### Configure the project

```
idf.py menuconfig
```

The example can run with the default configuration. IEEE802.15.4 Command Line is enabled with UART as the default interface. Additionally, USB JTAG is also supported and can be activated through the menuconfig:

```
Component config → ESP System Settings → Channel for console output → USB Serial/JTAG Controller
```

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

Now you'll get an IEEE802.15.4 command line shell.

## IEEE802.15.4 Command List

- [cca](#cca)
- [channel](#channel)
- [coordinator](#coordinator)
- [ed](#ed)
- [esp154](#esp154)
- [extaddr](#extaddr)
- [free](#free)
- [heap](#heap)
- [panid](#panid)
- [pending](#pending)
- [promisc](#promisc)
- [reg](#reg)
- [restart](#restart)
- [rx](#rx)
- [shortaddr](#shortaddr)
- [tx](#tx)
- [txpower](#txpower)
- [version](#version)
- [help](#help)

## IEEE802.15.4 Command Details

Most of the commands are available in both full and abbreviated form.
For example, to enable the receive mode, both `rx -r 1` and `rx --receive=1` are valid
```bash
> rx -r 1
I (4227508) i154cmd: RX Start
> rx --receive=1
I (4249898) i154cmd: RX Start
```
Only abbreviated formats are described below.


### cca
#### cca -g
Get the cca cfg.

```bash
> cca -g
I (2676818) i154cmd: threshold:-60 dB, mode: 1
```

#### cca -m \<mode\>
Set the cca mode.
* 0: carrier
* 1: ED
* 2: carrier or ED
* 3: carrier and ED
```bash
> cca -m 1
I (5047618) i154cmd: set cca mode as: 1
```

#### cca -v \<value\>
Set the cca threshold.
```bash
> cca -v 60
I (4731648) i154cmd: set cca threshold as: 60
```

### channel
#### channel -g
Get the channel number.
```bash
> channel -g
I (67238) i154cmd: current channel: 11
```

#### channel -s \<n\>
Set the channel number.
```bash
> channel -s 21
I (220268) i154cmd: set channel: 21
```

### coordinator
#### coordinator -d
Enable the coordinator.
```bash
> coordinator -d
I (438228) i154cmd: hardware coordinator is disabled
```

#### coordinator -e
Disable the coordinator.
```bash
> coordinator -e
I (453288) i154cmd: hardware coordinator is enabled
```

#### coordinator -g
Get the coordinator.
```bash
> coordinator -g
I (472398) i154cmd: hardware coordinator was enabled
```

### ed
#### ed -d \<duration\>
Configure the duration of energy detection.
```bash
> ed -d 2
I (668548) i154cmd: ed start
I (668548) i154cmd: ed_scan_rss_value: -98 dB
I (472398) i154cmd: hardware coordinator was enabled
```

### esp154
#### esp154 -e
Enable initialize the IEEE 802.15.4 subsystem.
```bash
> esp154 -e
I (87127) i154cmd: initialize the IEEE 802.15.4 subsystem
```

#### esp154 -d
Disable initialize the IEEE 802.15.4 subsystem.
```bash
> esp154 -d
I (97107) i154cmd: deinitialize the IEEE 802.15.4 subsystem
```

### extaddr
#### extaddr \<extaddr\>
Set the device extended address.
```bash
> extaddr 0xaa 0xbb 0xcc 0xdd 0x00 0x11 0x22 0x33
I (704937) i154cmd: set extaddr: 33221100ddccbbaa
```

#### extaddr -g
Get the device extended address.
```bash
> extaddr -g
I (615587) i154cmd: get extaddr: 0807060504030201
```

### free
Get the current size of free heap memory.
```bash
> free
235024
```

### heap
Get minimum size of free heap memory that was available during program execution.
```bash
> heap
min heap size: 231076
```

### panid
#### panid \<panid\>
Set the device Pan ID.
```bash
> panid 0x1234
I (1112117) i154cmd: set panid: 0x1234
```

#### panid -g
Get the device Pan ID.
```bash
> panid -g
I (1128457) i154cmd: current panid: 0x1234
```

### pending
#### pending \<addr\>
Add the address to pending table (2 bytes for short address, 8 bytes for extended address).
```bash
> pending 0x34 0x12
I (1806327) i154cmd: add pending shortaddr: 0x1234
```

#### pending -m \<mode\>
Set the autopending mode.
* 0: disable
* 1: enable
* 2: enhanced
```bash
> pending -m 1
I (1828817) i154cmd: set auto pending mode as: 1
```

#### pending -g
Get the autopending mode.
```bash
> pending -g
I (2007797) i154cmd: get auto pending mode as: 1
```

#### pending -r
Reset the pending address table.
```bash
> pending -r
I (2113377) i154cmd: clear the pending address table
```

#### pending -d <addr>
Delete the address from pending table (2 bytes for short address, 8 bytes for extended address).
```bash
> pending -d 0x34 -d 0x12
I (2322787) i154cmd: delete pending shortaddr: 0x1234
```

### promisc
#### promisc [-edg]
* e: Enable the promiscuous mode.
* d: Disable the promiscuous mode.
* g: Get the promiscuous mode.
```bash
> promisc -e
I (2599357) i154cmd: promiscuous mode is enabled
> promisc -d
I (2600977) i154cmd: promiscuous mode is disabled
> promisc -g
I (2602927) i154cmd: hardware promiscuous mode was disabled
```

### reg
#### reg -w \<address\> -v \<value\>
Write the register \<address\> with \<value\>.
```bash
> reg -w 0x0184 -v 0x00
I (103917) i154cmd: write reg 0x184: 0x1234
```

#### reg -r \<address\>
Read the register \<address\>.
```bash
> reg -r 0x0184
I (220967) i154cmd: reg 0x184 value: 0x1234
```

### restart
Restart the program.

### rx
#### rx -r \<mode\>
Configure the radio receive mode.
* 0: disable
* 1: enable
```bash
> rx -r 1
I (121977) i154cmd: RX Start
> rx -r 0
I (127267) i154cmd: radio exit receive mode
```

### shortaddr
#### shortaddr \<shortaddr\>
set the device short address.
```bash
> shortaddr 0x1234
I (543167) i154cmd: set shortaddr: 0x1234
```

#### shortaddr -g
```bash
> shortaddr -g
I (568387) i154cmd: current shortaddr: 0x1234
```

### tx
#### tx \<n\>
Transmit IEEE 802154 MAC frame.
* -C: Set cca trigging tx mode true.
```bash
> tx 0x00 0x11 0x22 0x33 0x44 0x55 0x66 0x77
I (6399947) i154cmd: tx sfd done, Radio state: 4
I (6399957) i154cmd: Tx Done 8 bytes
I (6399957) i154cmd: 00 11 22 33 44 55 66 77
> tx 0x00 0x11 0x22 0x33 0x44 0x55 0x66 0x77 -C
I (6580547) i154cmd: tx sfd done, Radio state: 4
I (6580547) i154cmd: Tx Done 8 bytes
I (6580547) i154cmd: 00 11 22 33 44 55 66 77
```

#### tx -l \<len\>
Transmit IEEE 802154 MAC frame wich has a length=\<len\>.
* -C: Set cca trigging tx mode true.
```bash
> tx -l 10
I (6721867) i154cmd: tx sfd done, Radio state: 4
I (6721867) i154cmd: Tx Done 10 bytes
I (6721867) i154cmd: 00 01 02 03 04 05 06 07
I (6721867) i154cmd: 08 09 00 00 00 00 00 00
> tx -l 10 -C
I (6726067) i154cmd: tx sfd done, Radio state: 4
I (6726067) i154cmd: Tx Done 10 bytes
I (6726067) i154cmd: 00 01 02 03 04 05 06 07
I (6726067) i154cmd: 08 09 00 00 00 00 00 00
```

### txpower
#### txpower -g
Get the txpower.
```bash
> txpower -g
I (7100077) i154cmd: current txpower: 20
```

#### txpower -s \<n\>
Set the txpower.
```bash
> txpower -s 40
I (7116217) i154cmd: set txpower: 40
```

### version
Get version of chip and SDK.
```bash
> version
IDF Version:v5.3-dev-699-gb94f6ef680-dirty
Chip info:
	      model:ESP32-H2
	      cores:1
	      feature:/BLE/External-Flash:2 MB
	      revision number:1
```

### help
Get the summary of all registered commands.

```bash
> help
cca  [-g] [-v <threshold>] [-m <mode>]
  cca config
  -v, --value=<threshold>  cca threshold setting
  -m, --mode=<mode>  cca mode, 0: carrier; 1: ED; 2: carrier or ED; 3: carrier and ED
     -g, --get  get the cca cfg

channel  [-g] [-s <n>]
  get/set the channel number
     -g, --get  get channel number
  -s, --set=<n>  set channel number

coordinator  [-edg]
  enable/disable/get the coordinator
  -e, --enable  enable the coordinator
  -d, --disable  disable the coordinator
     -g, --get  get the coordinator

ed  [-d <duration>]
  configure the duration
  -d, --duration=<duration>  configure the duration of energy detection

esp154  [-ed]
  enable/disable initialize the IEEE 802.15.4 subsystem
  -e, --enable  initialize the IEEE 802.15.4 subsystem
  -d, --disable  deinitialize the IEEE 802.15.4 subsystem

extaddr  [-g] [<extaddr>]...
  set/get the device extended address
     -g, --get  get the device extended address
     <extaddr>  set the device extended address

free 
  Get the current size of free heap memory

heap 
  Get minimum size of free heap memory that was available during program
  execution

help  [<string>]
  Print the summary of all registered commands if no arguments are given,
  otherwise print summary of given command.
      <string>  Name of command

log_level  <tag|*> <none|error|warn|debug|verbose>
  Set log level for all tags or a specific tag.
       <tag|*>  Log tag to set the level for, or * to set for all tags
  <none|error|warn|debug|verbose>  Log level to set. Abbreviated words are accepted.

panid  [-g] [<panid>]
  get/set the device Pan ID
       <panid>  set the device Pan ID
     -g, --get  get the device Pan ID

pending  [-gr] [-m <mode>] [<addr>]... [-d <addr>]...
  configure the pending address table
  -m, --mode=<mode>  set the autopending mode (0: disable; 1: enable; 2: enhanced)
     -g, --get  get the autopending mode (0: disable; 1: enable; 2: enhanced)
   -r, --reset  reset the pending address table
        <addr>  add the address to pending table (2 bytes for short address, 8 bytes for extended address)
  -d, --delete=<addr>  delete the address from pending table (2 bytes for short address, 8 bytes for extended address)

promisc  [-edg]
  enable/disable/get the promiscuous mode
  -e, --enable  enable the promiscuous mode
  -d, --disable  disable the promiscuous mode
     -g, --get  get the promiscuous mode

reg  [-r <address>] [-w <address>] [-v <value>]
  read/write the register
  -r, --read=<address>  read the register value
  -w, --write=<address>  register address to be written
  -v, --value=<value>  register value to be written

restart 
  Software reset of the chip

rx  [-r <mode>]
  enable/disable the receive mode
  -r, --receive=<mode>  configure the radio receive mode, 0: disable; 1: enable

shortaddr  [-g] [<shortaddr>]
  get/set the device short address
   <shortaddr>  set the device short address
     -g, --get  get the device short address

tx  [-C] [-l <len>] [<n>]... [-o <offset>]
  transmit IEEE 802154 MAC frame
  -l, --length=<len>  frame length
  -C, --cca_enable  set cca trigging tx mode true
           <n>  transmit raw frame
  -o, --offset=<offset>  tx buffer offset

txpower  [-g] [-s <n>]
  get/set the txpower
     -g, --get  get txpower
  -s, --set=<n>  set txpower,value range -80dBm~-10dBm

version 
  Get version of chip and SDK
```

#### help \<command\>
Get the summary of given command.

```bash
> help tx
tx  [-C] [-l <len>] [<n>]... [-o <offset>]
  transmit IEEE 802154 MAC frame
  -l, --length=<len>  frame length
  -C, --cca_enable  set cca trigging tx mode true
           <n>  transmit raw frame
  -o, --offset=<offset>  tx buffer offset
```
