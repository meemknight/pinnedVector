cmake_minimum_required(VERSION 3.5)
project(pinnedVectorTest)

add_subdirectory(pinnedVector)

#Define PROFILER_LIB_SOURCES to be a list of all the source files for the test 
file(GLOB_RECURSE PROFILER_LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.cpp")
add_executable(pinnedVector_test)
target_sources(pinnedVector_test PRIVATE ${PROFILER_LIB_SOURCES} )
target_link_libraries(pinnedVector_test PUBLIC pinnedVector)
set_property(TARGET pinnedVector_test PROPERTY CXX_STANDARD 17)



