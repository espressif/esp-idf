Runs a build test to check ldgen places libraries, objects and symbols
correctly as specified in the linker fragments. Specifically, this app
tests the placement for the main component, as specified in `main/linker.lf`
The Python script that performs the checks, `check_placements.py`, automatically
runs after the app is built.
