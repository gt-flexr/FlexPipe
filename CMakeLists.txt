cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(FLEXPIPE CXX)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_BUILD_TYPE Debug)

message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}, if you set Debug in CMakeLists.txt, you can get details")

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(PkgConfig REQUIRED)

include(cmake/utils.cmake)
util_print_os()
util_enable_code_debug_mode() # if CMAKE_BUILD_TYPE is Debug, then enable code debug mode

include(cmake/dependency/Flexpipe_Dependency.cmake)
setup_dependencies()

# Build
add_subdirectory(flexpipe) # header
add_subdirectory(source)

# # Build tests & exmamples
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
# add_subdirectory(tests)
# add_subdirectory(examples)
#
# # Installation settings
# util_set_installation()

message(STATUS "[FleXR Kernel Details]")
message("\t FLEXR_KERNEL_INCLUDE_DIRS: ${FLEXR_KERNEL_INCLUDE_DIRS}")
message("\t FLEXR_KERNEL_CXX_FLAGS:    ${FLEXR_KERNEL_CXX_FLAGS}")
message("\t FLEXR_KERNEL_LINKER_FLAGS: ${FLEXR_KERNEL_LINKER_FLAGS}")

