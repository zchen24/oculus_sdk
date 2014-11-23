cmake_minimum_required(VERSION 2.8)

project(OculusSDK)

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

add_subdirectory(3rdParty/EDID)
add_subdirectory(LibOVR)
#add_subdirectory(Samples/OculusWorldDemo)

install(FILES package.xml DESTINATION share/oculus_sdk)

