//
// Created by apowo on 2021/10/22.
//

#ifndef LUA_CJSON_LuaEngine_LUA_ENGINE_H
#define LUA_CJSON_LuaEngine_LUA_ENGINE_H

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
#include <vector>

#include "lua_type.h"
#include "lua_instance.h"

int addLuaCPath(lua_State* m_pLuaState, const char* path);






class LuaEngine
{
public:
    LuaEngine() ;
    ~LuaEngine() ;

    void init() ;

    void runScript(const char* filename) ;
    int registerLuaCallbackToRef(lua_State *L, int callbackIndex, int argsNum);
    int deregisterLuaCallbackToRef(lua_State* L, int ref);

    int callLuaRef(lua_State* L, int idx, int r=0);

    std::vector<std::string>& getTypeList() { return m_typeList; }

    void addType(std::shared_ptr<LuaType> t);
    void removeType(std::shared_ptr<LuaType> t);

    std::vector<std::string> m_typeList;
    lua_State* L = nullptr;

    int lua_createdRef = 0;
    int lua_destroyRef = 0;

    std::vector<std::shared_ptr<LuaType>> m_vec;

    std::shared_ptr<LuaInstance> m_inst = nullptr;

public:
    static int lua_registerCreated(lua_State* L);
    static int lua_registerBeforeDestroy(lua_State* L);
    static int lua_getTestInstance(lua_State* L);

};

class TestInst:public LuaInstance {
public:
    TestInst(LuaEngine *servicePtr);

    ~TestInst();

    static int lua_setTestStr(lua_State* L);
    static int lua_getTestStr(lua_State* L);

    static void register_lua_property_getters(lua_State* L);
    static void register_lua_property_setters(lua_State *L);

    DECLARE_CLASS(TestInst);
private:
    std::string m_testMemory;
};

#endif //LUA_CJSON_LuaEngine_LUA_ENGINE_H
