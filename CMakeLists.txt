cmake_minimum_required(VERSION 3.27)
project(CLAVM)

find_package(Threads)
include_directories(perfetto/sdk)
add_library(perfetto STATIC perfetto/sdk/perfetto.cc)

add_executable(clavm clavm.cpp)
add_executable(tcc tcc.cpp CLAVMAPI.cpp)
add_library(submit SHARED submit.cpp)
add_executable(test-clavmapi test-clavmapi.cpp CLAVMAPI.cpp)

target_include_directories(tcc PUBLIC ${PROJECT_SOURCE_DIR}/tcc/include)
target_include_directories(test-clavmapi PUBLIC ${PROJECT_SOURCE_DIR}/tcc/include)

target_link_libraries(clavm perfetto ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(tcc ${PROJECT_SOURCE_DIR}/tcc/lib/libtcc.a perfetto ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(submit perfetto ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(test-clavmapi ${PROJECT_SOURCE_DIR}/tcc/lib/libtcc.a perfetto ${CMAKE_THREAD_LIBS_INIT})

set_property(TARGET perfetto PROPERTY CXX_STANDARD 17)
set_property(TARGET clavm PROPERTY CXX_STANDARD 17)
set_property(TARGET tcc PROPERTY CXX_STANDARD 17)
set_property(TARGET submit PROPERTY CXX_STANDARD 17)
set_property(TARGET test-clavmapi PROPERTY CXX_STANDARD 17)
#set_property(TARGET submit PROPERTY POSITION_INDEPENDENT_CODE ON)
