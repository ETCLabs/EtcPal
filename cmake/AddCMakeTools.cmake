# Add CMake Tools
include(FetchContent)

set(CMAKE_TOOLS_GIT_TAG e205a214805a8945268876ac2d350f1715a77c69)
if(DEFINED ENV{CI})
  set(CMAKE_TOOLS_GIT_REPO https://gitlab-ci-token:$ENV{CI_JOB_TOKEN}@gitlab.etcconnect.com/etc/common-tech/tools/cmake-tools.git)
else()
  # If your project does not use GitLab CI, you can remove this if/else and simply use this URL
  set(CMAKE_TOOLS_GIT_REPO git@gitlab.etcconnect.com:etc/common-tech/tools/cmake-tools.git)
endif()

FetchContent_Declare(
  cmake_tools
  GIT_REPOSITORY ${CMAKE_TOOLS_GIT_REPO}
  GIT_TAG ${CMAKE_TOOLS_GIT_TAG}
)
FetchContent_MakeAvailable(cmake_tools)
set(CMAKE_TOOLS_MODULES ${cmake_tools_SOURCE_DIR}/modules)
