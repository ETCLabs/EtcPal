# EtcPal: ETC Platform Abstraction Layer

| Test Platform | Build & Test Status |
|---------------|:-------------------:|
| FreeRTOS+lwIP | [![Build & Test Status][freertos-lwip-build-badge]][azure-devops-link] |
| Linux         | [![Build & Test Status][linux-build-badge]][azure-devops-link] |
| macOS         | [![Build & Test Status][macos-build-badge]][azure-devops-link] |
| MQX           | [![Build & Test Status][mqx-build-badge]][azure-devops-link] |
| Windows       | [![Build & Test Status][win-build-badge]][azure-devops-link] |

[win-build-badge]: https://dev.azure.com/ETCLabs/EtcPal/_apis/build/status/ETCLabs.EtcPal?branchName=develop&stageName=Build%20and%20Test%20EtcPal&jobName=Windows
[macos-build-badge]: https://dev.azure.com/ETCLabs/EtcPal/_apis/build/status/ETCLabs.EtcPal?branchName=develop&stageName=Build%20and%20Test%20EtcPal&jobName=macOS
[mqx-build-badge]: https://dev.azure.com/ETCLabs/EtcPal/_apis/build/status/ETCLabs.EtcPal?branchName=develop&stageName=Build%20and%20Test%20EtcPal&jobName=MQX
[linux-build-badge]: https://dev.azure.com/ETCLabs/EtcPal/_apis/build/status/ETCLabs.EtcPal?branchName=develop&stageName=Build%20and%20Test%20EtcPal&jobName=Linux
[freertos-lwip-build-badge]: https://dev.azure.com/ETCLabs/EtcPal/_apis/build/status/ETCLabs.EtcPal?branchName=develop&stageName=Build%20and%20Test%20EtcPal&jobName=FreeRTOS%2FlwIP
[azure-devops-link]: https://dev.azure.com/ETCLabs/EtcPal/_build/latest?definitionId=4&branchName=develop 

Outside PR to test security settings

This repository contains ETC's Platform Abstraction Layer (PAL), which supports other open-source
libraries by [ETC](http://www.etcconnect.com).

ETC develops many software products which target a range of platforms from full desktop OS down to
32-bit microcontrollers. Because of this, there is a need for ETC's software libraries to be highly
portable. The EtcPal library serves this purpose in two main ways:

* Provides a platform-neutral interface for common system calls, in order to simplify the writing
  of platform-neutral software libraries.
* Provides a common location for utilities that are used by multiple other software libraries.

## Docs

EtcPal's API documentation is located [here](https://etclabs.github.io/EtcPal/docs/head). The
"modules" entry on the sidebar provides module-by-module documentation.

## Platform Ports

EtcPal is currently ported for the following platforms:
+ FreeRTOS (OS abstraction only)
+ Linux
+ lwIP (Network abstraction only)
+ macOS
+ Microsoft Windows
+ MQX

## Building

EtcPal is typically included as a dependency by other software libraries. However, if you would
like to build EtcPal on its own, there are instructions in the
[documentation](https://etclabs.github.io/EtcPal).

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
