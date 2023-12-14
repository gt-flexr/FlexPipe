macro(util_print_os)
  if (UNIX)
    message(STATUS "Print OS: UNIX")
    if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
      message("\t ${CMAKE_SYSTEM_NAME}")
    elseif (${CMAKE_SYSTEM_NAME} STREQUAL "OS X")
      message("\t ${CMAKE_SYSTEM_NAME}")
    endif ()
  endif()
endmacro()


# if build type is Debug, it enables __DEBUG__ macro for debug print in source code.
macro(util_enable_code_debug_mode)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D__DEBUG__=1)
    message(STATUS "Set Debug Mode... ON")
  else()
    add_definitions(-D__DEBUG__=0)
    message(STATUS "Set Debug Mode... OFF")
  endif()
endmacro()


macro(util_set_installation)
  # Install
  string(REPLACE ";" " " FLEXPIPE_DEP_CXX_FLAGS    "${FLEXPIPE_DEP_CXX_FLAGS}")
  string(REPLACE ";" " " FLEXPIPE_DEP_INCLUDE_DIRS "${FLEXPIPE_DEP_INCLUDE_DIRS}")
  string(REPLACE ";" " " FLEXPIPE_DEP_LINKER_FLAGS "${FLEXPIPE_DEP_LINKER_FLAGS}")
  string(REPLACE ";" " " FLEXPIPE_DEP_LINKER_LIBS  "${FLEXPIPE_DEP_LINKER_LIBS}")

  if(NOT WIN32)
    if(NOT DEFINED ENV{PKG_CONFIG_PATH})
      set( PKG_CONFIG_PATH "/usr/local/lib/pkgconfig")
      message("PKG_CONFIG_PATH is not found. Set it with ${PKG_CONFIG_PATH}")
    endif(NOT DEFINED ENV{PKG_CONFIG_PATH})

    configure_file("flexpipe.pc.in" "flexpipe.pc" @ONLY)
    #install(FILES ${CMAKE_CURRENT_BINARY_DIR}/flexr.pc DESTINATION ${PKG_CONFIG_PATH}/)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/flexpipe.pc DESTINATION /usr/local/lib/pkgconfig/)
  endif(NOT WIN32)
endmacro()

