project(OculusViewer)

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
add_executable(OculusViewer
  ROSViewer.h ROSViewer.cpp
  ROSViewer_Scene.cpp)
target_link_libraries(OculusViewer ${EXTRA_LIBS})

