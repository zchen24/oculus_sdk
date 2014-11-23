cmake_minimum_required(VERSION 2.8)
project (edid)
add_library(edid SHARED
  edid.h edid.cpp)

install(TARGETS edid
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  )
        
