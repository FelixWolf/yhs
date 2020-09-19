# -*- cmake -*-

project(
    "yocto"
    VERSION 0.1
    DESCRIPTION "Yocto Web Server"
)

set(SOURCE_FILES
    main.cpp
    yhs.cpp
)

set(HEADER_FILES
    main.h
)

list(APPEND SOURCE_FILES ${HEADER_FILES})

#This is such a small binary, go ahead and do all the nice things each time
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O3 -s")

if(UNIX)
    set(CMAKE_EXECUTABLE_SUFFIX ".x86_64")
elseif(WIN32)
    set(CMAKE_EXECUTABLE_SUFFIX ".exe")
else()
endif()

add_executable(server ${SOURCE_FILES})