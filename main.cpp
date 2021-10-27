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
#include <thread>
#include "lua_engine.h"
#include "testmain.hpp"


void work() {
    {
        LuaEngine t;
        t.init();
        t.runScript("hello.lua");
    }

}

class TestStatic {

    static std::string a;
};

std::string TestStatic::a = "dsdsdsdsdsdsdsdasdadasdasdsadsadadadadaasdadadssdsdsd";


int main() {
    std::cout << "Hello, World!" << std::endl;
//    mainaa();

//    auto testa = std::make_shared<TestStatic>();
//
//    std::cout << "size sizeof(std::shared_ptr<LuaInstance>)" << sizeof(std::shared_ptr<LuaInstance>) << " sizeof sizeof(std::shared_ptr<TestInst>)" << sizeof(std::shared_ptr<TestInst>) << std::endl;
    for (int i = 0; i < 10; i++) {
//        work();
//        std::this_thread::sleep_for (std::chrono::milliseconds(1000));
        std::thread t2(work);
        t2.detach();
    }

    // wait thread finished
    std::this_thread::sleep_for(std::chrono::seconds(15));

    return 0;
}



