#include "animatedobject.h"

AnimatedObject::AnimatedObject()
{
    frame = 0;
    location = 0;
    rotation = 0.0f;
    frames_to_target = 100;
    finish = false;
}

AnimatedObject::AnimatedObject(int speed) {
    frame = 0;
    location = 0;
    rotation = 0.0f;
    frames_to_target = speed;
    finish = false;
}


AnimatedObject::~AnimatedObject() {}

bool AnimatedObject::reachedGoal()
{
    return finish;
}

void AnimatedObject::addFrame(unsigned int next, unsigned int nextnext) {
    frame += 1;
    rotation += ROT_SPED;

    if (frame >= frames_to_target) {
        frame = 0;
        location = next;
        if(location >= nextnext)
            finish = true;
    }
}

void AnimatedObject::render(Pattern kanji, std::vector<Pattern> hiros)
{
    GLfloat   mat_ambient[]     = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash[]       = {1.0, 0.0, 0.0, 1.0};
    GLfloat   mat_flash_shiny[] = {50.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);

    glm::vec4 origin = hiros[location].get_origin();

    unsigned int next = (location+1)%hiros.size();
    while (!hiros[next].active)
        next = (next+1)%hiros.size();
    glm::vec4 target = hiros[next].get_origin();

    double * gl_para = kanji.get_transformation_matrix();
    float t = ((float) frame)/(float)frames_to_target;
    gl_para[12] += origin.x * (1-t) + target.x*t;
    gl_para[13] += origin.y * (1-t) + target.y*t;
    gl_para[14] += origin.z * (1-t) + target.z*t;

    position = glm::vec3(gl_para[12], gl_para[13], gl_para[14]);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixd( gl_para );


    //glScalef(50, 50, 50);
    glRotatef(rotation, 0, 1, 0);

    //glutSolidDodecahedron();
    glutSolidCube(30);

    unsigned int nextnext = (next+1)%hiros.size();
    while (!hiros[nextnext].active)
        nextnext = (nextnext+1)%hiros.size();
    addFrame(next, nextnext);
}

glm::vec3 AnimatedObject::getPosition()
{
    return position;
}
