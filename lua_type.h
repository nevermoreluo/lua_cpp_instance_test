//
// Created by apowo on 2021/10/22.
//

#ifndef LUA_CJSON_TEST_LUA_TYPE_H
#define LUA_CJSON_TEST_LUA_TYPE_H

#include <iostream>
#ifdef __cplusplus
extern "C"
{
#endif
#include <lua.hpp>
#include <lualib.h>
#include <lauxlib.h>
#include <lstate.h>
#pragma comment(lib, "lua.lib")

#ifdef __cplusplus
};
#endif

#include <unistd.h>

#include <memory>

class LuaEngine;

using luaRegisterFunc = void (*)(lua_State *L);

class LuaType:public std::enable_shared_from_this<LuaType>
{
public:
    LuaType();
    ~LuaType();

    int wrap_lua(lua_State* L);

    std::string toString();

    static int lua_gc(lua_State *L);
    static int lua_eq(lua_State *L);
    static int lua_toString(lua_State *L);

    static int lua__new(lua_State* L);

    static int lua_index(lua_State *L);


    static void registerLuaType(LuaEngine* eng, std::string typeName, std::string className,
                                luaRegisterFunc register_metamethods,
                                luaRegisterFunc register_methods,
                                luaRegisterFunc register_getters,
                                luaRegisterFunc register_setters);

    static int lua_newindex(lua_State *L);

    static int lua_pairs(lua_State* L);

    static void register_lua_metamethods(lua_State *L);
    static void register_lua_methods(lua_State *L);
    static void register_lua_property_setters(lua_State *L);
    static void register_lua_property_getters(lua_State *L);
    static std::shared_ptr<LuaType> checkType(lua_State* L, int index, bool errIfNot=true, bool allowNil=true);

    virtual std::string getClassName();
    virtual std::string getLuaClassName();
    static void _tq_init(LuaEngine* eng);
    static std::string TypeName;
    static std::string LuaTypeName;
};


#endif //LUA_CJSON_TEST_LUA_TYPE_H
