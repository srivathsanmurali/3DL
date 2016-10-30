# stuff added by macro
add_executable(filesystemEx filesystem.cc ${HDRS})
add_executable(plyWriteEx plyWrite.cc ${HDRS})
add_executable(plyReadEx plyRead.cc ${HDRS})
add_executable(voxelGridFilterEx voxelGridFilterEx.cc ${HDRS})

# manually add dependencies here
target_link_libraries(filesystemEx libStlplus)
target_link_libraries(plyReadEx libPlyIO)
target_link_libraries(plyWriteEx libPlyIO)
target_link_libraries(voxelGridFilterEx libPlyIO libVoxelGridFilter)
