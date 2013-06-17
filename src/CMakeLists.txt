
add_executable (flvParser main.c 
  ${flvParser_SOURCE_DIR}/mongoose/mongoose.c 
  core.c
  inputFilters.c
  outputFilters.c
  )

target_link_libraries(flvParser xml2 pthread)


