cmake_minimum_required(VERSION 3.0)
project(strelitzia)

########## ########## ##########

add_executable(strelitzia
  game.c
  game.h
  main.c
  scene.h
  scriptlib.c
  scriptlib.h
  version.h
)

add_custom_command(
  OUTPUT scene.h
  COMMAND ${MRBC} -g -B STRELITZIA_CLASS_SCENE -o scene.h ${CMAKE_CURRENT_LIST_DIR}/scene.rb
  DEPENDS ${CMAKE_CURRENT_LIST_DIR}/scene.rb
)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  target_compile_definitions(strelitzia PLATFORM_WINDOWS)
endif()

target_include_directories(strelitzia SYSTEM PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${SDL2_INCLUDE_DIRS}
  ${SDL2_IMAGE_INCLUDE_DIRS}
  ${SDL2_MIXER_INCLUDE_DIRS}
  ${SDL2_TTF_INCLUDE_DIRS}
  ${ONIGURUMA_INCLUDE_DIR}
  ${MRUBY_INCLUDE_DIR}
)

target_link_libraries(strelitzia
  ${SDL2_LDFLAGS}
  ${SDL2_IMAGE_LDFLAGS}
  ${SDL2_MIXER_LDFLAGS}
  ${SDL2_TTF_LDFLAGS}
  ${LIBONIG}
  ${LIBMRUBY}
)

install(
  TARGETS strelitzia
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
