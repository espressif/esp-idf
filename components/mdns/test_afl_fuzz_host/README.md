## Introduction
This test uses [american fuzzy lop](http://lcamtuf.coredump.cx/afl/) to mangle real mdns packets and look for exceptions caused by the parser.

A few actuall packets are collected and exported as bins in the ```in``` folder, which is then passed as input to AFL when testing. The setup procedure for the test includes all possible services and scenarios that could be used with the given input packets. Output of the parser before fuzzing can be found in [input_packets.txt](input_packets.txt)

## Installing AFL
To run the test yourself, you need to dounload the [latest afl archive](http://lcamtuf.coredump.cx/afl/releases/afl-latest.tgz) and extract it to a folder on your computer.

The rest of the document will refer to that folder as ```PATH_TO_AFL```.

### Preparation
- On Mac, you will need to insall the latest Xcode and llvm support from [Homebrew](https://brew.sh)

    ```bash
    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    brew install --with-clang --with-lld --HEAD llvm
    export PATH="/usr/local/opt/llvm/bin:$PATH"
    ```

- On Ubuntu you need the following packages:
    
    ```bash
    sudo apt-get install make clang llvm libbsd-dev
    ```

### Compile AFL
Compiling AFL is as easy as running make:

```bash
cd [PATH_TO_AFL]
make
cd llvm_mode/
make
```

After successful compilation, you can export the following variables to your shell (you can also add them to your profile if you want to use afl in other projects)

```bash
export AFL_PATH=[PATH_TO_AFL]
export PATH="$AFL_PATH:$PATH"
```

## Running the test
Apple has a crash reporting service that could interfere with AFLs normal operation. To turn that off, run the following command:

```bash
launchctl unload -w /System/Library/LaunchAgents/com.apple.ReportCrash.plist
sudo launchctl unload -w /System/Library/LaunchDaemons/com.apple.ReportCrash.Root.plist
```

Ubuntu has a similar service. To turn that off, run as root:

```bash
echo core >/proc/sys/kernel/core_pattern
```

After going through all of the requirements above, you can ```cd``` into this test's folder and simply run ```make fuzz```.

