#ifndef SCENE_H
#define SCENE_H

#include <string>
#include <vector>

#include <ngl/Light.h>
#include <ngl/Camera.h>

#include "geometry.h"
//QUESTION: what to do when there is cycle with include headers
//FIX: Scene interface needs to be break apart so different object have only part of its interface that concerns them
class Flock;

class Scene {
public:
    Scene();
    ~Scene();

    void initialize();
    void update(ngl::Real _deltaT);

    const ngl::Light* getLight() const;
    ngl::Camera *getCamera();
    const std::vector<SceneObject*>& getSceneObjectList() const;
    const std::vector<Mesh*>& getMeshList() const;

private:
    ngl::Light* m_light;
    ngl::Camera* m_camera;
    std::vector<Flock*> m_flocks;

    std::vector<SceneObject*> m_sceneObjects;
    std::vector<Mesh*> m_meshes;
};

#endif // SCENE_H
