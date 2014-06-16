
#include <stdio.h>
#include <stdlib.h>

#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/freeglut.h>

#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>

#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

//
// Camera configuration.
//
#ifdef _WIN32
char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
char			*vconf = "";
#endif

int             xsize, ysize;
int             thresh = 150;//200;
int             counter = 0;

const char           *cparam_name    = "Data/camera_para.dat";
ARParam         cparam;

const char           *patt_hiro      = "Data/patt.hiro";
const char           *patt_kanji      = "Data/patt.kanji";
const char           *patt_sample1      = "Data/patt.sample1";
int            id_kanji, id_hiro, id_sample1;

static void   init(void);
static void   cleanup(void);
static void   keyEvent( unsigned char key, int x, int y);
static void   mainLoop(void);
static void   draw( void );

#include "pattern.hpp"
#include "mesh.h"
#include "animatedobject.h"
#include "turret.h"

Mesh mesh;
Pattern kanji;
vector<Pattern> hiro;
vector<Pattern> towers;


//struct animated_object {
//    unsigned int frame;
//    unsigned int location;
//};

#define BULLET_SPEED 5.0f
#define BULLET_RAD 10.0f
#define TOWER_SHOOT_FREQ 10
struct bullet
{
    glm::vec3 position;
    bool has_target;
    glm::vec3 targ_pos;
    uint target_id;
};
std::vector<bullet> bullets;

//animated_object renderable;
bool debug = false;
std::vector<AnimatedObject> enemies;

void initGL() {
    glewInit();
    if(!mesh.load("models/bunny.ply"))
        std::cout << "Failed to load mesh" << std::endl;
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    init();

    arVideoCapStart();
    argMainLoop( NULL, keyEvent, mainLoop );
    return (0);
}

static void keyEvent( unsigned char key, int x, int y) {
    /* quit if the ESC key is pressed */
    if( key == 0x1b ) {
        printf("*** %f (frame/sec)\n", (double)counter/arUtilTimer());
        cleanup();
        exit(0);
    }
    if( key == 'd' ) {
        debug = !debug;
    }
}

bool pattern_sort(Pattern a, Pattern b) {
    return a.get_origin().x < b.get_origin().x;
}

void updateBullets()
{
    for(uint i = 0; i < bullets.size(); ++i)
    {
        bullet bull = bullets[i];
        if(bull.has_target)
        {
            AnimatedObject enemy = enemies[bull.target_id];
            bull.targ_pos = enemy.getPosition();
        }
        glm::vec3 dir = bull.targ_pos - bull.position;
        dir = glm::normalize(dir);

        dir *= BULLET_SPEED;
        bull.position += dir;

        bullets[i] = bull;
    }

    for(uint i = 0; i < bullets.size(); ++i)
    {
        bullet bull = bullets[i];

        if(glm::distance(bull.targ_pos, bull.position) < BULLET_RAD)
        {
            bullets.erase(bullets.begin()+i, bullets.begin()+i+1);
            --i;
            if(bull.has_target)
            {
                enemies.erase(enemies.begin() + bull.target_id, enemies.begin() + bull.target_id+1);
                for(uint j = 0; j < bullets.size(); ++j)
                {
                    if(bullets[j].target_id == bull.target_id) bullets[j].has_target = false;
                }
            }
        }
    }
}

std::vector<int> elapsed_frames_tower;
void updateTowers() {

    for(uint i = 0; i < towers.size(); ++i) {
        ++elapsed_frames_tower[i];
        if (towers[i].active) {
            double * gl_para = kanji.get_transformation_matrix();
            glm::vec4 origin = towers[i].get_origin();
            glm::vec3 turret_position = glm::vec3(gl_para[12] + origin.x, gl_para[13] + origin.y, gl_para[14] + origin.z);
            int enemy = -1;

            for(uint j = 0; j < enemies.size(); ++j) {

                glm::vec3 enemy_position = enemies[j].getPosition();
                if(Turret::inRange(turret_position, enemy_position)) {
                    enemy = j;
                    break;
                }
            }

            if(enemy >= 0 && kanji.active) {
                if(elapsed_frames_tower[i]%TOWER_SHOOT_FREQ == 0)
                {
                    bullet bull;
                    bull.has_target = true;
                    bull.target_id = enemy;
                    glm::vec3 pos = glm::vec3(towers[i].get_origin().x + kanji.get_origin().x, towers[i].get_origin().y + kanji.get_origin().y, towers[i].get_origin().z + kanji.get_origin().z);
                    bull.position = pos;
                    bullets.push_back(bull);
                }
            }
        }
    }
}

#define ENEMIES_PROB 0.02d
void updateEnemies() {
    if((double)rand()/(double)RAND_MAX >= 1-ENEMIES_PROB && enemies.size() < 5)
        enemies.push_back(AnimatedObject());

    for(uint i = 0; i < enemies.size(); ++i) {

        if(enemies[i].reachedGoal()) {
            cout << "ouch" << endl;
            enemies.erase(enemies.begin()+i, enemies.begin()+i+1);
            for(uint j = 0; j < bullets.size(); ++j) {
                if(bullets[j].target_id == i) bullets[j].has_target = false;

            }
        }
    }
}

/* main loop */
static void mainLoop(void) {
    ARUint8         *dataPtr;
    ARMarkerInfo    *marker_info;
    int             marker_num;
    int             j;

    /* grab a vide frame */
    if( (dataPtr = (ARUint8 *)arVideoGetImage()) == NULL ) {
        arUtilSleep(2);
        return;
    }

    if (counter == 0) arUtilTimerReset();
    counter++;

    if (counter%1100 == 0) {
        vector<Pattern> new_hiros;
        for (unsigned int i = 0; i < hiro.size(); ++i)
            if (hiro[i].active)
                new_hiros.push_back(hiro[i]);
        hiro = new_hiros;

        vector<Pattern> new_turrets;
        for (unsigned int i = 0; i < towers.size(); ++i)
            if (counter - towers[i].last_activated < 100)
                new_turrets.push_back(towers[i]);
        towers = new_turrets;
    }

    argDrawMode2D();
    argDispImage( dataPtr, 0,0 );

    /* detect the markers in the video frame */
    if( arDetectMarker(dataPtr, thresh, &marker_info, &marker_num) < 0 ) {
        cleanup();
        exit(0);
    }

    arVideoCapNext();

    glColor3f(0.0, 1.0, 0.0);
    glLineWidth(6.0f);

    for(int i = 0; i < marker_num; ++i)
        argDrawSquare(marker_info[i].vertex, 0, 0);
    cerr << marker_num << "Patterns detected!" << endl;


    for (unsigned int i = 0; i < towers.size(); ++i)
        towers[i].active = false;

    for( j = 0; j < marker_num; j++ ) {

        if (id_kanji == marker_info[j].id) {

            kanji.set_position(&marker_info[j]);

        } else if (id_hiro == marker_info[j].id && kanji.oriented) {

            double patt_width = 80.f;
            double patt_center[2] = {0, 0};
            double patt_trans[3][4];

            double gl_para[16];

            arGetTransMat(&marker_info[j], patt_center, patt_width, patt_trans);
            argConvGlpara(patt_trans, gl_para);

            int closest = -1;
            float min_dist = FLT_MAX;
            for (unsigned int i = 0; i < hiro.size(); ++i) {
                glm::vec4 korigin = hiro[i].reference;
                glm::vec4 origin(gl_para[12]-korigin.x, gl_para[13]-korigin.y, gl_para[14]-korigin.z, 1.0);
                float dist = glm::distance(origin, hiro[i].get_origin());

                if (dist < min_dist) {
                    min_dist = dist;
                    closest = i;
                }
            }

            if (closest < 0 || (min_dist > 200 )) {
                hiro.push_back(Pattern());
                closest = hiro.size()-1;
            }


            hiro[closest].accomulate_relative_position(&marker_info[j], kanji.get_origin());

        } else if(id_sample1 == marker_info[j].id && kanji.oriented ) {

            double patt_width = 80.f;
            double patt_center[2] = {0, 0};
            double patt_trans[3][4];

            double gl_para[16];

            arGetTransMat(&marker_info[j], patt_center, patt_width, patt_trans);
            argConvGlpara(patt_trans, gl_para);

            int closest = -1;
            float min_dist = FLT_MAX;
            for (unsigned int i = 0; i < towers.size(); ++i) {
                glm::vec4 korigin = towers[i].reference;
                glm::vec4 origin(gl_para[12]-korigin.x, gl_para[13]-korigin.y, gl_para[14]-korigin.z, 1.0);
                float dist = glm::distance(origin, towers[i].get_origin());

                if (dist < min_dist) {
                    min_dist = dist;
                    closest = i;
                }
            }

            if (closest < 0 || (min_dist > 200 )) {
                elapsed_frames_tower.push_back(0);
                towers.push_back(Pattern());
                closest = towers.size()-1;
            }

            towers[closest].accomulate_relative_position(&marker_info[j], kanji.get_origin());
            towers[closest].active = true;
            towers[closest].last_activated = counter;
        }
    }

    sort(hiro.begin(), hiro.end(), pattern_sort);


    for (unsigned int i = 0; i < hiro.size(); ++i) {
        cout << hiro[i].active << " - " << hiro[i].get_origin().x  << " " << hiro[i].get_origin().y << " " << hiro[i].get_origin().z << endl;
    }

    unsigned int active_hiros = 0;
    for (unsigned int i = 0; i < hiro.size(); ++i)
        active_hiros += hiro[i].active;
    if(active_hiros > 1) {
        updateTowers();
        updateBullets();
        updateEnemies();
    }
    draw();
    argSwapBuffers();
}

static void init( void ) {
    ARParam  wparam;

    /* open the video path */
    if( arVideoOpen( vconf ) < 0 ) exit(0);
    /* find the size of the window */
    if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
    printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

    /* set the initial camera parameters */
    if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
        printf("Camera parameter load error !!\n");
        exit(0);
    }

    arParamChangeSize( &wparam, xsize, ysize, &cparam );
    arInitCparam( &cparam );
    printf("*** Camera Parameter ***\n");
    arParamDisp( &cparam );

    id_kanji = arLoadPatt(patt_kanji);
    id_hiro = arLoadPatt(patt_hiro);
    id_sample1 = arLoadPatt(patt_sample1);
    if ( id_kanji < 0 || id_hiro < 0 || id_sample1 < 0) {
        printf("pattern load error !!\n");
        exit(0);
    }

    /* open the graphics window */
    argInit( &cparam, 1.0, 0, 0, 0, 0 );

    //init GL and meshes
    initGL();

//    renderable.frame = 0;
//    renderable.location = 0;

//    enemies.push_back(AnimatedObject());
//    bullet bull;
//    bull.has_target = false;
//    bull.target_id = 0;
//    bull.position = glm::vec3(100, 100, 0);
//    bull.targ_pos = glm::vec3(0,0,0);
//    bullets.push_back(bull);

}

/* cleanup function called when program exits */
static void cleanup(void)
{
    arVideoCapStop();
    arVideoClose();
    argCleanup();
}

void drawPath()
{
    double * gl_para = kanji.get_transformation_matrix();
    glm::vec3 pos_kanji(gl_para[12], gl_para[13], gl_para[14]);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    for(uint i = 0; i < hiro.size()-1; ++i)
    {
        glm::vec4 p1 = hiro[i].get_origin();
        glm::vec4 p2 = hiro[i+1].get_origin();
        glVertex3f(p1.x+pos_kanji.x, p1.y+pos_kanji.y, p1.z+pos_kanji.z);
        glVertex3f(p2.x+pos_kanji.x, p2.y+pos_kanji.y, p2.z+pos_kanji.z);
    }
    glEnd();
}

static void draw( void )
{
    GLfloat   light_position[]  = {100.0,-200.0,200.0,0.0};
    GLfloat   ambi[]            = {0.1, 0.1, 0.1, 0.1};
    GLfloat   lightZeroColor[]  = {0.9, 0.9, 0.9, 0.1};

    argDrawMode3D();
    argDraw3dCamera( 0, 0 );
    glClearDepth( 1.0 );
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambi);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightZeroColor);



    {
        GLfloat   mat_ambient[]     = {0.0, 0.0, 1.0, 1.0};
        GLfloat   mat_flash[]       = {0.0, 0.0, 1.0, 1.0};
        GLfloat   mat_flash_shiny[] = {50.0};
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        for(int i = 0; i < towers.size(); ++i)
        {
            if (towers[i].active) {
                double * gl_para = kanji.get_transformation_matrix();
                glm::vec4 origin = towers[i].get_origin();
                gl_para[12] += origin.x;
                gl_para[13] += origin.y;
                gl_para[14] += origin.z;

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glLoadMatrixd( gl_para );

                glutSolidTeapot(50);
                //glutSolidCone(50,20,20,20);
            }
        }
    }
    {
        glEnable (GL_BLEND);

        GLfloat mat_ambient[]    = {0.0, 1.0, 0.0, 0.5};
        GLfloat mat_flash[]       = {0.0, 1.0, 0.0, 0.7};
        GLfloat mat_flash_shiny[] = {50.0};
        glMaterialfv(GL_FRONT, GL_SPECULAR, mat_flash);
        glMaterialfv(GL_FRONT, GL_SHININESS, mat_flash_shiny);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
        for(uint i = 0; i < bullets.size(); ++i)
        {

            double * gl_para = kanji.get_transformation_matrix();
            gl_para[12] = bullets[i].position.x;
            gl_para[13] = bullets[i].position.y;
            gl_para[14] = bullets[i].position.z;

            glMatrixMode(GL_MODELVIEW);
            glLoadIdentity();
            glLoadMatrixd( gl_para );

            glutSolidSphere(BULLET_RAD, 20, 20);

        }
        glDisable (GL_BLEND);
    }

    unsigned int active_hiros = 0;
    for (unsigned int i = 0; i < hiro.size(); ++i)
        active_hiros += hiro[i].active;

    if (active_hiros > 1) {
        for(uint i = 0; i < enemies.size(); ++i) {
            enemies[i].render(kanji, hiro);
        }
    }


    glDisable( GL_LIGHTING );

    glDisable( GL_DEPTH_TEST );

    if(debug) drawPath();

    /** cout << gl_para[0] << " " << gl_para[4] << " " << gl_para[8] << " " << gl_para[12] << endl <<
             gl_para[1] << " " << gl_para[5] << " " << gl_para[9] << " " << gl_para[13] << endl <<
             gl_para[2] << " " << gl_para[6] << " " << gl_para[10]<< " " << gl_para[14]<< endl <<
             gl_para[3]<< " " << gl_para[7]<< " " << gl_para[11]<< " " << gl_para[15] << endl << endl << endl;
    **/
}
