# LightWeight Platform Abstraction

This repository contains the LightWeight Platform Abstraction (lwpa) library,
which supports other open-source libraries by [ETC](http://www.etcconnect.com).

The lwpa library serves two main purposes:

* Provides a platform-neutral interface for common system calls, in order to
  simplify the writing of platform-neutral software libraries.
* Provides a common location for utilities that are used by multiple other
  software libraries.

## Platform Ports

lwpa is currently ported for the following platforms and toolchains:
+ Windows
  - Visual Studio 2015 (x86 and x86_64)

## Building

lwpa is typically included as a dependency by other software libraries.
However, if you would like to build lwpa on its own as a static lib, there are
instructions in the [documentation](https://etclabs.github.io/lwpa)
