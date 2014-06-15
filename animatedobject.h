#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include "mesh.h"
#include "pattern.hpp"
#include "vector"

#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/freeglut.h>

#define ROT_SPED 0.9;
class AnimatedObject
{
public:
    AnimatedObject();
    AnimatedObject(int speed);
    ~AnimatedObject();

    void render(Pattern kanji, std::vector<Pattern> hiros);
    bool reachedGoal();
    glm::vec3 getPosition();

private:
    void addFrame(int num_crl_points);
    unsigned int frame;
    unsigned int location;
    float rotation;
    float frames_to_target;
    bool finish;

    glm::vec3 position;

};

#endif // ANIMATEDOBJECT_H
