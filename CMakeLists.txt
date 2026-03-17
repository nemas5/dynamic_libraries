cmake_minimum_required(VERSION 3.16)
project(dynamic_libraries)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(main main.cpp)

set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}
)

target_link_libraries(main PRIVATE dl)

file(GLOB PLUGIN_SOURCES ${CMAKE_SOURCE_DIR}/plugins/*.cpp)
set(PLUGIN_OUTPUT_DIR ${CMAKE_SOURCE_DIR}/plugins)

foreach(plugin_src ${PLUGIN_SOURCES})
    get_filename_component(plugin_name ${plugin_src} NAME_WE)
    add_library(${plugin_name} SHARED ${plugin_src})
    set_target_properties(${plugin_name} PROPERTIES
        PREFIX ""
        LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_DIR}
    )
endforeach()