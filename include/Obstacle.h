#ifndef OBSTACLE_H
#define OBSTACLE_H

#include "BaseObjects.h"
#include "Utils.h"

class Obstacle : public MovingObject
{
public:
    Obstacle(const Mesh* _mesh, int _shaderId, const ngl::Transformation& _transform = ngl::Transformation()):
        MovingObject(_mesh, _shaderId, _transform)
    { }

    Obstacle(const Obstacle& _obstacle):
        MovingObject(_obstacle)
    { }

    ~Obstacle() { }

    void update(ngl::Real _deltaT)
    { }

    static RenderObject* sCreate(const Mesh* _mesh)
    {
        ngl::Transformation t;
        t.setPosition(utils::genRandPointInBox(10.0, 10.0));

        return new Obstacle(_mesh, -1, t);
    }
};

#endif // OBSTACLE_H
