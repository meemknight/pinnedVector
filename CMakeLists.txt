cmake_minimum_required(VERSION 3.13)
project(pinnedVector)


add_library(pinnedVector INTERFACE)
target_include_directories(pinnedVector INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

if(WIN32)
	target_compile_definitions(pinnedVector INTERFACE _WIN32) 
  
endif()



#Define PROFILER_LIB_SOURCES to be a list of all the source files for the test 
file(GLOB_RECURSE PROFILER_LIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/demo/*.cpp")
add_executable(pinnedVector_test)
target_sources(pinnedVector_test PRIVATE ${PROFILER_LIB_SOURCES} )
target_link_libraries(pinnedVector_test PUBLIC pinnedVector)



