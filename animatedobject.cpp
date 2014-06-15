#include "animatedobject.h"

AnimatedObject::AnimatedObject()
{
    frame = 0;
    location = 0;
}

AnimatedObject::~AnimatedObject()
{
    if(mesh != NULL)
    {
        delete mesh;
        mesh = NULL;
    }
}

void AnimatedObject::addFrame(int num_crl_points)
{
    frame += 1;
    if (frame == 100) {
        frame = 0;
        location = (location+1)% num_crl_points;
    }
}

void AnimatedObject::Render(Pattern kanji, std::vector<Pattern> hiros)
{
    glm::vec4 origin = hiros[location].get_origin();
    glm::vec4 target = hiros[(location+1)%hiros.size()].get_origin();
    double * gl_para = kanji.get_transformation_matrix();
    float t = ((float) frame)/100.0f;
    gl_para[12] += origin.x * (1-t) + target.x*t;
    gl_para[13] += origin.y * (1-t) + target.y*t;
    gl_para[14] += origin.z * (1-t) + target.z*t;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadMatrixd( gl_para );

    glMatrixMode(GL_MODELVIEW);
    //glTranslatef( 0.0, 0.0, 25.0 );
    glScalef(50, 50, 50);
    //mesh->Render();
    glutSolidTeapot(30);

    addFrame(hiros.size());
}
