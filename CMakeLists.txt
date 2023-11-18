cmake_minimum_required(VERSION 3.27)
project(CLAVM VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# perfetto
#
find_package(Threads)
include_directories(perfetto/sdk)
add_library(perfetto STATIC perfetto/sdk/perfetto.cc)

# CLAVM
#
add_executable(clavm clavm.cpp rtaudio/RtAudio.cpp rtmidi/RtMidi.cpp CLAVMAPI.cpp)
target_compile_options(clavm PUBLIC -Wno-deprecated-declarations)
target_compile_definitions(clavm PUBLIC __MACOSX_CORE__)
target_include_directories(clavm PUBLIC ${PROJECT_SOURCE_DIR}/rtaudio ${PROJECT_SOURCE_DIR}/rtmidi)
target_link_libraries(clavm perfetto ${CMAKE_THREAD_LIBS_INIT}
  "-framework CoreMIDI"
  "-framework CoreAudio"
  "-framework CoreFoundation"
)

# SUBMIT
#
add_library(submit SHARED submit.cpp)
target_link_libraries(submit perfetto ${CMAKE_THREAD_LIBS_INIT})

# TCC
#
add_executable(tcc tcc.cpp CLAVMAPI.cpp)
target_include_directories(tcc PUBLIC ${PROJECT_SOURCE_DIR}/tcc/include)
target_link_libraries(tcc
  perfetto ${CMAKE_THREAD_LIBS_INIT}
  ${PROJECT_SOURCE_DIR}/tcc/lib/libtcc.a
)

# TESTING
#
add_executable(test-clavmapi test-clavmapi.cpp CLAVMAPI.cpp)
target_include_directories(test-clavmapi PUBLIC ${PROJECT_SOURCE_DIR}/tcc/include)
target_link_libraries(test-clavmapi
  perfetto ${CMAKE_THREAD_LIBS_INIT}
  ${PROJECT_SOURCE_DIR}/tcc/lib/libtcc.a
)
