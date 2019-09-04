# Changelog
All notable changes to the EtcPal library will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
- New port: Linux
- New port: macOS
- lwpa_init() in lwpa/common.h. lwpa_init() must be called before using features
  defined by feature macros defined in lwpa/common.h.
- Extra documentation page for network interface indexes.
- New function lwpa_netint_copy_interfaces() to perform the old behavior of
  lwpa_netint_get_interfaces() (see Changed below)

### Changed
- Naming: Library name changed from lwpa to EtcPal. All API names updated
  accordingly.
- Naming: 'operating system' and 'platform' are used somewhat interchangably by
  this library, but operating system more correctly refers to what lwpa targets.
  Plus, 'os' makes for nicer names than 'plat'. A pass is made for consistency.
- Moved the sockaddr and inet_xtox conversion functions into the lwpa_inet
  module
- lwpa_thread_stop() changed to lwpa_thread_join() to better describe behavior
- LwpaNetintInfo: Removed 'gate' member, which has no consistent meaning across
  platforms.
- lwpa_log_callback interface tweak - combine the various log message pointers
  into a struct LwpaLogStrings.
- LwpaNetintInfo: 'ifindex' renamed to 'index' and changed to type unsigned int
  for better compliance with RFC 3493
- lwpa_netint_get_interface_for_dest(): Signature changed to return lwpa_error_t
- lwpa_socket: Multicast socket options now refer to network interfaces only by
  interface index, for portability and compatibility with IPv6.
- lwpa_netint_get_interfaces() simply provides const access to the cached array
  of network interfaces built at init time.
- lwpa_rbtree: Change some function signatures to give more meaningful return
  information.

### Removed

## [0.2.0] - 2019-05-29
### Added
- This CHANGELOG file.
- lwpa_mempool: LWPA_MEMPOOL_DEFINE_ARRAY(), which allows a memory pool of
  fixed-size arrays of elements.
- lwpa_uuid: Functions to generate V1 and V4 UUIDs using OS APIs where
  available.

### Changed
- lwpa is now built with CMake. Documentation updated accordingly.
- Harmonize with path-based include structure of other ETC libraries - include
  modules using #include "lwpa/[module].h"
- Windows dependencies removed from unit tests - unit tests are now
  platform-neutral.
- Added lwpa_ prefix to existing values and functions where necessary for better
  namespacing.
- lwpa_socket() signature changed to return a meaningful error code.
- lwpa_poll() removed and replaced with a new API based on context structures.
- lwpa_error: Harmonize naming style with other enums

### Removed
- lwpa_uid (lwpa_uid.h and lwpa_uid.c) and estardm (estardm.h). These are now in
  the [RDM](https://github.com/ETCLabs/RDM) repository as rdm/uid.h, rdm/uid.c,
  and rdm/defs.h, respectively.
- estardmnet (estardmnet.h). This is now in the
  [RDMnet](https://github.com/ETCLabs/RDMnet) repository as rdmnet/defs.h.
- Visual Studio project files, as we now build with CMake.

## [0.1.0] - 2018-10-18
### Added
- Initial library modules, tests and documentation.

[Unreleased]: https://github.com/ETCLabs/EtcPal/compare/master...develop
[0.2.0]: https://github.com/ETCLabs/EtcPal/compare/v0.1.0...v0.2.0
[0.1.0]: https://github.com/ETCLabs/EtcPal/releases/tag/v0.1.0
