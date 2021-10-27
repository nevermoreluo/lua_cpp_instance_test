//
// Created by apowo on 2021/10/25.
//

#include "lua_class_factory.h"
#include "lua_engine.h"
#include "lua_instance.h"

std::map<std::string, ClassMetadata *> ClassFactory::metadataTable;
bool ClassFactory::isInitialized = false;


std::vector<std::string> ClassFactory::getRegisteredClasses()
{
    std::vector<std::string> regged;

    for (auto it = metadataTable.begin(); it != metadataTable.end(); ++it)
    {
        regged.push_back(it->first);
    }

    return regged;
}

void ClassFactory::addClass(const std::string& className, ClassMetadata *newClassMetadata)
{
    if (!newClassMetadata)
    {
        throw ("newClassMetadata cannot be NULL.");
    }
    metadataTable.insert(std::make_pair(className, newClassMetadata));
}

void ClassFactory::initClasses(LuaEngine* eng)
{
    for (auto it = metadataTable.begin(); it != metadataTable.end(); ++it)
    {
        if (it->second)
        {
            it->second->getInitFunc()(eng);
        }
    }
}

void ClassFactory::registerCoreClasses()
{
    if (isInitialized)
    {
        return;
    }
    isInitialized = true;


    // TODO handler event
//#if HAVE_ENET
//        Instance::NetworkReplicator::registerClass();
//        Instance::ClientReplicator::registerClass();
//        Instance::ServerReplicator::registerClass();
//        Instance::NetworkPeer::registerClass();
//        Instance::NetworkServer::registerClass();
//        Instance::NetworkClient::registerClass();
//        Instance::RemoteEvent::registerClass();
//#endif
}



