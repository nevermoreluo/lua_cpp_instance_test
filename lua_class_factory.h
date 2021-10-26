//
// Created by apowo on 2021/10/25.
//

#ifndef LUA_CJSON_TEST_LUA_CLASS_FACTORY_H
#define LUA_CJSON_TEST_LUA_CLASS_FACTORY_H

#include <cstring>
#include <vector>
#include <map>
#include <memory>

class LuaEngine;
class LuaInstance;
using InstanceInitFnc = void (*)(LuaEngine *eng);

class ClassMetadata {
public:
    virtual std::shared_ptr<LuaInstance> newInstance(LuaEngine* eng) = 0;
    virtual std::string getClassName() = 0;
    virtual InstanceInitFnc getInitFunc() = 0;
};


class ClassFactory {
public:
    static std::vector<std::string> getRegisteredClasses();
    static void addClass(const std::string& className, ClassMetadata* newClassMetadata);
    static void initClasses(LuaEngine* eng);
    static void registerCoreClasses();

private:
    static std::map<std::string, ClassMetadata*> metadataTable;
    static bool isInitialized;
};


#endif //LUA_CJSON_TEST_LUA_CLASS_FACTORY_H
