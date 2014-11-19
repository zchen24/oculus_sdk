project(OpenGLTutorial)

find_package( OpenCV REQUIRED )

#add_executable(OpenGLTutorial
#  mainOpenGL.cpp
#  shader/sample.vs
#  shader/sample.fs)

add_executable(OpenGLTutorial
  mainCopy.cpp
  shader/sample.vs
  shader/sample.fs
  shader/hello-gl.f.glsl
  shader/hello-gl.v.glsl)

target_link_libraries(OpenGLTutorial
  ${orocos_kdl_LIBRARIES}
  ${OpenCV_LIBS}
  glut GLEW
  )

add_custom_command(TARGET OpenGLTutorial POST_BUILD
  COMMAND ln -fs
  ${CMAKE_CURRENT_SOURCE_DIR}/shader ${CMAKE_BINARY_DIR}/output/shaderGL)

add_custom_command(TARGET OpenGLTutorial POST_BUILD
  COMMAND ln -fs
  ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/output/assetsGL)



