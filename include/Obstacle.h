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

    void update(ngl::Real _deltaT) { }
};

#endif // OBSTACLE_H
