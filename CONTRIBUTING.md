# Contributing to EtcPal

Thank you for your interest in contributing to the EtcPal project!

## Pull requests

Thanks for your interest in contributing code to the EtcPal library!

### Building and Debugging the Library

Check out the relevant [docs page](https://etclabs.github.io/EtcPal/docs/head/building_etcpal.html)
for how to build EtcPal.

When configuring with CMake, you will want to define `ETCPAL_BUILD_TESTS=ON` so that you can check
that the unit tests pass as you modify the EtcPal library. This adds a few unit test executable
targets to the build; you can run them all at once using CTest by just typing `ctest` in the build
directory (or `ctest -C [configuration]` for a multi-config CMake generator).

The EtcPal example applications can help provide immediate debugging feedback for changes made to
the library. Enable them by configuring with `ETCPAL_BUILD_EXAMPLES=ON`.

### Adding New Features

EtcPal has a strictly-controlled scope in order to keep it lightweight and prevent it from becoming
a monolithic catch-all. If your pull request adds a new feature or module to EtcPal, know that it
might not be accepted for this reason.

If you are adding a platform-abstraction feature, it is unlikely to be accepted unless it is ported
for all platforms that EtcPal supports. For feature requests, it might be better to 
[open an issue](https://github.com/ETCLabs/EtcPal/issues) so that the feature can be discussed by
EtcPal maintainers before developing it.

### Before Opening a Pull Request

* Make sure the unit tests pass
* If possible, test on multiple platforms
* Add unit tests if applicable for any regressions or new features you have added to the library
  + (This is not necessary if just modifying the example apps as they do not have test coverage)
* Format the code you've touched using clang-format (CMake creates a convenient target `reformat_all`
  which runs clang-format on all of the EtcPal sources if it is available on your PATH)

## Reporting issues

### Check to make sure your issue isn't already known

If you have identified a reproducible bug or missing feature, please do the following before
opening an issue:

* Make sure the bug or feature is not covered as a known issue in the README or documentation.
* Make sure the bug or feature is not covered in an existing open issue.

### Write a comprehensive bug report

A good bug report includes the following:

* Which library code module(s) you were using and the library's version
* A set of steps to reproduce the issue, in order
* What you expected to see, and what happened instead
* If the bug has occured in code you wrote that uses the EtcPal library, please provide code
  snippets and try to reduce to a minimal reproducible example.
* Any logging output that was produced when the issue occurred
