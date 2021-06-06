project(lua-assetmodule LANGUAGES C)

cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)

find_package(Lua 5.4 REQUIRED)

if(NOT ASSET_MODULE_NAME)
  message(STATUS "ASSET_MODULE_NAME was not defined!")
else()
  add_library(miniz SHARED miniz/miniz.c)
  target_compile_options(miniz PRIVATE -fpic -O2)
  target_include_directories(miniz PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/miniz)
  target_compile_definitions(miniz PRIVATE
    _LARGEFILE64_SOURCE=1
    MINIZ_NO_STDIO=1
    MINIZ_NO_TIME=1
    MINIZ_NO_ARCHIVE_APIS=1
    MINIZ_NO_ARCHIVE_WRITING_APIS=1)

  add_executable(compress compress.c)
  target_link_libraries(compress PRIVATE miniz)
  set_target_properties(compress PROPERTIES EXCLUDE_FROM_ALL TRUE)

  set(TARGET_NAME ${ASSET_MODULE_NAME})
  add_library(${TARGET_NAME} SHARED module.c)
  set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
  target_link_libraries(${TARGET_NAME} PRIVATE miniz ${LUA_LIBRARIES})
  target_compile_options(${TARGET_NAME} PRIVATE -fpic -Os -fno-unwind-tables
    -fno-asynchronous-unwind-tables -fno-math-errno -fno-unroll-loops
    -fmerge-all-constants -fno-stack-protector -ffunction-sections
    -fdata-sections -fno-ident -fno-stack-protector)
  target_link_options(${TARGET_NAME} PRIVATE -Wl,--gc-sections -Wl,-z,norelro
    -Wl,--build-id=none)
  target_include_directories(${TARGET_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LUA_INCLUDE_DIR})
  target_compile_definitions(${TARGET_NAME} PRIVATE MODULE_NAME=${ASSET_MODULE_NAME})
endif()
