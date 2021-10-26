//
// Created by apowo on 2021/10/22.
//

#include "lua_type.h"
#include "lua_engine.h"

#define LUA_CPP_LUA_CLASS_KEY "_cpp_lua_type"
#define LUA_CPP_USER_DATA_KEY "_cpp_user_data"

std::string LuaType::TypeName = "LuaType";
std::string LuaType::LuaTypeName = "luaL_Type_LuaType";
std::string LuaType::getClassName(){
    return TypeName;
}
std::string LuaType::getLuaClassName(){
    return LuaTypeName;
}

void LuaType::_tq_init(LuaEngine* eng)
{
    registerLuaType(eng, LuaTypeName, TypeName, register_lua_metamethods, register_lua_methods,
                    register_lua_property_getters, register_lua_property_setters);
}

LuaType::LuaType()
{

}

LuaType::~LuaType()
{
    printf("destroy call ~luatype\n");
}


void LuaType::registerLuaType(LuaEngine* eng, std::string typeName, std::string className,
                           luaRegisterFunc register_metamethods,
                           luaRegisterFunc register_methods,
                           luaRegisterFunc register_getters,
                           luaRegisterFunc register_setters)
{
    eng->getTypeList().push_back(typeName);

    lua_State* L = eng->L;

    luaL_newmetatable(L, typeName.c_str());
    register_metamethods(L);

//            lua_pushstring(L, "__metatable");
//            lua_pushstring(L, "This metatable is locked");
//            lua_rawset(L, -3);

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

    // Item get
    lua_pushstring(L, "__index");
    lua_pushcfunction(L, lua_index);
    lua_rawset(L, -3);

    // Item set
    lua_pushstring(L, "__newindex");
    lua_pushcfunction(L, lua_newindex);
    lua_rawset(L, -3);

    lua_pushstring(L, "__pairs");
    lua_pushcfunction(L, lua_pairs);
    lua_rawset(L, -3);

    // Set name
    lua_pushstring(L, "__name");
    lua_pushstring(L, className.c_str());
    lua_rawset(L, -3);

    lua_pop(L, 1);
}


int LuaType::wrap_lua(lua_State* L)
{
    lua_newtable(L);
    lua_pushstring(L, LUA_CPP_USER_DATA_KEY);
    std::shared_ptr <LuaType> shared_this = std::enable_shared_from_this<LuaType>::shared_from_this();

    std::weak_ptr <LuaType> *udata = static_cast<std::weak_ptr <LuaType> *>(lua_newuserdata(L, sizeof(std::weak_ptr <
            LuaType > )));
    new(udata) std::weak_ptr<LuaType>(shared_this);
    lua_rawset(L, -3);

    lua_pushstring(L, LUA_CPP_LUA_CLASS_KEY);
    lua_pushstring(L, getLuaClassName().c_str());
    lua_rawset(L, -3);

    luaL_getmetatable(L, getLuaClassName().c_str());
    lua_setmetatable(L, -2);
    return 1;
}

std::string LuaType::toString()
{
    char buff[100];
    snprintf(buff, sizeof(buff), "%s<%p>", TypeName.c_str(), this);
    std::string buffAsStdStr = buff;
    return buffAsStdStr;
}

void LuaType::register_lua_metamethods(lua_State *L)
{
    luaL_Reg metamethods[] = {
            {"__tostring", lua_toString},
            {"__eq",       lua_eq},
            {"__gc",       lua_gc},
            {NULL, NULL}
    };
    luaL_setfuncs(L, metamethods, 0);
}

void LuaType::register_lua_methods(lua_State *L)
{
    luaL_Reg methods[] = {
            {"New", lua__new},
            {NULL, NULL}
    };
    luaL_setfuncs(L, methods, 0);
}

void LuaType::register_lua_property_setters(lua_State *L)
{
    luaL_Reg properties[] = {
            {NULL, NULL}
    };
    luaL_setfuncs(L, properties, 0);
}

void LuaType::register_lua_property_getters(lua_State *L)
{
    luaL_Reg properties[] = {
            {NULL, NULL}
    };
    luaL_setfuncs(L, properties, 0);
}

std::shared_ptr<LuaType> LuaType::checkType(lua_State *L, int index, bool errIfNot, bool allowNil)
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
    LuaEngine* eng = (LuaEngine*)L->userdata;
    auto typeList = eng->getTypeList();

    if (lua_istable(L, index))
    {
        lua_pushstring(L, LUA_CPP_LUA_CLASS_KEY);
        lua_rawget(L, index);
        if (lua_isstring(L, lua_gettop(L)))
        {
            std::string lClassKey = lua_tostring(L, lua_gettop(L));
            lua_pop(L, 1);
            if (lClassKey.rfind("luaL_Type_", 0) == 0)
            {
                lua_pushstring(L, LUA_CPP_USER_DATA_KEY);
                lua_rawget(L, index);
                if (lua_isuserdata(L, lua_gettop(L)))
                {
                    unsigned size = typeList.size();
                    void *udata = lua_touserdata(L, lua_gettop(L));
                    int meta = lua_getmetatable(L, index);
                    if (meta != 0 && nullptr != udata)
                    {
                        lua_pop(L, 2);
                        return (*static_cast<std::weak_ptr<LuaType> *>(udata)).lock();
                    }
                }
                lua_pop(L, 2);
            }
        } else {
            lua_pop(L, 1);
        }
    }
    return NULL;
}

int LuaType::lua_pairs(lua_State* L)
{
    std::shared_ptr<LuaType> t = checkType(L, 1, false);
    if (t)
    {
        lua_getglobal(L, "next");
        lua_pushstring(L, "_cpp_table");
        lua_rawget(L, 1);
        if (!lua_istable(L, -1))
        {
            lua_newtable(L);
        }
        lua_pushnil(L);
    }
    return 3;
}


int LuaType::lua__new(lua_State* L)
{
    auto newGuy = std::make_shared<LuaType>();
    LuaEngine* Vs = (LuaEngine*)L->userdata;
    return newGuy->wrap_lua(L);
}


int LuaType::lua_newindex(lua_State *L)
{
    std::shared_ptr<LuaType> t = checkType(L, 1, false);
    if (t)
    {
        const std::string name = luaL_checkstring(L, 2);
        lua_getmetatable(L, 1);//-3
        lua_getfield(L, -1, "__propertysetters");//-2
        lua_getfield(L, -1, name.c_str());//-1
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

            lua_pushstring(L, "_cpp_table");
            lua_rawget(L, 1);
            bool newT = !lua_istable(L, -1);
            if (newT)
            {
                lua_pop(L, 1);
                lua_pushstring(L, "_cpp_table");
                lua_newtable(L);
            }
            lua_pushnil(L);
            lua_copy(L, 2, -1);
            lua_rawget(L, -2);
            if (lua_compare(L, -1, 3, LUA_OPEQ) == 1)
            {
                // skip same sth
                return 0;
            }
            lua_pushnil(L);
            lua_copy(L, 2, -2);
            lua_copy(L, 3, -1);
            lua_rawset(L, -3);
            if (newT)
            {
                lua_rawset(L, 1);
            }
            return 0;
//                    return luaL_error(L, "attempt to new index '%s' (a nil value)", name);
        }
    } else {
        printf("can not find lua type\n");
    }
    return 0;
}

int LuaType::lua_index(lua_State *L)
{
    int index = 1;

    LuaEngine* eng = (LuaEngine*)L->userdata;
    auto typeList = eng->getTypeList();

    std::shared_ptr<LuaType> t = checkType(L, index, false);
    if (!t)
    {
        bool luaType = false;
        if (lua_istable(L, index))
        {
            unsigned size = typeList.size();
            int meta = lua_getmetatable(L, index);
            if (meta != 0)
            {
                for (unsigned i = 0; i < size; i++)
                {
                    luaL_getmetatable(L, typeList[i].c_str());
                    if (lua_rawequal(L, -1, -2))
                    {
                        lua_pop(L, 1);
                        luaType = true;
                        break;
                    } else {
                        lua_pop(L, 1);
                    }
                }
            }
        }
        if (!luaType)
        {
            return 0;
        }
    } else {
        lua_getmetatable(L, 1);//-3
    }

    const char *name = luaL_checkstring(L, 2);
    lua_getfield(L, -1, "__propertygetters");//-2
    lua_getfield(L, -1, name);//-1
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
        lua_getfield(L, -1, "__methods");//-2
        lua_getfield(L, -1, name);//-1
        if (lua_iscfunction(L, -1))
        {
            lua_remove(L, -2);
            lua_remove(L, -3);

            return 1;
        } else
        {
//                    if (!t)
//                    {
//                        // TODO try to get __className in lua
//                        return luaL_error(L, "Can not call __index by empty typeInstance, please check instance, %s", __FUNCTION__ );
//                    }
            lua_pop(L, 3);

            lua_getfield(L, 1, "_cpp_table");
            if (lua_istable(L, -1))
            {
                lua_pushstring(L, name);
                lua_rawget(L, -2);
            } else {
                lua_pushnil(L);
            }
            return 1;
//                    return luaL_error(L, "attempt to index '%s' (a nil value)", name);
//                    return 0;
        }
    }
}

int LuaType::lua_eq(lua_State *L)
{
    std::shared_ptr<LuaType> t = checkType(L, 1, false);
    if (t)
    {
        std::shared_ptr<LuaType> ot = checkType(L, 2, false);
        if (ot)
        {
            lua_pushboolean(L, t == ot);
            return 1;
        }
    }

    lua_pushboolean(L, false);
    return 1;
}

int LuaType::lua_gc(lua_State *L)
{
    std::shared_ptr<LuaType> t = checkType(L, 1, false);
    LuaEngine* eng = (LuaEngine*)L->userdata;
    eng->removeType(t);
    lua_remove(L, 1);
    return 0;
}

int LuaType::lua_toString(lua_State *L)
{
    std::shared_ptr<LuaType> t = checkType(L, 1, false);
    if (!t)
    {
        lua_pushstring(L, "Type");
        return 1;
    }

    lua_pushstring(L, t->toString().c_str());
    return 1;
}