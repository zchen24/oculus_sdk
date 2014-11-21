project(OpenGLTutorial)

find_package( OpenCV REQUIRED )

include_directories(../../3rdParty/glext)
set(OVR_ALL_LIBRARIES OculusVR)
list(APPEND OVR_ALL_LIBRARIES ${OVR_LIBRARIES} CommonSrc Xinerama)


add_executable(OpenGLTutorial
  mainCopy.cpp
  OculusRift.h
  OculusRift.cpp
  OculusShader.h
  OculusShader.cpp
  shader/oculus.f.glsl
  shader/oculus.v.glsl)

target_link_libraries(OpenGLTutorial
  ${orocos_kdl_LIBRARIES}
  ${OpenCV_LIBS}
  ${OVR_ALL_LIBRARIES}
  glut GLEW
  )


#add_executable(OpenGLTutorialSDK
#  mainCopySDK.cpp
#  OculusRift.h
#  OculusRift.cpp
#  OculusShaderSDK.h
#  OculusShaderSDK.cpp
#  shader/oculusSDK.f.glsl
#  shader/oculusSDK.v.glsl)

#target_link_libraries(OpenGLTutorialSDK
#  ${orocos_kdl_LIBRARIES}
#  ${OpenCV_LIBS}
#  ${OVR_ALL_LIBRARIES}
#  glut GLEW
#  )

add_custom_command(TARGET OpenGLTutorial POST_BUILD
  COMMAND ln -fs
  ${CMAKE_CURRENT_SOURCE_DIR}/shader ${CMAKE_BINARY_DIR}/output/shaderGL)

add_custom_command(TARGET OpenGLTutorial POST_BUILD
  COMMAND ln -fs
  ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/output/assetsGL)



