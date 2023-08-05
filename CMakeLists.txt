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

## Create the gcov target.
#SET(CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
#set(OBJECT_DIR "${CMAKE_BINARY_DIR}/CMakeFiles/${PROJECTNAME}.dir")
#message("-- Object files will be output to: ${OBJECT_DIR}")
#add_custom_target(gcov
#    COMMAND mkdir -p coverage
#    COMMAND ${CMAKE_MAKE_PROGRAM} DataValidationgco
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
#    )
#add_custom_command(TARGET gcov
#    COMMAND echo "=================== GCOV ===================="
#    COMMAND gcov -b ${CMAKE_SOURCE_DIR}/*.cpp -o ${OBJECT_DIR} | grep -A 5 "UProfDataValidation.cpp" > CoverageSummary.tmp
#    COMMAND cat CoverageSummary.tmp
#    COMMAND echo "-- Coverage files have been output to ${CMAKE_BINARY_DIR}/coverage"
#    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/coverage
#    )
#add_dependencies(gcov ${PROJECTNAME})