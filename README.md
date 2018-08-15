# LightWeight Platform Abstraction

This repository contains the LightWeight Platform Abstraction (lwpa) library,
which supports other open-source libraries by [ETC](http://www.etcconnect.com).

The lwpa library serves two main purposes:

* Provides a platform-neutral interface for common system calls, in order to
  simplify the writing of platform-neutral software libraries.
* Provides a common location for utilities that are used by multiple other
  software libraries.

## Platform Ports

lwpa is currently ported for the following platforms:
+ Microsoft Windows
+ MQX RTOS

## Building

lwpa is typically included as a dependency by other software libraries.
However, if you would like to build lwpa on its own as a static lib, there are
instructions in the [documentation](https://etclabs.github.io/lwpa)

## About this ETCLabs Project

lwpa is official, open-source software developed by ETC employees and is
designed to interact with ETC products. For challenges using, integrating,
compiling, or modifying this software, we encourage posting on the
[issues page](https://github.com/ETCLabs/lwpa/issues) of this project.

Before posting an issue or opening a pull request, please read the
[contribution guidelines](./CONTRIBUTING.md).
