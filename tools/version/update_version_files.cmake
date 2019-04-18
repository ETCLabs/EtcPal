if(NOT NEW_VERSION_NUMBER)
  message(FATAL_ERROR "You must pass a version number!")
endif()

set(LWPA_VERSION_STRING ${NEW_VERSION_NUMBER})
string(REPLACE "." ";" LWPA_VERSION_LIST ${LWPA_VERSION_STRING})

# Build the variables we will need to configure the version header
list(GET LWPA_VERSION_LIST 0 LWPA_VERSION_MAJOR)
list(GET LWPA_VERSION_LIST 1 LWPA_VERSION_MINOR)
list(GET LWPA_VERSION_LIST 2 LWPA_VERSION_PATCH)
list(GET LWPA_VERSION_LIST 3 LWPA_VERSION_BUILD)
string(TIMESTAMP LWPA_VERSION_DATESTR "%Y-%m-%d")
string(TIMESTAMP CURRENT_YEAR "%Y")
set(LWPA_VERSION_COPYRIGHT "Copyright ${CURRENT_YEAR} ETC Inc.")

# Configure the version header
message(STATUS "Configuring versioned build for ${LWPA_VERSION_STRING}...")
get_filename_component(VERSION_DIR ${CMAKE_SCRIPT_MODE_FILE} DIRECTORY)
configure_file(${VERSION_DIR}/templates/version.h.in ${VERSION_DIR}/../../include/lwpa/version.h)
configure_file(${VERSION_DIR}/templates/current_version.txt.in ${VERSION_DIR}/current_version.txt)

# Stage the changed files
execute_process(COMMAND
  git add current_version.txt
  WORKING_DIRECTORY ${VERSION_DIR}
)
execute_process(COMMAND
  git add include/lwpa/version.h
  WORKING_DIRECTORY ${VERSION_DIR}/../..
)
configure_file(${VERSION_DIR}/templates/commit_msg.txt.in ${VERSION_DIR}/tmp_commit_msg.txt)
configure_file(${VERSION_DIR}/templates/tag_msg.txt.in ${VERSION_DIR}/tmp_tag_msg.txt)

message(STATUS "Versioned files updated.")
