## Getting started
This repository contains FreeRTOS kernel source/header files and kernel ports only. This repository is referenced as a submodule in [FreeRTOS/FreeRTOS](https://github.com/FreeRTOS/FreeRTOS) repository, which contains pre-configured demo application projects under ```FreeRTOS/Demo``` directory. 

The easiest way to use FreeRTOS is to start with one of the pre-configured demo application projects.  That way you will have the correct FreeRTOS source files included, and the correct include paths configured.  Once a demo application is building and executing you can remove the demo application files, and start to add in your own application source files.  See the [FreeRTOS Kernel Quick Start Guide](https://www.FreeRTOS.org/FreeRTOS-quick-start-guide.html) for detailed instructions and other useful links.

Additionally, for FreeRTOS kernel feature information refer to the [Developer Documentation](https://www.FreeRTOS.org/features.html), and [API Reference](https://www.FreeRTOS.org/a00106.html).

### Getting help
If you have any questions or need assistance troubleshooting your FreeRTOS project, we have an active community that can help on the [FreeRTOS Community Support Forum](https://forums.freertos.org).

## Cloning this repository

To clone using HTTPS:
```
git clone https://github.com/FreeRTOS/FreeRTOS-Kernel.git
```
Using SSH:
```
git clone git@github.com:FreeRTOS/FreeRTOS-Kernel.git
```

## Repository structure
- The root of this repository contains the three files that are common to 
every port - list.c, queue.c and tasks.c.  The kernel is contained within these 
three files.  croutine.c implements the optional co-routine functionality - which
is normally only used on very memory limited systems.

- The ```./portable``` directory contains the files that are specific to a particular microcontroller and/or compiler. 
See the readme file in the ```./portable``` directory for more information.

- The ```./include``` directory contains the real time kernel header files.

### Code Formatting
FreeRTOS files are formatted using the "uncrustify" tool. The configuration file used by uncrustify can be found in the [FreeRTOS/FreeRTOS repository](https://github.com/FreeRTOS/FreeRTOS/blob/master/tools/uncrustify.cfg). 

### Spelling
*lexicon.txt* contains words that are not traditionally found in an English dictionary. It is used by the spellchecker to verify the various jargon, variable names, and other odd words used in the FreeRTOS code base. If your pull request fails to pass the spelling and you believe this is a mistake, then add the word to *lexicon.txt*. 
Note that only the FreeRTOS Kernel source files are checked for proper spelling, the portable section is ignored.

