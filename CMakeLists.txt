cmake_minimum_required (VERSION 2.6)

project (flvParser)

include_directories(/usr/include/libxml2) 

add_executable (flvParser main.c)

target_link_libraries(flvParser xml2)


