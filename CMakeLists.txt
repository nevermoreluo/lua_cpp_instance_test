cmake_minimum_required(VERSION 3.10)
project(lua_cjson_test)

set(CMAKE_CXX_STANDARD 11)

include_directories(./lua)
include_directories(./lua-cjson)


add_subdirectory(lua build/lua.out)
add_subdirectory(lua-cjson build/lua-cjson.out)



add_executable(lua_cjson_test main.cpp lua_type.cpp lua_type.h lua_engine.cpp lua_engine.h lua_instance.cpp lua_instance.h lua_class_factory.cpp lua_class_factory.h testmain.hpp)

target_link_libraries(lua_cjson_test lua -lpthread -lm -lz)
