cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# source files
set(SOURCE_ALL SoundSystemImpl.cpp)
#set(HEADER_ALL SoundSystemImpl.hpp)

add_library(sound ${SOURCE_ALL} ${HEADER_ALL})

# link libraries
if (UNIX AND NOT APPLE)
    find_package(OpenAL REQUIRED)
    find_package(SndFile CONFIG REQUIRED)
    find_package(loguru CONFIG REQUIRED)
    target_link_libraries(sound PUBLIC OpenAL::OpenAL SndFile::sndfile loguru)
endif(UNIX AND NOT APPLE)

if (APPLE)
    find_package(SndFile CONFIG REQUIRED)
    find_package(loguru CONFIG REQUIRED)

    message("OpenAL-Soft path: $ENV{OPENAL_PATH}")
    target_link_libraries(sound PUBLIC $ENV{OPENAL_PATH}/lib/libopenal.dylib SndFile::sndfile loguru)
    target_include_directories(sound PUBLIC $ENV{OPENAL_PATH}/include/AL)
endif(APPLE)