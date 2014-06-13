#ifndef CBBOX_H
#define CBBOX_H

#include "glm/glm.hpp"


using namespace glm;


struct CBBox
{

    vec3 limits[2];

    CBBox();
    CBBox(const vec3 &minPoint, const vec3 &maxPoint);

    void set(const vec3 &minPoint, const vec3 &maxPoint);

    vec3 getCenter() const;
    float getCircumRadius() const;
};


#endif // BBOX_H
