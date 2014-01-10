#include "Factory.h"

#include "Boid.h"
#include "Obstacle.h"
#include "Utils.h"

// instantiate the static variable in SceneObjectFactory
std::map<std::string, RenderObjectFactory::createCallback> RenderObjectFactory::sObjects;

void RenderObjectFactory::sRegisterObject(const std::string &_type, createCallback _cb)
{
    sObjects[_type] = _cb;
}

void RenderObjectFactory::sUnregisterObject(const std::string &_type)
{
    sObjects.erase(_type);
}

RenderObject *RenderObjectFactory::sCreateObject(const std::string &_type, const Mesh *_mesh)
{
    std::map<std::string, createCallback>::iterator it = sObjects.find(_type);
    if (it == sObjects.end())
        return NULL;

    // call the creation callback to construct this derived type
    return (it->second)(_mesh);
}
