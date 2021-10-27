#include <lua.hpp>	//	LuaJIT header
#include <iostream>	//	Access to std::system("PAUSE")
#include <memory>

class MetaClass {

};


//	A simple class to be bound in our Lua environment.
class MyClass: public std::enable_shared_from_this<MyClass>
{
public:
    MyClass()
    {
        printf("Created New MyClass\n");
    }
    ~MyClass()
    {
        printf("Destroyed Existing MyClass\n");
    }

    void SomeFunction(const char* PassedString)
    {
        printf("MyClass: %s\n", PassedString);
    }

    int wrap_lua(lua_State* L) {
        std::shared_ptr <MyClass> shared_this = std::enable_shared_from_this<MyClass>::shared_from_this();


        void * ud = lua_newuserdata(L, sizeof(std::shared_ptr<MyClass>));
        new(ud) std::shared_ptr<MyClass>(shared_this);
//    lua_rawset(L, -3);

//    lua_pushstring(L, LUA_CPP_LUA_CLASS_KEY);
//    lua_pushstring(L, getLuaClassName().c_str());
//    lua_rawset(L, -3);

        luaL_getmetatable(L, "MyClass_Methods");
        lua_setmetatable(L, -2);
        return 1;
    }

    static MetaClass* _tq_classmetadata;
    static std::shared_ptr<MyClass> self;
};

MetaClass* MyClass::_tq_classmetadata=nullptr;
std::shared_ptr<MyClass> MyClass::self = nullptr;

//	Accessor function so the Lua Garbage Collector can properly free our resources.
static int MyClass_Destructor(lua_State *L)
{
    std::shared_ptr<MyClass>* Obj = static_cast<std::shared_ptr<MyClass>*>(lua_touserdata(L, -1));
    Obj->reset();
    return 0;
}

//	Accessor function to pull our class object out of the userdata
//	then use it to call the appropriate member function.
static int MyClass_SomeFunction(lua_State *L)
{
    if (lua_isuserdata(L, -1))
    {
        auto Obj = static_cast<std::shared_ptr<MyClass>*>(lua_touserdata(L, -1));
        Obj->get()->SomeFunction("Hello Poppet!");
    }
    else {
        printf("[Lua Error]: Tried to access non-userdata from the stack.");
    }
    return 0;
}

//	This function will register our MyClass metatable into the Lua environment
//	so we can later attach it to our userdata.
void RegisterMyClass(lua_State *L)
{
    static const luaL_Reg MyClass_Methods[] = {
            { "__gc",			MyClass_Destructor },
            { "SomeFunction",	MyClass_SomeFunction },
            { NULL,				NULL },
    };

    //	Create an empty metatable and push it onto the stack.
    //	NOTE: The name does NOT need to match the name of our static struct above
    //	this is only done for readability
    luaL_newmetatable(L, "MyClass_Methods");
    //	Duplicate the metatable so 2 now exist on the stack.
    lua_pushvalue(L, -1);
    /* Pop the first metatable off the stack and assign it to __index
    * of the second one. We set the metatable for the table to itself.
    * This is equivalent to the following in lua:
    * metatable = {}
    * metatable.__index = metatable
    */
    lua_setfield(L, -2, "__index");

    //	Add our methods into the metatable.
    luaL_setfuncs(L, MyClass_Methods, 0);

    //	Clear the stack
    //lua_remove(L, -1);
}

//	Here we have a function which instantiates userdata to represent our
//	MyClass object, sets the appropriate metatable, and returns it to the Lua environment.
static int MyClass_Create(lua_State *L)
{
    //	Push our userdata onto the stack.
    auto resource = MyClass::self;

    return resource->wrap_lua(L);
//    size_t ObjectSize = sizeof(std::shared_ptr<MyClass>);
//
//    void* Memory = lua_newuserdata(L, ObjectSize);
//    new(Memory) std::shared_ptr<MyClass>(resource);
//
//    //	Push our metatable onto the stack.
//    luaL_getmetatable(L, "MyClass_Methods");
//    //	Pop the metatable off the stack and set apply it to our userdata.
//    lua_setmetatable(L, -2);
//    //	Return our userdata back to the Lua environment.
//    return 1;
}

int mainaa()
{
    printf("Mark Startup Memory\n");

    MyClass::_tq_classmetadata = new MetaClass;
    //	Create the Lua State/Environment and load the core libraries.
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);

    //	Register our bound class.
    RegisterMyClass(L);
    //	Register the creation func.
    lua_pushcfunction(L, MyClass_Create);
    lua_setglobal(L, "NewMyClass");
    MyClass::self = std::make_shared<MyClass>();
    //	Run our supplied 'main.lua' file and report back any script errors.
    if (luaL_loadfile(L, "main.lua") || lua_pcall(L, 0, 0, 0))
    {
        printf("[Lua Error]: %s\n", lua_tostring(L, -1));
    }

    //	Finally close our State/Environment and pause the application.
    lua_close(L);

    MyClass::self = nullptr;

    printf("Mark Shutdown Memory\n");
    return 0;
}