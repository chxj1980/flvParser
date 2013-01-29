cmake_minimum_required (VERSION 2.6)

project (flvParser)

set(CMAKE_C_FLAGS "-g")

include_directories(/usr/include/libxml2) 
include_directories(mongoose)



add_executable (flvParser main.c 
  mongoose/mongoose.c)

target_link_libraries(flvParser xml2 pthread)

add_custom_target(cscope find . /usr/include/libxml2/ | egrep -i '\\.\([chly]\(xx|pp\)*|cc|hh\)$$'  | grep -v CMakeFiles > cscope.files
  COMMAND cscope -q -b -i cscope.files
  COMMAND etags -L cscope.files

  )

