add_library(libPlyIO plyIO.cc ${HDRS})
install(TARGETS libPlyIO DESTINATION "lib/3DL")
