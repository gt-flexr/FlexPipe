set(FLEXPIPE_DEP_CXX_FLAGS "")
set(FLEXPIPE_DEP_LINKER_FLAGS "")
set(FLEXPIPE_DEP_LINKER_LIBS "")
set(FLEXPIPE_DEP_INCLUDE_DIRS "")

# Built on top of RaftLib
include(cmake/dependency/RaftLib.cmake)

# SpdLog for Logging
include(cmake/dependency/Spdlog.cmake)

# Catch for test cases
include(cmake/dependency/catch2.cmake)

# Boost
include(cmake/dependency/boost.cmake)

# Communication Libs
include(cmake/dependency/LibZMQ.cmake) # TCP-based socket communication with diverse patterns
include(cmake/dependency/uvgRTP.cmake) # Real-time Transport Protocol (RTP) for time-critical data
include(cmake/dependency/flexr_shm_queue.cmake) # Shared memory queue for inter-process communication


macro(setup_dependencies)
  use_raftlib()
  use_spdlog()
  use_catch2()
  use_boost()
  use_libzmq()
  use_uvgrtp()
  use_shmq()

  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "[Flexpipe] dependency flags")
    message("\tCXX_FLAGS: ${FLEXPIPE_DEP_CXX_FLAGS}")
    message("\tLINKER_FLAGS: ${FLEXPIPE_DEP_LINKER_FLAGS}")
    message("\tLINKER_LIBS: ${FLEXPIPE_DEP_LINKER_LIBS}")
    message("\tINCLUDE_DIRS: ${FLEXPIPE_DEP_INCLUDE_DIRS}")
  endif()
endmacro()
