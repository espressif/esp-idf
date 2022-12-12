| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

## Introduction
This test uses [american fuzzy lop](http://lcamtuf.coredump.cx/afl/) to mangle real dns, dhcp client, dhcp server packets and look for exceptions caused by the parser.

A few actual packets are collected and exported as bins in the ```in_dns, in_dhcp_client, in_dhcp_server``` folders, which is then passed as input to AFL when testing. The setup procedure for the test includes all possible services and scenarios that could be used with the given input packets. The output of the parser before fuzzing can be found in [input_packets.txt](input_packets.txt)

## Building and running the tests using AFL
To build and run the tests using AFL(afl-clang-fast) instrumentation

```bash
cd $IDF_PATH/components/lwip/test_afl_host
make fuzz MODE=dns/dhcp_client/dhcp_server
```

(Please note you have to install AFL instrumentation first, check `Installing AFL` section)

## Building the tests using GCC INSTR(off)
To build the tests without AFL instrumentations and instead of that use GCC compiler(In this case it will only check for compilation issues and will not run AFL tests).

```bash
cd $IDF_PATH/components/lwip/test_afl_host
make INSTR=off MODE=dns/dhcp_client/dhcp_server
```

## Installing AFL
To run the test yourself, you need to download the [latest afl archive](http://lcamtuf.coredump.cx/afl/releases/afl-latest.tgz) and extract it to a folder on your computer.

The rest of the document will refer to that folder as ```PATH_TO_AFL```.

### Preparation
- On Mac, you will need to install the latest Xcode and llvm support from [Homebrew](https://brew.sh)

    ```bash
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    brew install --with-clang --with-lld --HEAD llvm
    export PATH="/usr/local/opt/llvm/bin:$PATH"
    ```

- On Ubuntu you need the following packages:
    
    ```bash
    sudo apt-get install make clang-4.0(or <=4.0) llvm-4.0(or <=4.0) libbsd-dev
    ```

Please note that if specified package version can't be installed(becouse the system is newer than 2017), you can install it from source.

### Compile AFL
Compiling AFL is as easy as running make:

```bash
cd [PATH_TO_AFL]
make
cd llvm_mode/
make
```

After successful compilation, you can export the following variables to your shell (you can also add them to your profile if you want to use AFL in other projects).

```bash
export AFL_PATH=[PATH_TO_AFL]
export PATH="$AFL_PATH:$PATH"
```

Please note LLVM must be <=4.0.0, otherwise afl does not compile, as there are some limitations with building AFL on MacOS/Linux with the latest LLVM. Also, Windows build on cygwin is not fully supported.

## Additional info
Apple has a crash reporting service that could interfere with AFL's normal operation. To turn that off, run the following command:

```bash
launchctl unload -w /System/Library/LaunchAgents/com.apple.ReportCrash.plist
sudo launchctl unload -w /System/Library/LaunchDaemons/com.apple.ReportCrash.Root.plist
```

Ubuntu has a similar service. To turn that off, run as root:

```bash
echo core >/proc/sys/kernel/core_pattern
```
