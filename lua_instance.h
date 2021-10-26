//
// Created by apowo on 2021/10/25.
//

#ifndef LUA_CJSON_TEST_LUA_INSTANCE_H
#define LUA_CJSON_TEST_LUA_INSTANCE_H

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

#include "lua_class_factory.h"

class LuaEngine;
class LuaInstance;
using luaRegisterFunc = void (*)(lua_State *L);


#define DECLARE_CLASS(Class_Name) \
    virtual std::string getClassName(); \
    virtual std::string getLuaClassName(); \
    static ClassMetadata* _tq_classmetadata; \
    static void registerClass(); \
    static void _tq_init(LuaEngine* eng);          \
protected: \
 static std::string ClassName; \
 static std::string LuaClassName

#define _DEFCLASS_SHARED(Class_Name) \
    std::string Class_Name::ClassName = "TQ" #Class_Name; \
    std::string Class_Name::LuaClassName = "luaL_Instance_TQ" #Class_Name; \
    std::string Class_Name::getClassName(){ \
        return ClassName; \
    } \
    std::string Class_Name::getLuaClassName(){ \
        return LuaClassName; \
    } \
    ClassMetadata* Class_Name::_tq_classmetadata = nullptr; \
    void Class_Name::registerClass(){ _tq_classmetadata = new Class_Name##_ClassMetadata; } \
    void Class_Name::_tq_init(LuaEngine* eng)

#define DEFINE_CLASS(Class_Name) \
    class Class_Name##_ClassMetadata: public ClassMetadata{ \
    public: \
    Class_Name##_ClassMetadata(){ \
        ClassFactory::addClass("TQ" #Class_Name, this); \
    } \
    virtual std::shared_ptr<LuaInstance> newInstance(LuaEngine* eng){ \
        return std::make_shared<Class_Name>(eng); \
    } \
    virtual std::string getClassName(){ \
        return "TQ" #Class_Name; \
    } \
    virtual InstanceInitFnc getInitFunc(){ \
        return Class_Name::_tq_init; \
    } \
    }; \
    _DEFCLASS_SHARED(Class_Name)


#define _DEFCLASS_SHARED_ABS(Class_Name) \
    std::string Class_Name::ClassName = "TQ" #Class_Name; \
    std::string Class_Name::LuaClassName = "luaL_Instance_TQ" #Class_Name; \
    std::string Class_Name::getClassName(){ \
        return ClassName; \
    } \
    std::string Class_Name::getLuaClassName(){ \
        return LuaClassName; \
    } \
    ClassMetadata* Class_Name::_tq_classmetadata = nullptr; \
    void Class_Name::registerClass(){ _tq_classmetadata = new Class_Name##_ClassMetadata; } \
    void Class_Name::_tq_init(LuaEngine* eng)

#define DEFINE_CLASS_ABS(Class_Name) \
    class Class_Name##_ClassMetadata: public ClassMetadata{ \
    public: \
    Class_Name##_ClassMetadata(){ \
    } \
    virtual std::shared_ptr<LuaInstance> newInstance(LuaEngine* eng){ \
        return nullptr; \
    } \
    virtual std::string getClassName(){ \
        return "TQ" #Class_Name; \
    } \
    virtual InstanceInitFnc getInitFunc(){ \
        return Class_Name::_tq_init; \
    } \
    }; \
    _DEFCLASS_SHARED_ABS(Class_Name)


class LuaInstance: public std::enable_shared_from_this<LuaInstance> {
public:
    explicit LuaInstance(LuaEngine* eng);

    virtual ~LuaInstance();

    virtual int getId() { return 0; }

    virtual int getInstanceType() const { return 0; }

    static void registerLuaClass(LuaEngine* eng, std::string className, luaRegisterFunc register_metamethods,
                                 luaRegisterFunc register_methods,
                                 luaRegisterFunc register_getters,
                                 luaRegisterFunc register_setters,
                                 luaRegisterFunc register_events);


    virtual std::string toString();

    static std::shared_ptr<LuaInstance> checkInstance(lua_State *L, int index, bool errIfNot = true, bool allowNil = true);

    static int lua_index(lua_State *L);

    static int lua_newindex(lua_State *L);
    static int lua_eq(lua_State *L);
    static int lua_gc(lua_State *L);
    static int lua_toString(lua_State *L);
    static void register_lua_metamethods(lua_State *L);
    static void register_lua_methods(lua_State *L);
    static void register_lua_property_setters(lua_State *L);
    static void register_lua_property_getters(lua_State *L);
    static void register_lua_events(lua_State *L);

    int wrap_lua(lua_State *L);
    virtual void clearClassMetadata() { _tq_classmetadata = nullptr;}

    std::string className;
    LuaEngine* eng = nullptr;


    virtual std::string getClassName();
    virtual std::string getLuaClassName();
    static ClassMetadata* _tq_classmetadata;
    static void registerClass();
    static void _tq_init(LuaEngine* eng);
    static std::string ClassName;
    static std::string LuaClassName;

};


#endif //LUA_CJSON_TEST_LUA_INSTANCE_H
