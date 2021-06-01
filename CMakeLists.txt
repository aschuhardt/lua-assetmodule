project(lua-assetmodule LANGUAGES C)

cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

find_package(Lua 5.4 REQUIRED)

if(NOT ASSET_MODULE_NAME)
  message(STATUS "ASSET_MODULE_NAME was not defined!")
else()
  add_executable(compress compress.c miniz/miniz.c)
  target_include_directories(compress PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/miniz)
  set_target_properties(compress PROPERTIES EXCLUDE_FROM_ALL TRUE)

  set(TARGET_NAME ${ASSET_MODULE_NAME})
  add_library(${TARGET_NAME} SHARED module.c miniz/miniz.c)
  target_link_libraries(${TARGET_NAME} PRIVATE ${LUA_LIBRARIES})
  target_compile_options(${TARGET_NAME} PRIVATE -fPIC -Os)
  target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/miniz
    ${LUA_INCLUDE_DIR})
  target_compile_definitions(${TARGET_NAME} PRIVATE MODULE_NAME=${ASSET_MODULE_NAME})
endif()
