file(GLOB_RECURSE FLEXPIPE_SRCS *.cc)
add_library(flexpipe SHARED ${FLEXPIPE_SRCS})

target_include_directories(flexpipe PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_options(flexpipe PUBLIC ${FLEXPIPE_DEP_CXX_FLAGS})
target_link_libraries(flexpipe ${FLEXPIPE_DEP_LINKER_FLAGS})
target_link_libraries(flexpipe ${FLEXPIPE_DEP_LINKER_LIBS})

install(TARGETS flexpipe ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
