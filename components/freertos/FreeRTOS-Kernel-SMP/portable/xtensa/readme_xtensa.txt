    FreeRTOS Port for Xtensa Configurable and Diamond Processors
    ============================================================

                FreeRTOS Kernel Version 10.0.0


Introduction
------------

This document describes the Xtensa port for FreeRTOS multitasking RTOS.
For an introduction to FreeRTOS itself, please refer to FreeRTOS
documentation.

This port currently works with FreeRTOS kernel version 10.0.0.


Xtensa Configuration Requirements and Restrictions
--------------------------------------------------

The Xtensa configurable architecture supports a vast space of processor
features. This port supports all of them, including custom processor
extensions defined in the TIE language, with certain minimum
requirements. You must use Xtensa Tools to compile and link FreeRTOS and
your application for your Xtensa configuration. The port uses the Xtensa
Hardware Abstraction Layer (HAL) to adapt to your Xtensa configuration.
NOTE: It may be possible to build and run this with the open-source
xtensa-linux tools provided you have the correct overlay for your Xtensa
configuration. However, this has not been tested and is currently not
supported by Cadence.

This port includes optional reentrancy support for the 'newlib' and
'xclib' C runtime libraries distributed with Xtensa Tools, providing
thread-safety on a per task basis (for use in tasks only, not interrupt
handlers).

NOTE: At this time only 'newlib' and 'xclib' C libraries are supported
for thread safety. The 'uclibc' library is not reentrant and does not
provide thread safety at this time. However, if you are not concerned
with reentrancy then you can use any of these libraries.

This port also includes a simple example application that may run on
a supported board or the Xtensa instruction set simulator (ISS). There
are also a couple of test programs used in maintaining the port, which
serve as additional examples.

FreeRTOS for Xtensa configurable processors requires the following minimum
processor configuration options:
- Timer interrupt option with at least one interruptible timer.
- Interrupt option (implied by the timer interrupt option).
- Exception Architecture 2 (XEA2). Please note that XEA1 is NOT supported.
  All 'Diamond', 'Xtensa 6', 'Xtensa LX' and 'Xtensa LX2' processors and
  most 'Xtensa T1050' processors are configured with XEA2.
All Diamond processor cores meet these requirements and are supported.

Minimal support for certain evaluation boards is provided via a board
independent XTBSP API implemented by a board specific library distributed
with the Xtensa Tools. This provides the board clock frequency and basic
polled drivers for the display and console device. Note that XTBSP
is not a tradtional RTOS "board support package" with RTOS specific
interrupt-driven drivers - it is not specific to any RTOS. Note that
FreeRTOS can run on any Xtensa or Diamond board without this board support
(a "raw" platform), but you will have to provide the clock frequency
and drivers for any on-board devices you want to use.


Installation
------------

The Xtensa port of FreeRTOS is available at this location:

    https://github.com/foss-xtensa/amazon-freertos

This download includes the core FreeRTOS source and include files needed
to build the port. You can also download the official release of FreeRTOS
version 1.0.0 or later from this location:

    https://github.com/aws/amazon-freertos

The Xtensa port files are currently not included in the official package.

All source is provided along with a Makefile that works for any host
platform supported by Xtensa Tools (Windows, Linux). These instructions
are written for Windows users, but can easily be understood and adapted
to other host platforms.

First install the FreeRTOS common package in a directory of your choosing.
The structure of that package will look like this:

<install directory>
|-- demos
|   `-- cadence
|       `-- sim
|           |-- common
|           |   |-- application_code
|           |   |   `-- cadence_code
|           |   `-- config_files
|           `-- xplorer
`-- lib
    |-- FreeRTOS
    |   `-- portable
    |       |-- Common
    |       |-- MemMang
    |       `-- XCC
    |           `-- Xtensa
    `-- include
        `-- private

The Xtensa Tools are available from Cadence as part of a processor
license. Be sure you have installed the Xtensa Tools and your processor
configuration.


Building FreeRTOS for Xtensa
----------------------------

To build the FreeRTOS library and the example programs, go into the
directory 'demos/cadence/sim' and use the makefile in that directory.
"make all" will build all the examples. There is another makefile in
the 'lib/FreeRTOS/portable/XCC/Xtensa' directory that builds just the
FreeRTOS library.

By default, you will build for the Xtensa instruction set simulator. If
you have a supported emulation board, you can build to run on that. You
can also build to run on a raw Xtensa core with no board support, a
good starting point for supporting your own target platform. Cadence
recommends doing functional development on the simulator because it
is easier to debug with, then move to a board if/when you need to test
hardware drivers or real-time performance.

The provided makefile simplifies building FreeRTOS and the example
for your Xtensa configuration and platform (ISS, board, etc.). There
are detailed instructions in the comments at the top of the makefile.

The makefiles work on Windows and Linux and support incremental builds.
The build for each Xtensa configuration and target platform is placed in
a subdirectory so several core and platform builds can co-exist even with
incremental rebuilds. You may specify the root of the build area (if tou
want it to be elsewhere than under the source tree) by defining BLDROOT
either in the make command or your shell environment.


Building the FreeRTOS Library
-----------------------------

First, be sure you have installed Xtensa Tools and your processor
configuration, and be sure that Xtensa Tools are in your search path.
You can use xt-make, which comes with the Xtensa Tools, to run the
makefiles.

Change directories to the Xtensa port directory:

> cd lib/FreeRTOS/portable/XCC/Xtensa

Now build the FreeRTOS RTOS as a library (libfreertos.a) as follows:

> xt-make

which by default builds for the simulator (TARGET=sim), or:

> xt-make TARGET=board

which builds for a supported board. Note that the board type does not
need to be specified when building the FreeRTOS library.

If you are building for an Xtensa processor configuration that is not the
default you selected when you installed Xtensa Tools, you need to define the
environment variable XTENSA_CORE. If your configuration is not in the
default registry you selected when you installed Xtensa Tools, you also
need to define the environment variable XTENSA_SYSTEM. See tools manuals.
You can avoid defining these in your environment if you pass the variables
you need to redefine into xt-make as follows:

> xt-make XTENSA_CORE=<your_config_name> XTENSA_SYSTEM=<your_registry> ...

There are more details about build options in the comment in the Makefile.

After the library has been built, you must link your application with this
library in order to use FreeRTOS.


Building the FreeRTOS Examples
------------------------------

The provided examples are designed to run on the Xtensa instruction set
simulator (ISS) or a supported evaluation board programmed with your
Xtensa processor configuration.

To build the examples for the default platform (simulator):

> cd demos/cadence/sim

> xt-make all

which is the same as

> xt-make all TARGET=sim

The boards currently supported are the Xilinx ML605 and KC705 FPGA
development boards. To target these boards, type

> xt-make all TARGET=ml605

or

> xt-make all TARGET=kc705

To build in a location other than the default, specify the new location
using the BLDROOT variable. Note that this makefile will invoke the
FreeRTOS library build makefile automatically, passing on the relevant
parameters based on what you specified.

You can override the default compilation options by specifying the new
options via CFLAGS. For example:

> xt-make all TARGET=sim CFLAGS="-O2 -Os -g"

This compiles the examples and links them with the FreeRTOS library
libfreertos.a and the appropriate linker-support package (LSP) for your
target platform (you can override the LSP by adding LSP=<lsp> to the
xt-make command line). The resulting ELF files can be downloaded and
executed on the target. The example binaries appear in the platform
specific subdirectory described earlier.

To build your application with thread-safe C library support, you
need to make certain modifications to the application to plug in and
invoke the reentrancy support. This allows each task to use the library
without interference with other tasks (it is not safe for interrupt
handlers to call the C library).

First, you must define

    XT_USE_THREAD_SAFE_CLIB

to a nonzero value either in xtensa_config.h or on the compiler's command
line. Note that the default xtensa_config.h provided with this port does
define this to 1 if either newlib or xclib is detected.

Then, you must also make sure to allocate extra space on the stack for
each task that will use the C library reentrant functions. This extra
space is to be allocated over and above the actual stack space required
by the task itself. The define

    XT_STACK_EXTRA_CLIB

specifies the amount of extra space to be added on to the stack to allow
saving the context for the C library as well as the coprocessors if any.
E.g. if your task requires 2000 bytes of stack space, you must allocate
(2000 + XT_STACK_EXTRA_CLIB) bytes for the stack.


IMPORTANT NOTE
--------------

The header file FreeRTOS.h, which is a part of the core FreeRTOS sources,
includes <reent.h> if thread safety for the C libraries is enabled. For
xclib, this file exists in <sys/reent.h> and so is reported as missing.
To work around this, the makefiles supplied with this port will copy the
reent.h header into the build directory during the build process. If you
use a different build process, then you must make sure to copy this file
to a location that is included in the list of include paths. This can be
the build directory or the directory that contains the Xtensa port source
files.


Running or Debugging an Application
-----------------------------------

To execute the example application on the simulator:

> xt-run [--turbo] example.exe

The option --turbo provides much faster, but non-cycle-accurate simulation
(the --turbo option is only available with Xtensa Tools version 7 or later).


To execute on the simulator using the Xplorer GUI based debugger:

> xplorer --debug example.exe


To execute on a supported evaluation board, download example.exe per
instructions in the tools manuals. Be sure the board has been programmed
with the correct configuration and is set up to boot from RAM and debug
a downloaded program!  Optionally you may connect a terminal or terminal
emulator to the serial port on the board with settings as described in
the board user manual, and see the output of printf on the terminal.

To obtain I/O on a "raw" platform such as an unsupported board, you need
to provide low level I/O drivers (eg. inbyte() and outbyte() for character
I/O if you want to use printf etc.).  You can run "raw" executables on
any Xtensa platform, including simulator and any board, but you will not
see any behavior specific to the platform (eg. display, printed output,
stopping simulation at end of program). You can, while debugging, use a
debugger mechanism called GDBIO to obtain basic I/O. To use GDBIO, link
with the gdbio LSP. Refer to Xtensa tools documentation for details.


Task Stack Sizes
----------------

The application must ensure that every task has enough space for its
stack. Each task needs enough space for its own use, its own interrupt
stack frame (defined in xtensa_context.h) and space to save coprocessor
state, if any. Several factors influence the size of the stack required,
including the compiler optimization level and the use of the C library.
Calls to standard output functions such as printf() can use up a lot of
stack space. The tool xt-stack-usage is helpful in determining safe stack
sizes for your application.

Some macros are provided in xtensa_config.h to help determine the stack
size for tasks that do and do not use the C library. Use these as the
basis for each task's stack size. They are minimum requirements taking
into account your configuration and use of the C library. In particular,
the define

    XT_STACK_MIN_SIZE

defines the minimum stack size for any task. Be very careful if you try
to use a stack size smaller than this minimum. Stack overruns can cause
all kinds of hard-to-debug errors. It is recommended that you enable the
FreeRTOS stack checking features during development.

WARNING: The newlib printf() function uses a lot of stack space. Be very
careful in using it. Optionally you can use the 'libxtutil' library for
output - it implements a subset of printf() that has smaller code size
and uses far less stack space. More information about this library is in
the Xtensa Tools documentation.


Interrupt Stack
---------------

Beginning with port version 1.2, the port uses a separate interrupt stack
for handling interrupts. Thus, it is no longer necessary for each task to
reserve space on its stack to handle interrupts. The size of the interrupt
stack is controlled by the parameter "configISR_STACK_SIZE" defined in
FreeRTOSConfig.h. Define this carefully to match your system requirements.


Assembler / Compiler Switches
-----------------------------

The following are compiler switches are used by the provided
Makefile in building the FreeRTOS library and example application.
These can be modified by editing the Makefile or by overriding the
CFLAGS variable in the make command line, for example:

> xt-make CFLAGS="-O2 -DXT_USE_THREAD_SAFE_CLIB"

    -g                      Specifies debug information.
    -c                      Specifies object code generation.
    -On                     Sets compiler optimization level n (default -O0).
    -mlongcalls             Allows assembler and linker to convert call
                            instructions to longer indirect call sequences
                            when target is out of range.
    -x assembler-with-cpp   Passes .s and .S files through C preprocessor.
    -Dmacro                 Define a preprocessor macro with no value.
    -Dmacro=value           Define a preprocessor macro with a value.

See the compiler / linker documentation for a full list of switches and
their use.

Many definitions can be provided at compile-time via the -D option
without editing the source code. Here are some of the more useful ones:

    XT_USE_THREAD_SAFE_CLIB Enable support for the reentrancy to provide
                            thread-safety for the newlib and xclib libraries
                            supplied with Xtensa Tools. Default ON.

    Note, the follwing defines are unique to the Xtensa port so have names
    beginning with "XT_".

    XT_SIMULATOR            Set this if building to run on the simulator.
                            Takes advantage of certain simulator control
                            and reporting facilities, and adjusts timing
                            of periodic tick to provide a more acceptable
                            performance in simulation (see XT_CLOCK_FREQ).
                            Set by default unless PLATFORM is overridden.

    XT_BOARD                Set this if building for a supported board.
                            Be sure to specify the correct LSP for the
                            board. See the example makefile for usage.

    XT_CLOCK_FREQ=freq      Specifies the target processor's clock
                            frequency in Hz. Used primarily to set the
                            timer that generates the periodic interrupt.
                            Defaults are provided and may be edited in
                            xtensa_timer.h (see comments there also).
                            Default for simulator provides more acceptable
                            performance, but cannot provide real-time
                            performance due to variation in simulation
                            speed per host platform and insufficient
                            cycles between interrupts to process them.
                            Supported board platforms by default leave
                            this undefined and compute the clock frequency
                            at initialization unless this is explicitly
                            defined.

    XT_TICK_PER_SEC=n       Specifies the frequency of the periodic tick.

    XT_TIMER_INDEX=n        Specifies which timer to use for periodic tick.
                            Set this if your Xtensa processor configuration
                            provides more than one suitable timer and you
                            want to override the default. See xtensa_timer.h .

    XT_INTEXC_HOOKS         Enables hooks in interrupt vector handlers
                            to support dynamic installation of exception
                            and interrupt handlers. Disabled by default.

    XT_USE_OVLY             Enable code overlay support. It uses a mutex,
                            hence configUSE_MUTEX must be enabled. This
                            option is currently unsupported.

    XT_USE_SWPRI            Enable software prioritization of interrupts.
                            Enabling this will prioritize interrupts with
                            higher bit numbers over those with lower bit
                            numbers at the same level. This works only for
                            low and medium priority interrupts that can be
                            dispatched to C handlers.


Register Usage and Stack Frames
-------------------------------

The Xtensa architecture specifies two ABIs that determine how the general
purpose registers a0-a15 are used: the standard windowed ABI use with
the Xtensa windowed register file architecture, and the optional and
more conventional Call0 ABI (required for Xtensa configurations without
a windowed register file).

Xtensa processors may have other special registers (including co-processor
registers and other TIE "states") that are independent of this choice
of ABI. See Xtensa documentation for more details.

In the windowed ABI the registers of the current window are used as follows:
    a0      = return address
    a1      = stack pointer (alias sp)
    a2      = first argument and result of call (in simple cases)
    a3-7    = second through sixth arguments of call (in simple cases).
              Note that complex or large arguments are passed on the
              stack. Details are in the Xtensa Tools manuals.
    a8-a15  = available for use as temporaries.
There are no callee-save registers. The windowed hardware automatically
saves registers a0-a3 on a call4, a0-a8 on a call8, a0-a12 on a call12,
by rotating the register window. Hardware triggers window overflow and
underflow exceptions as necessary when registers outside the current
window need to be spilled to preallocated space in the stack frame, or
restored. Complete details are in the Xtensa manuals. The entire windowed
register file is saved and restored on interrupt or task context switch.

The Call0 ABI does not make use of register windows, relying instead
on a fixed set of 16 registers without window rotation.
The Call0 ABI is more conventional and uses registers as follows:
    a0      = return address
    a1      = stack pointer (alias sp)
    a2      = first argument and result of call (in simple cases)
    a3-7    = second through sixth arguments of call (in simple cases).
              Note that complex or large arguments are passed on the
              stack. Details are in the Xtensa Tools manuals.
    a8-a11  = scratch.
    a12-a15 = callee-save (a function must preserve these for its caller).
On a FreeRTOS API call, callee-save registers are saved only when a task
context switch occurs, and other registers are not saved at all (the caller
does not expect them to be preserved). On an interrupt, callee-saved
registers might only be saved and restored when a task context-switch
occurs, but all other registers are always saved and restored.

An Xtensa processor has other special registers independent of the ABI,
depending on the configuration (including co-processor registers and other
TIE state) that are part of the task context. FreeRTOS preserves all such
registers over an unsolicited context-switch triggered by an interrupt.
However it does NOT preserve these over a solicited context-switch during
a FreeRTOS API call. This bears some explanation. These special registers
are either ignored by the compiler or treated as caller-saved, meaning
that if kept "live" over a function call (ie. need to be preserved)
they must be saved and restored by the caller. Since solicited entry to
FreeRTOS is always made by a function call, FreeRTOS assumes the caller
has saved any of these registers that are "live". FreeRTOS avoids a lot
of overhead by not having to save and restore every special register
(there can be many) on every solicited context switch.

As a consequence, the application developer should NOT assume that special
registers are preserved over a FreeRTOS API call such as vTaskDelay().
If multiple tasks use a register, the caller must save and restore it.

The saved context stack frames for context switches that occur as
a result of interrupt handling (interrupt frame) or from task-level
API calls (solicited frame) are described in human readable form in
xtensa_context.h .  All suspended tasks have one of these two types
of stack frames. The top of the suspended task's stack is pointed to
by pxCurrentTCB->pxTopOfStack. A special location common to both stack
frames differentiates solicited and interrupt stack frames.


Improving Performance, Footprint, or Ease of Debugging
------------------------------------------------------

By default FreeRTOS for Xtensa is built with debug (-g) and without
compiler optimizations (-O0). This makes debugging easier. Of course,
-O0 costs performance and usually also increases stack usage. To make
FreeRTOS run faster you can change the Makefile to enable the desired
optimizations or set a predefined optimization level (-O<level>) .

Maximum performance is achieved with -O3 -ipa, but that might increase
the footprint substantially. A good compromise is -O2. See the compiler
manual for details.

Minimal footprint is achieved by optimizing for space with -Os, at the
cost of some performance. See the compiler manual for details.

The Xtensa architecture port-specific assembly files are coded with no
file-scope labels inside functions (all labels inside functions begin with
".L"). This allows a profiler to accurately associate an address with a
function, and also allows the debugger's stack trace to show the correct
function wherever the program counter is within that function. However
there are some tradeoffs in debugging. Local (".L") labels are not
visible to the debugger, so the following limitations may be observed
during debugging:
- You cannot set a breakpoint on a local label inside a function.
- Disassembly will show the entire function, but will get out of sync and
  show incorrect opcodes if it crosses any padding before an aligned local
  branch target (".L" label, not ".Ln"). Restart disassembly specifying an
  address range explicitly between points where there is padding.
Since FreeRTOS is provided in source form, it is not difficult to remove
the ".L" and ".Ln" prefixes from local labels if you want them visible.
They can also be made visible by passing the '-L' option to the assembler
and linker (see the assembler and linker manuals for details).


Interrupt and Exception Handling
--------------------------------

FreeRTOS provides a complete set of efficient exception and first-level
interrupt handlers installed at the appropriate exception and interrupt
vector locations. The Xtensa architecture supports several different
classes of exceptions and interrupts. Being a configurable architecture,
many of these are optional, and the vector locations are determined by
your processor configuration. (Note that Diamond cores are pre-configured
with specific vector locations.) The handlers provided use conditional
compilation to adapt to your processor configuration and include only
the code that is needed.

Xtensa vector locations may reside almost anywhere, including in ROM.
The amount of code space available at each of these locations is
often very small (e.g. due to following vectors). A small stub of
code installed at the vector jumps to the corresponding handler,
usually in RAM. The exception and interrupt handlers are defined in
xtensa_vectors.S. They are not specific to FreeRTOS, but call into
FreeRTOS where appropriate via macros defined in xtensa_rtos.h .

The handlers provided for low and medium priority interrupts are just
dispatchers that save relevant state and call user-definable handlers.
See the files xtensa_vectors.S and xtensa_api.h for more details of how
to create and install application-specific user interrupt handlers.
Similarly, user-defined handlers can be installed for exceptions (other
than a few which are always handled by the OS).

The high priority interrupt handlers provided may be considered templates
into which the application adds code to service specific interrupts.
The places where application handlers should be inserted are tagged with
the comment "USER_EDIT" in xtensa_vectors.S.

This FreeRTOS port supports strict priority-based nesting of interrupts.
An interrupt may only nest on top of one of strictly lower priority.
Equal priority interrupts concurrently pending are handled in an
application-defined sequence before any lower priority interrupts
are handled. During interrupt and exception handling, the processor's
interrupt level (PS.INTLEVEL) is used to control the interrupt priority
level that can be accepted; interrupt sources are not controlled
individually by FreeRTOS (the application is free to access the INTENABLE
register directly to enable/disable individual interrupts, eg. using
Xtensa HAL services). This approach provides the most deterministic
bounds on interrupt latency (for a given priority) and stack depth.

Software prioritization of interrupts at the same priority is controlled
by the definition of XT_USE_SWPRI. See above for a description of this
parameter.

The following subsections describe the handling of each class of exception
and interrupt in more detail. Many have nothing to do with FreeRTOS but
are mentioned because there is code to handle them in xtensa_vectors.S.

User Exception and Interrupt Handler (Low/Medium Priority):

    All Xtensa 'general exceptions' come to the user, kernel, or double
    exception vector.  The exception type is identified by the EXCCAUSE
    special register (level 1 interrupts are one particular cause of a
    general exception). This port sets up PS to direct all such exceptions
    to the user vector. Exceptions taken at the other two vectors usually
    indicate a kernel or application bug.

    Level 1 interrupts are identified at the beginning of the handler
    and are dispatched to a dedicated handler. Then, syscall and alloca
    exceptions are identified and dispatched to special handlers described
    below. After this, coprocessor exceptions are identified and dispatched
    to the coprocessor handler.

    Any remaining exceptions are processed as follows:

    Having allocated the exception stack frame, the user exception handler
    saves the current task state and sets up a C environment and enables
    the high-priority class of interrupts (which do not interact with
    FreeRTOS), then reads EXCCAUSE and uses the cause (number) to index
    into a table of user-specified handlers. The correct handler is then
    called. If the handler returns, the context is restored and control is
    returned to the code that caused the exception. The user-defined handler
    may alter the saved context, or any other system state, that allows the
    faulting instruction to be retried.

    If the cause is a level 1 (low-priority) or medium-priority interrupt,
    the handler enables all interrupts above that priority level after
    saving the task context. It then sets up the environment for C code
    and then calls the handler (found in the handler table) for the
    interrupt number. If the user has not specified a handler, then the
    default handler will be called, which will terminate the program.

    If the interrupt is for the system timer, it calls a special interrupt
    handler for the system timer tick, which calls _frxt_timer_int then
    clears its bit from the mask. This interrupt cannot be hooked by the
    user-defined handler.

    Finally, the handler calls _frxt_int_exit to allow FreeRTOS to perform
    any scheduling necessary and return either to the interrupted task
    or another.

    If software prioritization is enabled, the handler will re-enable all
    interrupts at the same level that are numerically higher than the current
    one, before calling the user handler. This allows a higher priority
    interrupt to pre-empt the lower priority handler.

Medium Priority Interrupt Handlers:

    Medium priority interrupts are those at levels 2 up to XCHAL_EXCM_LEVEL,
    a configuration-specific maximum interrupt level affected by the global
    'exception mode' bit in the processor status word (PS.EXCM).
    Interrupt levels above XCHAL_EXCM_LEVEL are of the high-priority class.
    The Xtensa hardware documentation considers medium priority interrupts
    to be a special case of high-priority interrupts, but from a software
    perspective they are very different.

    Dispatch of medium-priority interrupts is discussed in the section
    above.

High Priority Interrupt Handlers:

    High priority interrupts are those strictly above XCHAL_EXCM_LEVEL,
    a configuration-specific maximum interrupt level affected by the
    global 'exception mode' bit in the processor status word (PS.EXCM).
    High priority handlers may not directly interact with FreeRTOS at all,
    and are described here only for the sake of completeness. They must
    be coded in assembler (may not be coded in C) and are intended to be
    used for handling extremely high frequency hardware events that need
    to be handled in only a few cycles. A high priority interrupt handler
    may trigger a software interrupt at a medium or low priority level to
    occasionally signal FreeRTOS. Please see Xtensa documentation.

    There is a separate vector and a few special registers for each high
    priority interrupt, providing for fast dispatch and efficient nesting
    on top of lower priority interrupts. Handlers are templates included
    only for the vectors that exist in your Xtensa processor configuration.
    These templates are written for only one interrupt per high priority
    level to minimize latency servicing very fast time-critical interrupts.
    The vector code jumps to the corresponding first-level interrupt handler,
    which then executes application-provided assembler code before returning
    quickly to the interrupted task or lower priority handler.

Kernel Exception Handler:

    Kernel mode is not used in this port of FreeRTOS, and therefore kernel
    exceptions should not happen. A stub is provided for the vector that
    triggers the debugger (if connected) or calls _xt_panic to freeze the
    processor should a kernel exception occur.

Alloca Exception Handler:

    Alloca exceptions are generated by the 'movsp' instruction, which
    is used only in the windowed ABI. Its purpose is to allocate some
    space on top of the stack. Because the window hardware may have
    spilled some registers to the 16 byte "base save" area below the
    stack pointer, it is necessary to protect those values. The alloca
    handler accomplishes this quickly without setting up an interrupt
    frame or entering FreeRTOS, by emulating a register underflow and
    re-executing 'movsp'.

Syscall Exception Handler:

    Syscall exceptions are generated by a 'syscall' instruction.
    The windowed ABI specifies that executing this instruction with
    a value of zero in register a2 must spill any unsaved registers
    in the windowed register file to their pre-determined locations
    on the caller's stack. The handler does exactly that, and skips
    over the 'syscall' instruction before returning to the caller.
    If a2 is non-zero, the handler returns a2 == -1 to the caller.

Co-Processor Exception Handler:

    A co-processor exception is generated when a task accesses a
    co-processor that it does not "own". Ownership represents which
    task's state is currently in the co-processor. Co-processors are
    context-switched "lazily" (on demand) only when a non-owning task
    uses a co-processor instruction, otherwise a task retains ownership
    even when it is preempted from the main processor. The co-processor
    exception handler performs the context-switch and manages ownership.

    Co-processors may not be used by any code outside the context of a
    task. A co-processor exception triggered by code that is not part
    of a running task is a fatal error and FreeRTOS for Xtensa will panic.
    This restriction is intended to reduce the overhead of saving and
    restoring co-processor state (which can be quite large) and in
    particular remove that overhead from interrupt handlers.

Debug Exception Handler:

    A debug exception is caused as a result of running code, such as by
    a 'break' instruction or hardware breakpoints and watchpoints, or
    as a result of an external debug interrupt, such as from an OCD based
    debugger or multiprocessor debug events ("breakin/breakout"). If the
    processor is running in OCD mode under control of an OCD-based debugger,
    the trigger event immediately halts the processor and gives control to
    the OCD debugger. Otherwise control is transferred to the debug vector.
    The debug vector handler calls the simulator if running on the ISS,
    which then takes control and interacts with any attached debugger.
    If running on hardware and not in OCD mode, debug exceptions are not
    expected, so the debug handler calls _xt_panic to freeze the processor.

Double Exception Handler:

    A double exception is a general exception that happens while the
    processor is in exception mode (PS.EXCM set), and thus indicates a
    bug in kernel code. The double exception vector handler triggers
    the debugger (if connected) or calls _xt_panic to freeze the
    processor.

Window Overflow and Underflow Exception Handlers:

    Window overflow and underflow handlers are required for use of the
    windowed ABI. Each has its own dedicated vector and highly optimized
    code that is independent of OS. See Xtensa documentation for details.

Hooks for Dynamic Installation of Handlers:

    Optional hooks are provided in the user exception and low level
    interrupt handler and all medium and high priority interrupt handlers,
    to dynamically install a handler function (which may be coded in C,
    unless in a high-priority interrupt handler). These hooks are enabled
    and used by automatic regression tests, they are not part of a normal
    FreeRTOS build. However an application is free to take advantage of
    them. The interrupt/exception hooks are described in xtensa_rtos.h .

    It is recommended that the application not make use of these hooks, but
    rather use xt_set_interrupt_handler() and xt_set_exception_handler()
    to install application-specific handlers. This method is more convenient
    and allows arguments to be passed to the handlers. Software prioritization
    of interrupts works only with this method. See xtensa_api.h for details.

Overlay Support

    Code overlays are currently not supported for FreeRTOS. This will be
    supported in a future release. Make sure that the option XT_USE_OVLY is
    never defined when building.


-End-
