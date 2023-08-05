cmake_minimum_required(VERSION 3.5)
set (CMAKE_CXX_STANDARD 14)

SET(PROJECTNAME "msapp")
project("${PROJECTNAME}" VERSION 0.1.0)


#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)
file(GLOB INCLUDES "*.h")
file(GLOB SOURCES "*.cpp")

add_compile_options("-Wall" "-Wpedantic" "-Wextra" "-fexceptions" "-O0" "-g")

include_directories(${INCLUDES})
add_executable("${PROJECTNAME}" ${SOURCES})

#Link pthread
set_target_properties("${PROJECTNAME}" PROPERTIES LINK_LIBRARIES -pthread)
