cmake_minimum_required(VERSION 3.10)

# Project name and version
project(setproxy VERSION 0.1.0 LANGUAGES C)

# Add executable target
add_executable(setproxy setproxy.c)

# Set compiler flags
target_compile_options(setproxy PRIVATE -Wall -Wextra -O2)

# Set C standard
set_target_properties(setproxy PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

# Install the binary
install(TARGETS setproxy
    RUNTIME DESTINATION bin
)

# Add custom target for cleaning
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_LIST_DIR}/cmake_clean.cmake
)

# Generate a cmake_clean.cmake file
file(WRITE ${CMAKE_CURRENT_LIST_DIR}/cmake_clean.cmake
"
file(REMOVE_RECURSE
    ${CMAKE_BINARY_DIR}/CMakeFiles
    ${CMAKE_BINARY_DIR}/CMakeCache.txt
    ${CMAKE_BINARY_DIR}/cmake_install.cmake
    ${CMAKE_BINARY_DIR}/Makefile
    ${CMAKE_BINARY_DIR}/setproxy
)
"
)
