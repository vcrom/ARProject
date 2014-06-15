#ifndef TURRET_H
#define TURRET_H

#include <glm/glm.hpp>

#define TURRET_RANGE 50.0f
class Turret
{
public:
    Turret();
    static bool inRange(glm::vec3 pos_turret, glm::vec3 pos_objective);
};

#endif // TURRET_H
