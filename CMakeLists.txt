cmake_minimum_required (VERSION 3.2)
project (mongoose)

option (MAIN "Compile the main" OFF)
option (EXAMPLES "Compile examples" ON)
option (HAS_JSON11 "Enables support for Json11 (https://github.com/dropbox/json11)" OFF)
option (ENABLE_REGEX_URL "Enable url regex matching dispatcher" OFF)
option (ENABLE_OPENSSL "Enable openssl" ON)

set (JSON11_DIR "${PROJECT_SOURCE_DIR}/../json11" CACHE STRING "Json11 (https://github.com/dropbox/json11) directory")

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
    endif ()
else ()
    set (CMAKE_CXX_STANDARD 11)
endif ()


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

if (ENABLE_REGEX_URL)
    add_definitions("-DENABLE_REGEX_URL")
    SET (CMAKE_CXX_FLAGS "-std=c++11")
endif (ENABLE_REGEX_URL)

if (ENABLE_OPENSSL)
    add_definitions("-DMG_ENABLE_SSL")
    set (EXTRA_LIBS ${EXTRA_LIBS} ssl crypto)
endif(ENABLE_OPENSSL)

include_directories(${CMAKE_BINARY_DIR})
include_directories ("${PROJECT_SOURCE_DIR}/src")
include_directories ("vendor/mongoose")
include_directories ("vendor/libyuarel")
add_definitions("-DMG_ENABLE_CALLBACK_USERDATA")
add_definitions("-DMG_ENABLE_HTTP_STREAMING_MULTIPART")
add_definitions("-DMG_ENABLE_THREADSAFE_MBUF")
add_definitions("-DMG_ENABLE_HTTP_WEBSOCKET=0")

find_package (Threads)

set(HEADERS
        src/Utils.h
        src/Controller.h
        src/Request.h
        src/AbstractRequestCoprocessor.h
        src/Response.h
        src/Server.h
        src/Session.h
        src/Sessions.h
)

set(SOURCES
        src/Utils.cpp
        src/Controller.cpp
        src/Request.cpp
        src/Response.cpp
        src/Server.cpp
        src/Session.cpp
        src/Sessions.cpp
        src/mongoose.c
        src/yuarel.c
)

# Adding sockets for Win32
if (WIN32)
    set (EXTRA_LIBS ${EXTRA_LIBS} ws2_32)
else(WIN32)
    set (EXTRA_LIBS ${EXTRA_LIBS} dl)
endif (WIN32)

# Compiling library
add_library (mongoose ${SOURCES})
target_link_libraries (mongoose ${EXTRA_LIBS} ${CMAKE_THREAD_LIBS_INIT})

if (HAS_JSON11)
    add_definitions("-DHAS_JSON11")
    include_directories ("${JSON11_DIR}/include/")
    link_directories("${JSON11_DIR}/lib/")
    target_link_libraries (mongoose json11)
endif (HAS_JSON11)

# Compiling tests
if (EXAMPLES)
    add_executable (basic_auth examples/basic_auth.cpp)
    target_link_libraries (basic_auth  mongoose)

    add_executable (examples examples/examples.cpp)
    target_link_libraries (examples  mongoose)
    if (HAS_JSON11)
        target_link_libraries (examples json11)
    endif (HAS_JSON11)
endif (EXAMPLES)

# install
set (INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/include/mongoose-cpp/" CACHE PATH "The directory the headers are installed in")
set (LIB_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/lib/" CACHE PATH "The directory the library is installed in")

install (FILES ${HEADERS} DESTINATION "${INCLUDE_INSTALL_DIR}")
install (TARGETS mongoose DESTINATION src EXPORT mongoose-targets)
install (EXPORT mongoose-targets DESTINATION "src/cmake/mongoose" FILE MongooseTargets.cmake)



install (FILES ${CMAKE_CURRENT_BINARY_DIR}/MongooseConfig.cmake DESTINATION "src/cmake/mongoose")
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/version.h DESTINATION "${INCLUDE_INSTALL_DIR}")
