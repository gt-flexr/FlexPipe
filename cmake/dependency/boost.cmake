macro(use_boost)
  find_package(Boost COMPONENTS system filesystem REQUIRED)
  if(Boost_FOUND)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
      message(STATUS "Boost Details")
      message("\t Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
      message("\t Boost_CXX_FLAGS: ${Boost_CXX_FLAGS} ${Boost_CFLAGS_OTHER}")
      message("\t Boost_LDFLAGS: ${Boost_LDFLAGS}")
    endif()

    include_directories(${Boost_INCLUDE_DIRS})
    list(APPEND FLEXPIPE_DEP_CXX_FLAGS ${Boost_CFLAGS_OTHER})
    list(APPEND FLEXPIPE_DEP_CXX_FLAGS ${Boost_CXX_FLAGS})
    list(APPEND FLEXPIPE_DEP_INCLUDE_DIRS -I${Boost_INCLUDE_DIRS})
    list(APPEND FLEXPIPE_DEP_LINKER_FLAGS ${Boost_LDFLAGS} -lboost_serialization)
  endif(Boost_FOUND)
endmacro()

