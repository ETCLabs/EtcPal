# LightWeight Platform Abstraction                                  {#mainpage}

## Introduction

lwpa is a C-language library for platform abstraction. It supports the
writing of platform-neutral C and C++ libraries by providing a set of
modules to abstract common system calls. There are also a few
platform-neutral utilities (i.e. data structures, logging) thrown in here
and there for convenience.

lwpa modules are typically optimized for embedded platforms and thus designed
to be as thin a wrapper over the underlying OS functionality as possible.

To jump right into the documentation, check out the
[Modules Overview](@ref lwpa).

## Source Location

the sources for lwpa are located in the heirarchy:
```
src/
  [platform_name]/
    [platform-specific lwpa sources]
  [platform-neutral lwpa sources]
```

The includes are in the heirarchy:
```
include/
  [platform_name]/
    [platform-specific lwpa headers]
  [platform-neutral lwpa headers]
```
Some lwpa headers are platform-specific and duplicated for each platform.
Platform-specific headers for the same module will always conform to an
identical interface, as documented in that module's documentation.

## Platforms

lwpa is currently ported for the following platforms and toolchains:

+ Windows
  - Visual Studio 2015 (x86 and x86_64)

### Building lwpa for Your Platform

There are project files for each platform and toolchain which compile the lwpa
sources into a static library. Alternatively, your project can simply build in
the lwpa sources directly. There are pros and cons to each approach:
- The project files have predefined compilation options which may not match
  with those of your project.
- However, if building in the sources directly, your project will have to be
  updated if lwpa sources are added or removed in the future.

Especially on embedded platforms, lwpa is often dependent on project-specific
headers (i.e. FreeRTOSConfig.h, mqx.h, etc). The BUILDING.txt file in each
build directory indicates how to point the lwpa project at the relevant paths
for your project, usually via IDE workspace-level variable defines.

The projects are located in the heirarchy:

```
port/
  [platform_name]/
    [toolchain_name]/
      [project files]
      BUILDING.txt
```

Static libraries are built to:
```
lib/
  [platform_name]/
    [toolchain_name]/
      liblwpa.a
      liblwpad.a (debug lib)
      lwpa.lib (Windows)
      lwpad.lib (Windows debug lib)
```

### Platform Dependencies

The platform ports of lwpa have the following dependencies:
+ Windows
  - Windows XP or higher
