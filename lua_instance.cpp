//
// Created by apowo on 2021/10/25.
//

#include <cstring>
#include "lua_instance.h"
#include "lua_engine.h"
#ifndef  LUA_CPP_USER_DATA_KEY
#define LUA_CPP_USER_DATA_KEY "_cpp_user_data"
#endif

#ifndef LUA_CPP_LUA_CLASS_KEY
#define LUA_CPP_LUA_CLASS_KEY "_cpp_lua_type"
#endif

class LuaInstance_ClassMetadata: public ClassMetadata{
public:
    LuaInstance_ClassMetadata(){
}
virtual std::shared_ptr<LuaInstance> newInstance(LuaEngine* eng){
return nullptr;
}
virtual std::string getClassName(){
        return "TQLuaInstance" ;
}
virtual InstanceInitFnc getInitFunc(){
        return LuaInstance::_tq_init;
}
};
std::string LuaInstance::ClassName = "TQLuaInstance" ;
std::string LuaInstance::LuaClassName = "luaL_Instance_TQLuaInstance" ;
std::string LuaInstance::getClassName(){
return ClassName;
}
std::string LuaInstance::getLuaClassName(){
return LuaClassName;
}
void LuaInstance::_tq_init(LuaEngine* eng) {
registerLuaClass(eng, LuaClassName,
                 register_lua_metamethods,
                 register_lua_methods,
                 register_lua_property_getters,
                 register_lua_property_setters,
                 register_lua_events
                 );
}

//DEFINE_CLASS_ABS(LuaInstance)
//{
//    registerLuaClass(eng, LuaClassName,
//                     register_lua_metamethods,
//                     register_lua_methods,
//                     register_lua_property_getters,
//                     register_lua_property_setters,
//                     register_lua_events
//    );
//}


LuaInstance::LuaInstance(LuaEngine* _eng)
{
    eng = _eng;
}

LuaInstance::~LuaInstance()
{
}


void LuaInstance::registerLuaClass(LuaEngine* eng, std::string className, luaRegisterFunc register_metamethods,
                             luaRegisterFunc register_methods,
                             luaRegisterFunc register_getters,
                             luaRegisterFunc register_setters,
                             luaRegisterFunc register_events)
{
    lua_State *L = eng->L;

    luaL_newmetatable(L, className.c_str());
    register_metamethods(L);

    // Name
    lua_pushstring(L, "__name");
    /*
      For now, I think all Instance classes being called
      'Instance' is for the best, in this context. I say this
      only because we typecheck against 'Instance' in Lua.
    */
    lua_pushstring(L, "LuaInstance");
    lua_rawset(L, -3);

    // Methods
    lua_pushstring(L, "__methods");
    lua_newtable(L);
    register_methods(L);
    lua_rawset(L, -3);

    // Property getters
    lua_pushstring(L, "__propertygetters");
    lua_newtable(L);
    register_getters(L);
    lua_rawset(L, -3);

    // Property setters
    lua_pushstring(L, "__propertysetters");
    lua_newtable(L);
    register_setters(L);
    lua_rawset(L, -3);

    // Events
    lua_pushstring(L, "__events");
    lua_newtable(L);
    register_events(L);
    lua_rawset(L, -3);

    // Item get
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, lua_index);
    lua_rawset(L, -3);

    // Item set
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, lua_newindex);
    lua_rawset(L, -3);



    lua_pop(L, 1);
}


std::string LuaInstance::toString()
{
    return getClassName();
}

std::shared_ptr<LuaInstance> LuaInstance::checkInstance(lua_State *L, int index, bool errIfNot, bool allowNil)
{
    if (nullptr == L)
    {
        return nullptr;
    }
    if (allowNil)
    {
        if (lua_isnoneornil(L, index))
        {
            return nullptr;
        }
    }

    if (lua_isuserdata(L, index))
    {
//        std::vector <std::string> existing = ClassFactory::getRegisteredClasses();
//        unsigned size = existing.size();
        void *udata = lua_touserdata(L, lua_gettop(L));
        return (*static_cast<std::shared_ptr <LuaInstance> *>(udata));
//        int meta = lua_getmetatable(L, index);
//        if (meta != 0)
//        {
//            for (unsigned i = 0; i < size; i++)
//            {
//                std::string name = "luaL_Instance_" + existing[i];
//                luaL_getmetatable(L, name.c_str());
//                if (lua_rawequal(L, -1, -2))
//                {
//                    lua_pop(L, 3);
//                    return (*static_cast<std::shared_ptr <LuaInstance> *>(udata));
//                }
//                lua_pop(L, 1);
//            }
//        }
    }


//    if (lua_istable(L, index))
//    {
//        lua_pushstring(L, LUA_CPP_USER_DATA_KEY);
//        lua_rawget(L, index);
//        if (lua_isuserdata(L, lua_gettop(L)))
//        {
//            std::vector <std::string> existing = ClassFactory::getRegisteredClasses();
//            unsigned size = existing.size();
//            void *udata = lua_touserdata(L, lua_gettop(L));
//            int meta = lua_getmetatable(L, index);
//            if (meta != 0)
//            {
//                for (unsigned i = 0; i < size; i++)
//                {
//                    std::string name = "luaL_Instance_" + existing[i];
//                    luaL_getmetatable(L, name.c_str());
//                    if (lua_rawequal(L, -1, -2))
//                    {
//                        lua_pop(L, 3);
//                        return (*static_cast<std::shared_ptr <LuaInstance> *>(udata));
//                    }
//                    lua_pop(L, 1);
//                }
//            }
//        }
//        // pop userdata and index string
//        lua_pop(L, 2);
//    }
    return nullptr;
}

int LuaInstance::lua_index(lua_State *L){
    std::shared_ptr <LuaInstance> inst = checkInstance(L, 1, false);
    const char *name = luaL_checkstring(L, 2);
    if (!inst && std::strcmp(name, "IsNil") != 0)
    {
        return luaL_error(L, "attempt to index '%s' (a nil value)", name);
    }
    lua_getmetatable(L, 1);// -3
    lua_getfield(L, -1, "__propertygetters");// -2
    lua_getfield(L, -1, name);// -1
    if (lua_iscfunction(L, -1))
    {
        lua_remove(L, -2);
        lua_remove(L, -2);

        lua_pushvalue(L, 1);
        lua_call(L, 1, 1);
        return 1;
    } else
    {
        lua_pop(L, 2);
        // Check methods
        lua_getfield(L, -1, "__methods");// -2
        lua_getfield(L, -1, name);// -1
        if (lua_iscfunction(L, -1))
        {
            lua_remove(L, -2);
            lua_remove(L, -3);

            return 1;
        } else
        {
            lua_pop(L, 2);
            // Check events
            lua_getfield(L, -1, "__events");// -2
            lua_getfield(L, -1, name);// -1
            if (lua_iscfunction(L, -1))
            {
                lua_remove(L, -2);
                lua_remove(L, -3);

                lua_pushvalue(L, 1);
                lua_call(L, 1, 1);
                return 1;
            } else
            {
                lua_pop(L, 3);
                lua_pushstring(L, name);
                lua_remove(L, -1);
                lua_rawget(L, 1);
                return 1;

//                    lua_pop(L, 3);
//                    std::shared_ptr <LuaInstance> kiddie = inst->FindFirstChild(name, false);
//                    if (kiddie)
//                    {
//                        return kiddie->wrap_lua(L);
//                    }

                return luaL_error(L, "attempt to index '%s' (a nil value)", name);
            }
        }
    }

    return 0;
}

int LuaInstance::lua_newindex(lua_State *L){
    std::shared_ptr <LuaInstance> inst = checkInstance(L, 1, false);
    if (!inst)
    {
        return 0;
    }

    const char *name = luaL_checkstring(L, 2);
    if (strcmp(name, LUA_CPP_USER_DATA_KEY) == 0 || strcmp(name, LUA_CPP_LUA_CLASS_KEY) == 0)
    {
        return luaL_error(L, "This key has been locked.");
    }

    lua_getmetatable(L, 1);// -3
    lua_getfield(L, -1, "__propertysetters");// -2
    lua_getfield(L, -1, name);// -1
    if (lua_iscfunction(L, -1))
    {
        lua_remove(L, -2);
        lua_remove(L, -2);

        lua_pushvalue(L, 1);
        lua_pushvalue(L, 3);
        lua_call(L, 2, 0);

        return 0;
    } else
    {
        lua_pop(L, 3);
        lua_rawset(L, -3);
        return 0;
    }
    luaL_error(L, "attempt to index '%s' (a nil value)", name);
    return 0;
}

int LuaInstance::lua_eq(lua_State *L){
    std::shared_ptr <LuaInstance> inst = checkInstance(L, 1, false);

    if (inst)
    {
        std::shared_ptr <LuaInstance> oinst = checkInstance(L, 2, false);
        if (oinst)
        {
            lua_pushboolean(L, inst == oinst);
            return 1;
        }
    } else if (lua_isnil(L, 2))
    {
        lua_pushboolean(L, true);
        return 1;
    } else if (lua_isnil(L, 1) && !checkInstance(L, 2, false))
    {
        lua_pushboolean(L, true);
        return 1;
    }
    lua_pushboolean(L, false);
    return 1;
}

int LuaInstance::lua_toString(lua_State *L){
    std::shared_ptr <LuaInstance> inst = checkInstance(L, 1, false);

    if (inst)
    {
        lua_pushstring(L, inst->toString().c_str());
    }

    return 1;
}

void LuaInstance::register_lua_metamethods(lua_State *L){
    luaL_Reg metamethods[] = {
            {"__tostring", LuaInstance::lua_toString},
            {"__eq",       lua_eq},
            {"__gc",       lua_gc},
            {NULL,         NULL}
    };
    luaL_setfuncs(L, metamethods, 0);
}

void LuaInstance::register_lua_methods(lua_State *L){
    luaL_Reg methods[] = {
//            {"GetFullName",               lua_getFullName},
            {NULL,                        NULL}
    };
    luaL_setfuncs(L, methods, 0);
}

void LuaInstance::register_lua_property_setters(lua_State *L){
    luaL_Reg properties[] = {
//            {"ClassName",  lua_readOnlyProperty},
//            {"UseCount",   lua_readOnlyProperty},
////            {"Name",       lua_setName},
//            {"Parent",     lua_setParent},
//            {"Archivable", lua_setArchivable},
            {NULL,         NULL}
    };
    luaL_setfuncs(L, properties, 0);
}

void LuaInstance::register_lua_property_getters(lua_State *L){
    luaL_Reg properties[] = {
            {NULL,         NULL}
    };
    luaL_setfuncs(L, properties, 0);
}

void LuaInstance::register_lua_events(lua_State *L){
    luaL_Reg events[] = {
            {NULL,                 NULL}
    };
    luaL_setfuncs(L, events, 0);
}


int LuaInstance::wrap_lua(lua_State *L){
//    lua_newtable(L);
//
//    lua_pushstring(L, LUA_CPP_USER_DATA_KEY);
    std::shared_ptr <LuaInstance> shared_this = std::enable_shared_from_this<LuaInstance>::shared_from_this();

//    void* ud = lua_newuserdata(L, sizeof(std::shared_ptr <
//            LuaInstance > ));
//    std::shared_ptr <LuaInstance> *udata = static_cast<std::shared_ptr <LuaInstance> *>(ud);
//    *static_cast<std::shared_ptr <LuaInstance>**>(ud) = udata;
//    new(udata) std::shared_ptr<LuaInstance>(shared_this);

    void * ud = lua_newuserdata(L, sizeof(std::shared_ptr<LuaInstance>));
    new(ud) std::shared_ptr<LuaInstance>(shared_this);
//    lua_rawset(L, -3);

//    lua_pushstring(L, LUA_CPP_LUA_CLASS_KEY);
//    lua_pushstring(L, getLuaClassName().c_str());
//    lua_rawset(L, -3);

    luaL_getmetatable(L, getLuaClassName().c_str());
    lua_setmetatable(L, -2);
    return 1;
}

int LuaInstance::lua_gc(lua_State *L){
    // reset??
    auto inst = checkInstance(L, 1);
    if (nullptr != inst) {
        inst.reset();
    }
//    lua_remove(L, 1);
    return 0;
}