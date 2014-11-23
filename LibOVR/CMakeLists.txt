cmake_minimum_required(VERSION 2.8)

project(OculusSDK)

set(OculusSDK_MAJOR_VERSION 0)
set(OculusSDK_MINOR_VERSION 4)
set(OculusSDK_PATCH_VERSION 3)
set(OculusSDK_VERSION
    ${OculusSDK_MAJOR_VERSION}.${OculusSDK_MINOR_VERSION}.${OculusSDK_PATCH_VERSION}
    )

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

set(DEF_INSTALL_CMAKE_DIR lib/cmake/OculusSDK)
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# 0.4.x doesn't need Xinerama
set(EXTRA_LIBS udev pthread GL X11)  
# set(EXTRA_LIBS udev pthread GL X11 Xinerama)

include_directories(Include)
file(GLOB INCLUDE_HEADER_FILES Include/*.h)

message(STATUS "INCLUDE" ${INCLUDE_HEADER_FILES})

# CAPI
file(GLOB_RECURSE CAPI_HEADER_FILES Src/CAPI/*.h)
file(GLOB_RECURSE CAPI_SOURCE_FILES Src/CAPI/*.cpp)

# Displays
file(GLOB_RECURSE DISPLAYS_HEADER_FILES Src/Displays/*.h)
file(GLOB_RECURSE DISPLAYS_SOURCE_FILES Src/Displays/*.cpp)

# Kernel
file(GLOB_RECURSE KERNEL_HEADER_FILES Src/Kernel/*.h)
file(GLOB_RECURSE KERNEL_SOURCE_FILES Src/Kernel/*.cpp)

# Net
file(GLOB_RECURSE NET_HEADER_FILES Src/Net/*.h)
file(GLOB_RECURSE NET_SOURCE_FILES Src/Net/*.cpp)

# Sensors
file(GLOB_RECURSE SENSORS_HEADER_FILES Src/Sensors/*.h)
file(GLOB_RECURSE SENSORS_SOURCE_FILES Src/Sensors/*.cpp)

# Service
file(GLOB_RECURSE SERVICE_HEADER_FILES Src/Service/*.h)
file(GLOB_RECURSE SERVICE_SOURCE_FILES Src/Service/*.cpp)

# Tracking
file(GLOB_RECURSE TRACKING_HEADER_FILES Src/Tracking/*.h)
file(GLOB_RECURSE TRACKING_SOURCE_FILES Src/Tracking/*.cpp)

# Util 
file(GLOB_RECURSE UTIL_HEADER_FILES Src/Util/*.h)
file(GLOB_RECURSE UTIL_SOURCE_FILES Src/Util/*.cpp)

# message(STATUS "UTIL" ${UTIL_HEADER_FILES})

file(GLOB SRC_HEADER_FILES Src/*.h)
file(GLOB SRC_SOURCE_FILES Src/*.cpp)


set(HEADER_FILES
  ${INCLUDE_HEADER_FILES}
  ${SRC_HEADER_FILES}
  ${CAPI_HEADER_FILES}
  ${DISPLAYS_HEADER_FILES}
  ${KERNEL_HEADER_FILES}
  ${NET_HEADER_FILES}
  ${SENSORS_HEADER_FILES}
  ${SERVICE_HEADER_FILES}
  ${TRACKING_HEADER_FILES}
  ${UTIL_HEADER_FILES} 
  )

set(SOURCE_FILES
  ${SRC_SOURCE_FILES}
  ${CAPI_SOURCE_FILES}
  ${DISPLAYS_SOURCE_FILES}
  ${KERNEL_SOURCE_FILES}
  ${NET_SOURCE_FILES}
  ${SENSORS_SOURCE_FILES}
  ${SERVICE_SOURCE_FILES}
  ${TRACKING_SOURCE_FILES}
  ${UTIL_SOURCE_FILES}
  )

add_library(ovr SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(ovr ${EXTRA_LIBS})


# ------ Install ----------

# install header files
install(FILES ${INCLUDE_HEADER_FILES}
  DESTINATION include/OculusSDK/Include)

install(FILES ${SRC_HEADER_FILES}
  DESTINATION include/OculusSDK/Src)

install(FILES ${CAPI_HEADER_FILES}
  DESTINATION include/OculusSDK/Src/CAPI)

install(FILES ${DISPLAYS_HEADER_FILES}
  DESTINATION include/OculusSDK/Src/Displays)

install(FILES ${KERNEL_HEADER_FILES}
  DESTINATION include/OculusSDK/Src/Kernel)

install(FILES ${NET_HEADER_FILES}
  DESTINATION include/OculusSDK/Src/Net)

install(FILES ${SENSORS_HEADER_FILES}
  DESTINATION include/OculusSDK/Src/Sensors)

install(FILES ${SERVICE_HEADER_FILES}
  DESTINATION include/OculusSDK/Src/Service)

install(FILES ${TRACKING_HEADER_FILES}
  DESTINATION include/OculusSDK/Src/Tracking)

install(FILES ${UTIL_HEADER_FILES}
  DESTINATION include/OculusSDK/Src/Util)

install(TARGETS ovr
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  )


# install udev rules
option(INSTALL_UDEV_RULES "Install udev rules for Oculus Rift" OFF)
IF (INSTALL_UDEV_RULES)
  INSTALL (
    FILES 90-oculus-hydro.rules
    DESTINATION "/etc/udev/rules.d"
    COMPONENT "udev"
    )
else (INSTALL_UDEV_RULES)
  message (STATUS "Udev rules not being installed, install them with -DINSTALL_UDEV_RULES=ON")
endif (INSTALL_UDEV_RULES)


# install cmake files
file(RELATIVE_PATH REL_INCLUDE_DIR
  "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}"
  "${CMAKE_INSTALL_PREFIX}/${INSTALL_INCLUDE_DIR}")

# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
set(CONF_LIBRARY_DIRS "${PROJECT_BINARY_DIR}")
configure_file(OculusSDKConfig.cmake.in
  "${PROJECT_BINARY_DIR}/OculusSDKConfig.cmake" @ONLY)

# ... for the install tree
set(CONF_INCLUDE_DIRS "\${OculusSDK_CMAKE_DIR}/${REL_INCLUDE_DIR}")
set(CONF_LIBRARY_DIRS "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}")
configure_file(OculusSDKConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/OculusSDKConfig.cmake" @ONLY)

# ... for both
configure_file(OculusSDKConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/OculusSDKConfigVersion.cmake" @ONLY)

install(FILES
  "${PROJECT_BINARY_DIR}/${CMAKE_FILES_DIRECTORY}/OculusSDKConfig.cmake"
  "${PROJECT_BINARY_DIR}/OculusSDKConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
















  





  