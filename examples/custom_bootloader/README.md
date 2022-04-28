# Custom bootloader examples

The following directory contains two examples presenting the different ways
we provide in order to override the second stage bootloader or complete it
with few hooks.

## Extending the bootloader

In both cases, a project can define custom bootloader components by creating
them within a directory called `bootloader_components`.

Naming one of them `main` would let the compiler entirely override the
2nd stage bootloader with the implementation provided.

The bootloader components containing the hooks can have any name, as long
as it is part of `bootloader_components`, it will be taken into account
in the build.

## Hooks vs overriding the bootloader

In brief, using hooks will let the application add code to the bootloader.
They cannot replace the code that is already executed within bootloader.

Two hooks are available at the moment, the first one is called before the
initialization, and the second one is performed after the bootloader
initialization, before choosing and loading any partition. The
signature for these hooks can be found in `bootloader_hooks.h` file in
`components/bootloader/subproject/main/`.


On the other hand, overriding the bootloader offers the possibility to
totally or partially re-write it, in order to include, remove or modify
parts of it. Thanks to this, it will be fully customizable.
This shall only be used if heavy changes are required and they cannot
be done with hooks or within an application.
