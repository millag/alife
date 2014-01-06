#include "Stork.h"

#include "Boid.h"
#include "Obstacle.h"
#include "utils.h"

// instantiate the static variable in RendererFactory
std::map<std::string, Stork::createCallback> Stork::m_storks;

void Stork::registerSpecies(const std::string &type, createCallback cb)
{
    m_storks[type] = cb;
}

void Stork::unregisterSpecies(const std::string &type)
{
    m_storks.erase(type);
}

SceneObject *Stork::deliverBaby(const std::string &type, int id)
{
    std::map<std::string, createCallback>::iterator it = m_storks.find(type);
    if (it == m_storks.end())
        return NULL;

    // call the creation callback to construct this derived type
    return (it->second)(id);
}
