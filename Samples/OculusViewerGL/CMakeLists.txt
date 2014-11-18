project(OculusViewerGL)

# KDL
find_package(orocos_kdl REQUIRED)
include_directories(${orocos_kdl_INCLUDE_DIRS})
link_directories(${orocos_kdl_LIBRARY_DIRS})

# All the Oculus VR functionality comes from this DLL
set(EXTRA_LIBS OculusVR)
# While the samples use the C API, they also use the 
# core SDK functionality quite a bit
list(APPEND EXTRA_LIBS ${OVR_LIBRARIES})

include_directories(../../3rdParty/glext)

# Supplementary rendering wrapper functionality
list(APPEND EXTRA_LIBS CommonSrc)


# Screen manipulation
list(APPEND EXTRA_LIBS Xinerama)

# Scene loading
list(APPEND EXTRA_LIBS TinyXML2)
find_package(OpenGL REQUIRED)
list(APPEND EXTRA_LIBS ${OPENGL_LIBRARIES})



# ROS Viewer
add_executable(OculusViewerGL
  main.cpp)
target_link_libraries(OculusViewerGL ${EXTRA_LIBS}
   ${orocos_kdl_LIBRARIES} glut GLEW)

