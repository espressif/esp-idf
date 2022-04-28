# Host test for EH_FRAME_PARSER

This test is meant to be run on a Linux x86(_64) host. The main purpose is to
trigger a SIGSEV (NULL pointer) exception at runtime in order to generate the
backtrace. It is then checked that the functions in the call stack are indeed
correctly determined in the right order.

## Requirements

A Linux host, x86 or x86_64. In any case, the example will be compiled with
the option `-m32`, it is then required to have gcc multilibs.

## Compile the example

To compile the example, simply type:
```
make
```

## Run the code

Execute the binary generated:
```
./eh_frame_test
```

If everything goes well, the output should be as is:
```
All tests passed
```

## Known issue

DWARF instructions in `x86` binaries include the instruction `DW_CFA_expression`.
However, this instruction is not supported by the parser, this is why the test
may print the following message:
```
Unsupported DWARF opcode 0: 0x10
```

This is not a big problem as the functions implemented for testing the backtrace
will not generate unimplemented DWARF instructions.
