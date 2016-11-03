Build System
************

This document explains the Espressif IoT Development Framework build system and the
concept of "components"

Read this document if you want to know how to organise a new ESP-IDF project.

We recommend using the esp-idf-template_ project as a starting point for your project.

Overview
========

An ESP-IDF project can be seen as an almagation of a number of components.
For example, for a webserver that shows the current humidity, we would
have:

- The ESP32 base libraries (libc, rom bindings etc)
- The WiFi drivers
- A TCP/IP stack
- The FreeRTOS operating system
- A webserver
- A driver for an humidity sensor
- Main code tying it all together

ESP-IDF makes these components explicit and configurable. To do that, when a project
is compiled, the build environment will look up all the components in the
ESP-IDF directories, the project directories and optionally custom other component
directories. It then allows the user to configure compile-time options using
a friendly text-based menu system to customize the ESP-IDF as well as other components
to the requirements of the project. After the components are customized, the
build process will compile everything into an output file, which can then be uploaded
into a board in a way that can also be defined by components.

A project in this sense is defined as a directory under which all the files required
to build it live, excluding the ESP-IDF files and the toolchain. A simple project
tree might look like this::

    - myProject/ - build/
                 - components/ - component1/ - component.mk
                                             - Kconfig
                                             - src1.c
                               - component2/ - component.mk
                                             - Kconfig
                                             - src1.c
                 - main/       - src1.c
                               - src2.c
                 - Makefile

As we can see, a project consists of a components/ subdirectory containing its
components as well as one or more directories containing the project-specific
sources; by default a single directory called 'main' is assumed. The project
directory will also have a Makefile where the projects name as well as optionally
other options are defined. After compilation, the project directory will contain
a 'build'-directory containing all of the objects, libraries and other generated
files as well as the final binary.

Components also have a custom makefile - ``component.mk``. This contains various definititions
influencing the build process of the component as well as the project it's used
in. Components may also include a Kconfig file defining the compile-time options that are
settable by means of the menu system.

Project Makefile variables that can be set by the programmer::

   PROJECT_NAME: Mandatory. Name for the project
   BUILD_DIR_BASE: Set the directory where all objects/libraries/binaries end up in.
      Defaults to $(PROJECT_PATH)/build
   COMPONENT_DIRS: Search path for components. Defaults to the component/ directories
	  in the ESP-IDF path and the project path.
   COMPONENTS: A list of component names. Defaults to all the component found in the
	  COMPONENT_DIRS directory
   EXTRA_COMPONENT_DIRS: Defaults to unset. Use this to add directories to the default
	  COMPONENT_DIRS.
   SRCDIRS: Directories under the project dir containing project-specific sources.
	  Defaults to 'main'. These are treated as 'lite' components: they do not have
	  include directories that are passed to the compilation pass of all components and
	  they do not have a Kconfig option.

Component-specific component.mk variables that can be set by the programmer::

    COMPONENT_ADD_INCLUDEDIRS: Relative path to include directories to be added to
	    the entire project. If an include directory is only needed to compile this
	    specific component, don't add it here.
    COMPONENT_PRIV_INCLUDEDIRS: Relative path to include directories that are only used
	    when compiling this specific component.
    COMPONENT_DEPENDS: Names of any components that need to be compiled before this component.
    COMPONENT_ADD_LDFLAGS: LD flags to add for the entire project. Defaults to -l$(COMPONENT_NAME).
	    Add libraries etc in the current directory as $(abspath libwhatever.a)
    COMPONENT_EXTRA_INCLUDES: Any extra include paths used when compiling the component's
	    source files. These will be prefixed with '-I' and passed to the compiler.
		Similar to COMPONENT_PRIV_INCLUDEDIRS, but these paths are passed as-is instead of
		expanded relative to the component directory.
    COMPONENT_SRCDIRS: Relative directories to look in for sources. Defaults to '.', the current
	    directory (the root of the component) only. Use this to specify any subdirectories. Note
   	    that specifying this overwrites the default action of compiling everything in the
	    components root dir; to keep this behaviour please also add '.' as a directory in this
	    list.
    COMPONENT_OBJS: Object files to compile. Defaults to the .o variants of all .c and .S files
	    that are found in COMPONENT_SRCDIRS.
    COMPONENT_EXTRA_CLEAN: Files that are generated using rules in the components Makefile
	    that also need to be cleaned
    COMPONENT_BUILDRECIPE: Recipe to build the component. Optional. Defaults to building all
	    COMPONENT_OBJS and linking them into lib(componentname).a
    COMPONENT_CLEANRECIPE: Recipe to clean the component. Optional. Defaults to removing
	    all built objects and libraries.
    COMPONENT_BUILD_DIR: Equals the cwd of the component build, which is the build dir
	    of the component (where all the .o etc files should be created).

These variables are already set early on in the Makefile and the values in it will
be usable in component or project Makefiles::

    CC, LD, AR, OBJCOPY: Xtensa gcc tools
    HOSTCC, HOSTLD etc: Host gcc tools
    LDFLAGS, CFLAGS: Set to usable values as defined in ESP-IDF Makefile
    PROJECT_NAME: Name of the project, as set in project makefile
    PROJECT_PATH: Path to the root of the project folder
    COMPONENTS: Name of the components to be included
    CONFIG_*: All values set by 'make menuconfig' have corresponding Makefile variables.

Inside your component's component.mk makefile, you can override or add to these variables
as necessary. The changes are isolated from other components (see Makefile.projbuild below
if you want to share these changes with all other components.)

For components, there also are these defines::

    COMPONENT_PATH: Absolute path to the root of the source tree of the component we're
	    compiling
    COMPONENT_LIBRARY: The full path to the static library the components compilation pass
    	is supposed to generate

Make Process
------------

The Make process is always invoked from the project directory by the
user; invoking it anywhere else gives an error. This is what happens if
we build a binary:

The Makefile first determines how it was included. It figures out
various paths as well as the components available to it. It will also
collect the ldflags and includes that the components specify they need.
It does this by running a dummy make on the components with a "get_variable"
target that will output these values.

The Makefile will then create targets to build the lib*.a libraries of
all components and make the elf target depend on this. The main Makefile
invokes Make on the componen.mk of each components inside a sub-mke: this way
the components have full freedom to do whatever is necessary to build
the library without influencing other components. By default, the
component.mk includes the utility makefile $(IDF_PATH)/make/component_common.mk.
This provides default targets and configurations that will work
out-of-the-box for most projects.

KConfig
-------

Each component can also have a Kconfig file, alongside the component.mk, that contains
details to add to "menuconfig" for this component.

Makefile.projbuild
------------------

For components that have parts that need to be evaluated in the top-level
project context, you can create a file called Makefile.projbuild in the
component root directory. These files is included into the project's
top-level Makefile.

For example, if your component needs to add to CFLAGS for the entire
project (not just for its own source files) then you can set
``CFLAGS +=`` in Makefile.projbuild. Note that this isn't necessary for
adding include directories to the project, you can set
``COMPONENT_ADD_INCLUDEDIRS`` (see above) in the component.mk.


KConfig.projbuild
-----------------

There's an equivalent to Makefile.projbuild for KConfig: if you want to include
options at the top-level, not inside the 'components' submenu then create a Kconfig.projbuild and
it will be included in the main menu of menuconfig.

Take good care when (re)defining stuff here: because it's included with all the other
.projbuild files, it's possible to overwrite variables or re-declare targets defined in
the ESP-IDF makefile/Kconfig and other .projbuild files. It's generally better to just
create a KConfig file, if you can.


Writing Component Makefiles
---------------------------

A component consists of a directory which doubles as the name for the
component: a component named 'httpd' lives in a directory called 'httpd'
Because components usually live under the project directory (although
they can also reside in an other folder), the path to this may be
something like  /home/myuser/projects/myprojects/components/httpd .

Components can have any name (unique to the project) but the name
cannot contain spaces (esp-idf does not support spaces in paths).

One of the things that most components will have is a component.mk makefile,
containing instructions on how to build the component. Because the
build environment tries to set reasonable defaults that will work most
of the time, component.mk can be very small.

Simplest component.mk
=====================

At the  minimum, component.mk will just include the ESP-IDF component "common" makefile,
which adds common component functionality::

    include $(IDF_PATH)/make/component_common.mk

This will take all the .c and .S files in the component root and compile
them into  object files, finally linking them into a library.


Adding source directories
=========================

By default, subdirectories are ignored. If your project has sources in subdirectories
instead of in the root of the component then you can tell that to the build
system by setting COMPONENT_SRCDIRS::

    COMPONENT_SRCDIRS := src1 src2
    include $(IDF_PATH)/make/component_common.mk

This will compile all source files in the src1/ and src2/ subdirectories
instead.

Specifying source files
=======================

The standard component.mk logic adds all .S and .c files in the source
directories as sources to be compiled unconditionally. It is possible
to circumvent that logic and hardcode the objects to be compiled by
manually setting the COMPONENT_OBJS variable to the name of the
objects that need to be generated::

    COMPONENT_OBJS := file1.o file2.o thing/filea.o thing/fileb.o anotherthing/main.o
    include $(IDF_PATH)/make/component_common.mk


Adding conditional configuration
================================

The configuration system can be used to conditionally compile some files
dependending on the options selected in ``make menuconfig``:

Kconfig::
    config FOO_ENABLE_BAR
	    bool "Enable the BAR feature."
    	help
	      	This enables the BAR feature of the FOO component.

Makefile::
    COMPONENT_OBJS := foo_a.o foo_b.o $(if $(CONFIG_FOO_ENABLE_BAR),foo_bar.o foo_bar_interface.o)
    include $(IDF_PATH)/make/component_common.mk


Source Code Generation
======================

Some components will have a situation where a source file isn't supplied
with the  component itself but has to be generated from another file. Say
our component has a header file that consists of the converted binary
data of a BMP file, converted using a hypothetical tool called bmp2h. The
header file is then included in as C source file called graphics_lib.c::

    COMPONENT_EXTRA_CLEAN := logo.h

    graphics_lib.o: logo.h

    logo.h: $(COMPONENT_PATH)/logo.bmp
        bmp2h -i $^ -o $@

    include $(IDF_PATH)/make/component_common.mk

In this example, graphics_lib.o and logo.h will be generated in the
current directory (the build directory) while logo.bmp comes with the
component and resides under the component path. Because logo.h is a
generated file, it needs to be cleaned when make clean is called which
why it is added to the COMPONENT_EXTRA_CLEAN variable.

Cosmetic Improvements
=====================

The above example will work just fine, but there's one last cosmetic
improvement that can be done. The make system tries to make the make
process somewhat easier on the  eyes by hiding the commands (unless you
run make with the V=1 switch) and this does  not do that yet. Here's an
improved version that will output in the same style as  the rest of the
make process::

   COMPONENT_EXTRA_CLEAN := test_tjpgd_logo.h

   graphics_lib.o: logo.h

    logo.h: $(COMPONENT_PATH)/logo.bmp
	    $(summary) BMP2H $@
	    $(Q) bmp2h -i $^ -o $@

    include $(IDF_PATH)/make/component_common.mk

Fully Overriding The Component Makefile
---------------------------------------

Obviously, there are cases where all these recipes are insufficient for a
certain component, for example when the component is basically a wrapper
around another third-party component not originally intended to be
compiled under this build system. In that case, it's possible to forego
the build  system entirely by setting COMPONENT_OWNBUILDTARGET and
possibly  COMPONENT_OWNCLEANTARGET and defining your own build- and clean
target. The build target can do anything as long as it creates
$(COMPONENT_LIBRARY) for the main file to link into the project binary,
and even that is not necessary: if the COMPONENT_ADD_LDFLAGS variable
is set, the component can instruct the linker to do anything else as well.


.. _esp-idf-template: https://github.com/espressif/esp-idf-template
