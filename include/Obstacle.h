#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "geometry.h"
#include "utils.h"

class Obstacle : public SceneObject
{
public:
    Obstacle(int _meshId = -1, const ngl::Transformation& _transform = ngl::Transformation()):
        SceneObject(_meshId, _transform)
    { }

    Obstacle(const Obstacle& _obstacle):
        SceneObject(_obstacle)
    { }

    ~Obstacle() { }

    static SceneObject* create(int _meshId)
    {
        ngl::Transformation t;
        t.setPosition(utils::genRandPointInBox(10.0, 10.0));

        return new Obstacle(_meshId, t);
    }
};

#endif // OBSTACLE_H
