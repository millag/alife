#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>
#include <string>
#include <map>

#include "ngl/Light.h"
#include "ngl/Camera.h"

#include "BaseObjects.h"
//QUESTION: what to do when there is cycle with include headers
//FIX: Scene interface needs to be break apart so different object have only part of its interface that concerns them
class Flock;

class Scene {
public:
    Scene();
    ~Scene();

    const AABB& getBoundingVolume() const;
    const std::vector<RenderObject*>& getRenderObjects() const;
    const std::vector<Mesh*>& getMeshes() const;

    void initialize();
    void update(ngl::Real _deltaT);
    void addBoids(unsigned _nCnt = 10, unsigned _flockId = 0);

private:
    std::vector<Flock*> m_flocks;
    AABB m_boundingVolume;

    std::vector<RenderObject*> m_renderObjects;
    std::vector<Mesh*> m_meshes;
    std::map<std::string, unsigned> m_meshMap;
};

#endif // SCENE_H
