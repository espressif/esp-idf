import multiprocessing
import os
import platform

# Make flavors, across the various kinds of Windows environments & POSIX...
if "MSYSTEM" in os.environ:  # MSYS
    MAKE_CMD = "make"
    MAKE_GENERATOR = "MSYS Makefiles"
elif os.name == "nt":  # other Windows
    MAKE_CMD = "mingw32-make"
    MAKE_GENERATOR = "MinGW Makefiles"
elif platform.system() == "FreeBSD":
    MAKE_CMD = "gmake"
    MAKE_GENERATOR = "Unix Makefiles"
else:
    MAKE_CMD = "make"
    MAKE_GENERATOR = "Unix Makefiles"

GENERATORS = [
    # ('generator name', 'build command line', 'version command line', 'verbose flag')
    ("Ninja", ["ninja"], ["ninja", "--version"], "-v"),
    (
        MAKE_GENERATOR,
        [MAKE_CMD, "-j", str(multiprocessing.cpu_count() + 2)],
        [MAKE_CMD, "--version"],
        "VERBOSE=1",
    ),
]
GENERATOR_CMDS = dict((a[0], a[1]) for a in GENERATORS)
GENERATOR_VERBOSE = dict((a[0], a[3]) for a in GENERATORS)

SUPPORTED_TARGETS = ["esp32", "esp32s2beta"]
