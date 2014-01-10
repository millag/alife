#ifndef STORK_H
#define STORK_H

#include <string>
#include <map>
#include "BaseObjects.h"

//this is from Jon Macey design patterns examples pls cite appropriately
class RenderObjectFactory
{
public :
    /// The type for the callback that creates a SceneObject instance
    typedef RenderObject *(*createCallback)(const Mesh* _mesh);

    /// Add a new SceneObject Stork to the system
    static void sRegisterObject(const std::string &_type, createCallback _cb);

    /// Remove an existing SceneObject Stork from the system
    static void sUnregisterObject(const std::string &_type);

    /// Create an instance of a named SceneObject
    static RenderObject *sCreateObject(const std::string &_type, const Mesh* _mesh);

private :
    static std::map<std::string, createCallback> sObjects;
};

#endif // STORK_H
