#ifndef ANIMATEDOBJECT_H
#define ANIMATEDOBJECT_H

#include "mesh.h"
#include "pattern.hpp"
#include "vector"

#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/freeglut.h>

class AnimatedObject
{
public:
    AnimatedObject();
    ~AnimatedObject();

    void addFrame(int num_crl_points);
    void Render(Pattern kanji, std::vector<Pattern> hiros);
private:
    unsigned int frame;
    unsigned int location;
    Mesh *mesh;
};

#endif // ANIMATEDOBJECT_H
