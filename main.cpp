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



void work() {
    {
        LuaEngine t;
        t.init();
        t.runScript("hello.lua");
    }

}



int main() {
    std::cout << "Hello, World!" << std::endl;
    for (int i = 0; i < 10; i++) {
//        work();
//        std::this_thread::sleep_for (std::chrono::milliseconds(1000));
        std::thread t2(work);
        t2.detach();
    }

    // wait thread finished
    std::this_thread::sleep_for(std::chrono::seconds(10));

    return 0;
}



