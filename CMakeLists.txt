cmake_minimum_required (VERSION 2.6)

project (flvParser)

include_directories(/usr/include/libxml2) 
include_directories(mongoose)

add_executable (flvParser main.c 
  mongoose/mongoose.c)

target_link_libraries(flvParser xml2 pthread)


set(CMAKE_C_FLAGS "-g")