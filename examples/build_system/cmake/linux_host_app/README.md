| Supported Targets | Linux |
| ----------------- | ----- |

This hello-world example builds a simple hello-world application for Linux.
The compiler used is the Linux-gcc.

There are two major differences to an IDF application built for an ESP chip compared to an application build for Linux:

1. The entry-point on Linux is `int main(int argc, char **argv)`, instead of `void app_main(void)` on an ESP chip.
   In this example for Linux, the `void app_main(void)` function is still included to make the connection to the IDF entry point clearer.
   However, it is simply called by `int main(int argc, char **argv)`.
   Refer to the source file [linux_host_app.cpp](main/linux_host_app.cpp) to see how it is used.

2. The project-level [CMakeLists.txt](CMakeLists.txt) for Linux is different from that of a normal IDF application for an ESP chip.
   On Linux, there is an additional line `set(COMPONENTS main)`, which clears the common requirements (default dependencies usually included in all IDF applications).
   This is currently necessary as the Linux-host feature is still under development.
   Otherwise, a lot of hardware-dependent code would be pulled in.

# Requirements
Currently, Ruby is required for the mock override of FreeRTOS.

# Build
Source the IDF environment as usual, then set the Linux target:
```bash
idf.py --preview set-target linux
```
sdkconfig.defaults sets the Linux target by default, so this not strictly necessary.

Once this is done, build the application:
```bash
idf.py build
```
Since this application runs on host, the flashing step is unnecessary.

# Run
```bash
`build/linux_host_app.elf`
```
