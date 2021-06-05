Runs a build test to check alignment and position of `.flash.appdesc` and
`.flash.rodata` sections. Indeed, `.flash.appdesc` shall ALWAYS be aligned on
a 16-byte bounds, whereas `.flash.rodata` can have any alignment. In any case,
the end address of first one shall match the start address of the second one.
This will let both of them be merged when generating the final bin image.
The Python script that performs the checks, `check_alignment.py`, automatically
runs after the app is built.
