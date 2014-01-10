#ifndef STORK_H
#define STORK_H

#include <string>
#include <map>
#include "geometry.h"

//this is from Jon Macey design patterns examples pls cite appropriately
class Stork
{
public :
    /// The type for the callback that creates a SceneObject instance
    typedef SceneObject *(*createCallback)(int id);

    /// Add a new SceneObject Stork to the system
    static void registerSpecies(const std::string &type, createCallback cb);

    /// Remove an existing SceneObject Stork from the system
    static void unregisterSpecies(const std::string &type);

    /// Create an instance of a named SceneObject
    static SceneObject *deliverBaby(const std::string &type, int id);

private :
    static std::map<std::string, createCallback> m_storks;
};

#endif // STORK_H
