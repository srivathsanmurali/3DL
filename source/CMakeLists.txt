#plyIO
add_library(libPlyIO plyIO.cc ${HDRS})
install(TARGETS libPlyIO DESTINATION "lib/3DL")
 
#voxelGridFilter
add_library(libVoxelGridFilter voxelGridFilter.cc ${HDRS})
install(TARGETS libVoxelGridFilter DESTINATION "lib/3DL")
