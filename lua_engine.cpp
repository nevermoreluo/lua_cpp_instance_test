//
// Created by apowo on 2021/10/22.
//

#include "lua_engine.h"
#include "lua_type.h"
#include <algorithm>

int addLuaCPath(lua_State* m_pLuaState, const char* path)
{
    lua_getglobal( m_pLuaState, "package" );
    lua_getfield( m_pLuaState, -1, "cpath" ); // get field "path" from table at top of stack (-1)
    std::string cur_path = lua_tostring( m_pLuaState, -1 ); // grab path string from top of stack
    cur_path.append( ";" ); // do your path magic here
    cur_path.append( path );
    lua_pop( m_pLuaState, 1 ); // get rid of the string on the stack we just pushed on line 5
    lua_pushstring( m_pLuaState, cur_path.c_str() ); // push the new one
    lua_setfield( m_pLuaState, -2, "cpath" ); // set the field "path" in table at -2 with value at top of stack
    lua_pop( m_pLuaState, 1 ); // get rid of package table from top of stack
    return 0; // all done!
}


class TestInst_ClassMetadata: public ClassMetadata{
public:
    TestInst_ClassMetadata(){
        ClassFactory::addClass("TQTestInst", this);
    }
    virtual std::shared_ptr<LuaInstance> newInstance(LuaEngine* eng){
        return std::make_shared<TestInst>(eng);
    }
    virtual std::string getClassName(){
        return "TQTestInst";
    }
    virtual InstanceInitFnc getInitFunc(){
        return TestInst::_tq_init;
    }
};
std::string TestInst::ClassName = "TQTestInst" ;
std::string TestInst::LuaClassName = "luaL_Instance_TQTestInst" ;
std::string TestInst::getClassName(){
return ClassName;
}
std::string TestInst::getLuaClassName(){
    return LuaClassName;
}

void TestInst::_tq_init(LuaEngine* eng) {
    registerLuaClass(eng, LuaClassName, register_lua_metamethods, register_lua_methods, register_lua_property_getters,
         register_lua_property_setters, register_lua_events
 );
}

//DEFINE_CLASS(TestInst)
//{
//registerLuaClass(eng, LuaClassName, register_lua_metamethods, register_lua_methods, register_lua_property_getters,
//        register_lua_property_setters, register_lua_events
//);
//}

TestInst::TestInst(LuaEngine *servicePtr) :LuaInstance(servicePtr)
{

}

TestInst::~TestInst()
{
    printf("destroy test inst\n");
}


int TestInst::lua_getTestStr(lua_State* L) {
    auto inst = checkInstance(L, 1);
    std::shared_ptr<TestInst> ti = std::dynamic_pointer_cast<TestInst>(inst);
    if (nullptr == ti) {
        return luaL_error(L, "bad args #1 expect TestInst");
    }
    lua_pushstring(L, ti->m_testMemory.c_str());
    return 1;
}

int TestInst::lua_setTestStr(lua_State* L)
{
    auto inst = checkInstance(L, 1);
    std::shared_ptr<TestInst> ti = std::dynamic_pointer_cast<TestInst>(inst);
    if (nullptr == ti) {
        return luaL_error(L, "bad args #1 expect TestInst");
    }
    std::string str = luaL_checkstring(L, 2);

    ti->m_testMemory = str;
    return 0;
}


void TestInst::register_lua_property_getters(lua_State* L)
{
    LuaInstance::register_lua_property_getters(L);
    luaL_Reg properties[] = {
            {"TestStr", lua_getTestStr},
            {nullptr,         nullptr}
    };
    luaL_setfuncs(L, properties, 0);
}

void TestInst::register_lua_property_setters(lua_State *L)
{
    LuaInstance::register_lua_property_setters(L);
    luaL_Reg properties[] = {
            {"TestStr", lua_setTestStr},
            {nullptr,         nullptr}
    };
    luaL_setfuncs(L, properties, 0);
}



LuaEngine::LuaEngine() {
    L = luaL_newstate();
    luaL_openlibs(L);
}
LuaEngine::~LuaEngine() {
    if (nullptr == L)
        return;
    callLuaRef(L, lua_destroyRef);

    deregisterLuaCallbackToRef(L, lua_createdRef);
    deregisterLuaCallbackToRef(L, lua_destroyRef);
    lua_close(L);
    m_inst = nullptr;
    m_vec.clear();
//    L->userdata = nullptr;
}

void LuaEngine::init() {
    ClassFactory::registerCoreClasses();
    ClassFactory::initClasses(this);
    LuaType::_tq_init(this);
    L->userdata = this;
    addLuaCPath(L, "./?.so;");


    m_inst = std::make_shared<TestInst>(this);

    lua_newtable(L);
    luaL_setmetatable(L, LuaType::LuaTypeName.c_str());
    lua_setglobal(L, "LuaType");

    luaL_Reg cLib [] = {
            {"RegisterCreated", lua_registerCreated},
            {"RegisterBeforeDestroy", lua_registerBeforeDestroy },
            {"GetTestInstance", lua_getTestInstance},
            {NULL, NULL} /* end of array */
    };

    lua_getglobal(L, "_G");
    luaL_setfuncs(L, cLib, 0);
    lua_pop(L, 1);

    chdir("../lua_scripts/");


}

int LuaEngine::lua_getTestInstance(lua_State* L) {
    LuaEngine* e = (LuaEngine*)L->userdata;
    e->m_inst->wrap_lua(L);
    return 1;
}


int LuaEngine::lua_registerCreated(lua_State* L)
{
    LuaEngine* e = (LuaEngine*)L->userdata;
    e->lua_createdRef = e->registerLuaCallbackToRef(L, 1, 0);
    return 0;
}

int LuaEngine::lua_registerBeforeDestroy(lua_State* L)
{
    LuaEngine* e = (LuaEngine*)L->userdata;
    e->lua_destroyRef = e->registerLuaCallbackToRef(L, 1, 0);
    e->lua_destroyRef = e->registerLuaCallbackToRef(L, 1, 0);
    return 0;
}

int LuaEngine::callLuaRef(lua_State* L, int idx, int r)
{
    if (nullptr != L && idx > 0)
    {
        int table_idx = lua_gettop(L); // should be 1
        if (table_idx > 0)
        {
            lua_pop(L, table_idx);
        }
        lua_rawgeti(L, LUA_REGISTRYINDEX, idx);
        if (lua_pcall(L, 0, r, 0) != 0)
        {
            /** error message on the stack top, lua_error will jump */
            if (lua_isstring(L, 1))
            {
                std::string errorMsg = lua_tostring(L, 1);
                printf("%s", errorMsg.c_str());
            }
        } else {
            return 1;
        }
//        lua_pushnil(L); // need nil element to call lua_next for default key
//        while (lua_next(L, -2))
//        {
//            lua_insert(L, -3);
//        }
//        lua_pop(L, 1);
//        int args_num = lua_gettop(L) - 1;
//        if (lua_isfunction(L, 1))
//        {
//            if (lua_pcall(L, args_num, r, 0) != 0)
//            {
//                /** error message on the stack top, lua_error will jump */
//                if (lua_isstring(L, 1))
//                {
//                    std::string errorMsg = lua_tostring(L, 1);
//                    printf("%s", errorMsg.c_str());
//                }
//            } else {
//                return 1;
//            }
//        } else
//        {
//            printf("No function to call");
//            return 0;
//        }
    } else
    {
        printf("Can not Call lua ref idx, wrong ref idx: %d", idx);
        return 0;
    }
    return 0;
}

int LuaEngine::registerLuaCallbackToRef(lua_State *L, int callbackIndex, int argsNum)
{
    int lua_top = lua_gettop(L);
    if (lua_isfunction(L, callbackIndex))
    {
        int maxArgsNum = lua_top - callbackIndex;
        if(maxArgsNum > 0 && (argsNum < 0 || argsNum >= maxArgsNum))
        {
            argsNum = maxArgsNum;
        }
        if (argsNum < 0)
            argsNum = 0;
//        lua_newtable(L);
//        for (int i = 0; i < argsNum + 1; i++)
//        {
//            lua_pushinteger(L,i + 1);
//            lua_pushvalue(L,i + callbackIndex);
//            lua_rawset(L, -3);
//        }
        lua_pushvalue(L, callbackIndex);
        int ref = luaL_ref(L, LUA_REGISTRYINDEX);
        return ref;
    }
    return -1;
}

int LuaEngine::deregisterLuaCallbackToRef(lua_State* L, int ref)
{
    if (ref > 0 && nullptr != L)
    {
        luaL_unref(L, LUA_REGISTRYINDEX, ref);
    }
    return 0;
}

void LuaEngine::runScript(const char* filename) {
    if(luaL_dofile(L, "hello.lua")) {
        std::cout << "Final:" << lua_tostring(L, -1) << "\n";
    }
    callLuaRef(L, lua_createdRef);
}


void LuaEngine::addType(std::shared_ptr<LuaType> t)
{
    m_vec.push_back(t);
}
void LuaEngine::removeType(std::shared_ptr<LuaType> t)
{
    if (t == nullptr) {
        return;
    }
    m_vec.erase(std::remove_if(m_vec.begin(), m_vec.end(), [t](std::shared_ptr<LuaType> x) { return t == x; }));
}

