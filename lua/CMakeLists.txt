set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)

add_library(lua SHARED
        ljumptab.h
        lopnames.h
#        ltests.c
#        onelua.c

        lapi.c
        lauxlib.c
        lbaselib.c
#        lbitlib.c
        lcode.c
        lcorolib.c
        lctype.c
        ldblib.c
        ldebug.c
        ldo.c
        ldump.c
        lfunc.c
        lgc.c
        linit.c
        liolib.c
        llex.c
        lmathlib.c
        lmem.c
        loadlib.c
        lobject.c
        lopcodes.c
        loslib.c
        lparser.c
        lstate.c
        lstring.c
        lstrlib.c
        ltable.c
        ltablib.c
        ltm.c
#        lua.c
#        luac.c
        lundump.c
        lutf8lib.c
        lvm.c
        lzio.c
        )
if (CMAKE_SYSTEM_NAME STREQUAL Linux)
    target_compile_definitions(lua PUBLIC LUA_USE_LINUX LUA_USE_LONGJMP)
    target_compile_options(lua PRIVATE -fPIC)
endif()
target_link_libraries(lua -Wl,-E -ldl)

set(LUA_LIBRARIES ${CMAKE_BINARY_DIR}/build/bin PARENT_SCOPE)
set(LUA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
