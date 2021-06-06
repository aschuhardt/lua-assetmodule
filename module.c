#ifndef MODULE_NAME
#error No MODULE_NAME was specified, so there is nothing to build.
#endif

#include <lauxlib.h>
#include <lua.h>
#include <miniz.h>
#include <stdlib.h>
#include <string.h>

// this nonsense took way too long to figure out
//
#define MAKE_SIGNATURE(prefix, name) prefix##name(lua_State* L)
#define MODULE_SIGNATURE(name) MAKE_SIGNATURE(luaopen_, name)

#define MAKE_METATABLE_NAME(prefix, name, suffix) prefix #name suffix
#define METATABLE_NAME(name) MAKE_METATABLE_NAME("assetmodule_", name, "_meta")

#define MAKE_STRINGIFY(value) #value
#define STRINGIFY(value) MAKE_STRINGIFY(value)

// clang-format off

#define __module_header(contents) #contents
#define _module_header(name) __module_header(name-asset-data.h)
#define module_header(name) _module_header(name)

// clang-format on

#include module_header(MODULE_NAME)

#define FIELD_LENGTH "length"
#define FIELD_BUFFER "buffer"

int cleanup_buffer(lua_State* L) {
  lua_getfield(L, 1, FIELD_BUFFER);
  if (lua_islightuserdata(L, -1)) {
    unsigned char* buffer = (unsigned char*)lua_touserdata(L, -1);
    if (buffer != NULL) {
      free(buffer);
    }
  }

  return 0;
}

int push_buffer_as_string(lua_State* L) {
  lua_getfield(L, 1, FIELD_BUFFER);
  unsigned char* buffer = (unsigned char*)lua_touserdata(L, -1);

  lua_getfield(L, 1, FIELD_LENGTH);
  size_t length = (size_t)luaL_checkinteger(L, -1);

  lua_pushlstring(L, (const char*)buffer, length);

  return 1;
}

void create_metatable(lua_State* L) {
  luaL_Reg methods[] = {{"__gc", cleanup_buffer},
                        {"__string", push_buffer_as_string},
                        {NULL, NULL}};
  luaL_newlib(L, methods);
  lua_setfield(L, LUA_REGISTRYINDEX, METATABLE_NAME(MODULE_NAME));
}

void delete_metatable(lua_State* L) {
  lua_pushnil(L);
  lua_setfield(L, LUA_REGISTRYINDEX, METATABLE_NAME(MODULE_NAME));
}

// clang-format off

#ifdef _WIN32
__declspec(dllexport)
#endif
int MODULE_SIGNATURE(MODULE_NAME) {

  // clang-format on

  create_metatable(L);

  lua_newtable(L);

  luaL_setmetatable(L, METATABLE_NAME(MODULE_NAME));

  unsigned char* buffer = malloc(asset_size * sizeof(unsigned char));
  if (buffer == NULL) {
    luaL_error(L,
               "Failed to allocate %lu bytes for asset stored in module \"%s\"",
               asset_size, STRINGIFY(MODULE_NAME));
    return 0;
  }

  unsigned long decompressed_size = asset_size;
  int status = uncompress(buffer, &decompressed_size, &asset_data[0],
                          sizeof(asset_data));
  if (status != Z_OK) {
    luaL_error(L, "Failed to decompress asset stored in module \"%s\"",
               STRINGIFY(MODULE_NAME));
    delete_metatable(L);
    free(buffer);
    return 0;
  }

  lua_pushlightuserdata(L, buffer);
  lua_setfield(L, -2, FIELD_BUFFER);

  lua_pushinteger(L, decompressed_size);
  lua_setfield(L, -2, FIELD_LENGTH);

  return 1;
}
