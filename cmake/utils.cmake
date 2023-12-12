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
  string(REPLACE ";" " " FLEXR_CORE_CXX_FLAGS    "${FLEXR_CORE_CXX_FLAGS}")
  string(REPLACE ";" " " FLEXR_CORE_INCLUDE_DIRS "${FLEXR_CORE_INCLUDE_DIRS}")
  string(REPLACE ";" " " FLEXR_CORE_LINKER_FLAGS "${FLEXR_CORE_LINKER_FLAGS}")
  string(REPLACE ";" " " FLEXR_CORE_LINKER_LIBS  "${FLEXR_CORE_LINKER_LIBS}")

  string(REPLACE ";" " " FLEXR_KERNEL_CXX_FLAGS    "${FLEXR_KERNEL_CXX_FLAGS}")
  string(REPLACE ";" " " FLEXR_KERNEL_LINKER_FLAGS "${FLEXR_KERNEL_LINKER_FLAGS}")
  string(REPLACE ";" " " FLEXR_KERNEL_LINKER_LIBS  "${FLEXR_KERNEL_LINKER_LIBS}")

  set(FLEXR_HEADER flexr)
  install(FILES ${CMAKE_SOURCE_DIR}/${FLEXR_HEADER} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

  if(NOT WIN32)
    if(NOT DEFINED ENV{PKG_CONFIG_PATH})
      set( PKG_CONFIG_PATH "/usr/local/lib/pkgconfig")
      message("PKG_CONFIG_PATH is not found. Set it with ${PKG_CONFIG_PATH}")
    endif(NOT DEFINED ENV{PKG_CONFIG_PATH})

    configure_file("flexr.pc.in" "flexr.pc" @ONLY)
    #install(FILES ${CMAKE_CURRENT_BINARY_DIR}/flexr.pc DESTINATION ${PKG_CONFIG_PATH}/)
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/flexr.pc DESTINATION /usr/local/lib/pkgconfig/)
  endif(NOT WIN32)
endmacro()

