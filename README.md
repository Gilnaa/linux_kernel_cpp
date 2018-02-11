# Compile C++ into your Linux Kernel #
This repository contains a simple example of how to compile C++ code into the Linux Kernel.
Code compiled is of course under many restrictions:
 - No exceptions
 - No globals with constructors
 - No inclusion of standard C++ (or even C) headers.

This repo provides the basic Makefile rules needed in order to compile and link C++ code,
as well as a glue-header which includes several basic Linux Kernel headers and fixes some compilation errors.

This was tested on Kernel `v4.15-rc6`, and should probably work on any Kernel in the `v4.*` series.
This is known not to work on any version prior to `v4.*`, but the fixes should be minimal.

# Integration #
`include` the Makefile anywhere in the buildsystem, and `#include "glue.hpp"` in your code.
After that, anywhere in the kernel where `obj-y += name.o` is specified, it will also look for `name.cpp` to compile.

# Why #
I just had a personal need for this.