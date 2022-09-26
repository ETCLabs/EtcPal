# EtcPal: ETC Platform Abstraction Layer

This repository contains ETC's Platform Abstraction Layer (PAL), which supports other open-source
libraries by [ETC](http://www.etcconnect.com).

ETC develops many software products which target a range of platforms from full desktop OS down to
32-bit microcontrollers. Because of this, there is a need for ETC's software libraries to be highly
portable. The EtcPal library serves this purpose in two main ways:

* Provides a platform-neutral interface for common system calls, in order to simplify the writing
  of platform-neutral software libraries.
* Provides a common location for utilities that are used by multiple other software libraries.

## Documentation

EtcPal's API documentation is located [here](https://etclabs.github.io/EtcPalDocs). The "modules" entry on the sidebar provides module-by-module documentation.

## Supported Platforms

EtcPal is currently ported for the following platforms:

* FreeRTOS (OS abstraction only)
* Linux
* lwIP (Network abstraction only)
* macOS
* Microsoft Windows
* MQX

EtcPal is also buildable in no-OS (bare-metal) environments, with its platform-neutral utility
functionality still available.

## Supported Languages

C++ wrappers support C++ version 14.

C functionality supports C99 with the exception of the following features:

* variable-length arrays
* flexible array members
* designated initializers
* the "restrict" keyword

## Quality Gates

### Code Reviews

* At least 2 developers must approve all code changes made before they can be merged into the integration branch.
* API and major functionality reviews typically include application developers as well.

### Automated testing

* This consists primarily of unit tests covering the individual API modules.
* Some integration tests have also been made.

### Automated Static Analysis

* Treating warnings as errors is enabled on all platforms.
* Adding Clang Tidy (in phases) is on the TODO list. Once implemented, refer to
.clang-tidy to see which rulesets have been added.

### Automated Style Checking

* Clang format is enabled – currently this follows the style guidelines established for our libraries,
 and it may be updated from time to time. See .clang-format for more details.
* Non-conformance to .clang-format will result in pipeline failures.  The code is not automatically re-formatted.

### Continuous Integration

* A GitLab CI pipeline is being used to run builds and tests that enforce all supported quality gates for all merge
requests, and for generating new library builds from main. See .gitlab-ci.yml for details.
* All merge requests are also required to be approved by two other developers before being integrated.

### Automated Dynamic Analysis

* ASAN is currently being used when running all automated tests on Linux to catch various memory errors during runtime.

## Revision Control

EtcPal development is using Git for revision control.

## Building

EtcPal is typically included as a dependency by other software libraries. However, if you would
like to build EtcPal on its own, there are instructions in the
[documentation](https://etclabs.github.io/EtcPalDocs).

## License

EtcPal is licensed under the Apache License 2.0. EtcPal also incorporates some third-party software
with different license terms, disclosed in ThirdPartySoftware.txt in the directory containing this
README file.

## About this ETCLabs Project

EtcPal is official, open-source software developed by ETC employees and is designed to interact
with ETC products. For challenges using, integrating, compiling, or modifying this software, we
encourage posting on the [issues page](https://github.com/ETCLabs/EtcPal/issues) of this project.

Before posting an issue or opening a pull request, please read the
[contribution guidelines](./CONTRIBUTING.md).
