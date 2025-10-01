// -------------------------------------------
// gMini : a minimal OpenGL/GLUT application
// for 3D graphics.
// Copyright (C) 2006-2008 Tamy Boubekeur
// All rights reserved.
// -------------------------------------------

// -------------------------------------------
// Disclaimer: this code is dirty in the
// meaning that there is no attention paid to
// proper class attribute access, memory
// management or optimisation of any kind. It
// is designed for quick-and-dirty testing
// purpose.
// -------------------------------------------


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>

#include <algorithm>
#include "Camera.h"
#include "kmath/vector.hpp"
#include "scene.h"
#include <GL/glut.h>

#include "utils/gl_utils.hpp"

// #include "imageLoader.h"
// #include "Material.h"


// -------------------------------------------
// OpenGL/GLUT application code.
// -------------------------------------------

static GLint window;
static unsigned int SCREENWIDTH = 480;
static unsigned int SCREENHEIGHT = 480;
static Camera camera;
static bool mouseRotatePressed = false;
static bool mouseMovePressed = false;
static bool mouseZoomPressed = false;
static int lastX=0, lastY=0, lastZoom=0;
static unsigned int FPS = 0;
static bool fullScreen = false;

std::vector<Scene> scenes;
unsigned int selected_scene;

std::vector<std::pair<kmath::Vec3, kmath::Vec3>> rays;

void printUsage () {
    std::cerr << std::endl
         << "gMini: a minimal OpenGL/GLUT application" << std::endl
         << "for 3D graphics." << std::endl
         << "Author : Tamy Boubekeur (http://www.labri.fr/~boubek)" << std::endl << std::endl
         << "Usage : ./gmini [<file.off>]" << std::endl
         << "Keyboard commands" << std::endl
         << "------------------" << std::endl
         << " ?: Print help" << std::endl
         << " w: Toggle Wireframe Mode" << std::endl
         << " g: Toggle Gouraud Shading Mode" << std::endl
         << " f: Toggle full screen mode" << std::endl
         << " <drag>+<left button>: rotate model" << std::endl
         << " <drag>+<right button>: move model" << std::endl
         << " <drag>+<middle button>: zoom" << std::endl
         << " q, <esc>: Quit" << std::endl << std::endl;
}

void usage () {
    printUsage ();
    exit (EXIT_FAILURE);
}


// ------------------------------------
void initLight () {
    GLfloat light_position[4] = {0.0, 1.5, 0.0, 1.0};
    GLfloat color[4] = { 1.0, 1.0, 1.0, 1.0};
    GLfloat ambient[4] = { 1.0, 1.0, 1.0, 1.0};

    glLightfv (GL_LIGHT1, GL_POSITION, light_position);
    glLightfv (GL_LIGHT1, GL_DIFFUSE, color);
    glLightfv (GL_LIGHT1, GL_SPECULAR, color);
    glLightModelfv (GL_LIGHT_MODEL_AMBIENT, ambient);
    glEnable (GL_LIGHT1);
    glEnable (GL_LIGHTING);
}

void init () {
    camera.resize (SCREENWIDTH, SCREENHEIGHT);
    initLight ();
    //glCullFace (GL_BACK);
    glDisable (GL_CULL_FACE);
    glDepthFunc (GL_LESS);
    glEnable (GL_DEPTH_TEST);
    glClearColor (0.2f, 0.2f, 0.3f, 1.0f);
}


// ------------------------------------
// Replace the code of this 
// functions for cleaning memory, 
// closing sockets, etc.
// ------------------------------------

void clear () {

}

// ------------------------------------
// Replace the code of this 
// functions for alternative rendering.
// ------------------------------------


void draw () {
    glEnable(GL_LIGHTING);
    scenes[selected_scene].draw();

    // draw rays : (for debug)
    //  std::cout << rays.size() << std::endl;
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glLineWidth(6);
    glColor3f(1,0,0);
    glBegin(GL_LINES);
    for( unsigned int r = 0 ; r < rays.size() ; ++r ) {
        glVertex3f( rays[r].first[0],rays[r].first[1],rays[r].first[2] );
        glVertex3f( rays[r].second[0], rays[r].second[1], rays[r].second[2] );
    }
    glEnd();
}

void display () {
    glLoadIdentity ();
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply ();
    draw ();
    glFlush ();
    glutSwapBuffers ();
}

void idle () {
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    static unsigned int counter = 0;
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [64];
        sprintf (winTitle, "Raytracer - FPS: %d", FPS);
        glutSetWindowTitle (winTitle);
        lastTime = currentTime;
    }
    glutPostRedisplay ();
}


void ray_trace_from_camera() {
    using namespace kmath;
    
    int w = glutGet(GLUT_WINDOW_WIDTH)  ,   h = glutGet(GLUT_WINDOW_HEIGHT);
    std::cout << "Ray tracing a " << w << " x " << h << " image" << std::endl;
    camera.apply();

    //    unsigned int nsamples = 100;
    unsigned int sample_count = 50;
    std::vector<Vec3> image(w * h , Vec3::ZERO);

    const Mat4 inv_proj = inverse(get_projection_matrixf());
    const Mat4 inv_model = inverse(get_modelview_matrixf());
    const Mat4 inv_mp = inv_model * inv_proj;

    const Vec3 camera_position = homogeneous_projection(inv_model * Vec4(Vec3::ZERO, 1.0));
    const float near_plane = get_depth_range().x;

    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++) {
            for(unsigned int s = 0 ; s < sample_count ; ++s) {
                float u = ((float)(x) + (float)(rand())/(float)(RAND_MAX)) / w;
                float v = ((float)(y) + (float)(rand())/(float)(RAND_MAX)) / h;
                const Vec3 ray_direction = homogeneous_projection(inv_mp * Vec4(2.0f * u - 1.0f, 2.0f * v - 1.0f, near_plane, 1.0)) - camera_position;
                
                const Vec3 color = scenes[selected_scene].rayTrace(Ray(camera_position, ray_direction));
                image[x + y*w] += color;
            }
            image[x + y*w] /= static_cast<float>(sample_count);
        }
    }
    std::cout << "\tDone" << std::endl;

    std::string filename = "./rendu.ppm";
    std::ofstream f(filename.c_str(), std::ios::binary);
    if (f.fail()) {
        std::cout << "Could not open file: " << filename << std::endl;
        return;
    }
    f << "P3" << std::endl << w << " " << h << std::endl << 255 << std::endl;
    for (int i=0; i<w*h; i++)
        f << (int)(255.f*std::min<float>(1.f,image[i].x)) << " " << (int)(255.f*std::min<float>(1.f,image[i].y)) << " " << (int)(255.f*std::min<float>(1.f,image[i].z)) << " ";
    f << std::endl;
    f.close();
}


void key (unsigned char keyPressed, [[maybe_unused]] int x, [[maybe_unused]] int y) {
    kmath::Vec3 pos, dir;
    switch (keyPressed) {
    case 'f':
        if (fullScreen == true) {
            glutReshapeWindow (SCREENWIDTH, SCREENHEIGHT);
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }
        break;
    case 'q':
    case 27:
        clear ();
        exit (0);
        break;
    case 'w':
        GLint polygonMode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygonMode);
        if(polygonMode[0] != GL_FILL)
            glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
        break;

    case 'r':
        camera.apply();
        rays.clear();
        ray_trace_from_camera();
        break;
    case '+':
        selected_scene++;
        if( selected_scene >= scenes.size() ) selected_scene = 0;
        break;
    default:
        printUsage ();
        break;
    }
    idle ();
}

void mouse (int button, int state, int x, int y) {
    if (state == GLUT_UP) {
        mouseMovePressed = false;
        mouseRotatePressed = false;
        mouseZoomPressed = false;
    } else {
        if (button == GLUT_LEFT_BUTTON) {
            camera.beginRotate (x, y);
            mouseMovePressed = false;
            mouseRotatePressed = true;
            mouseZoomPressed = false;
        } else if (button == GLUT_RIGHT_BUTTON) {
            lastX = x;
            lastY = y;
            mouseMovePressed = true;
            mouseRotatePressed = false;
            mouseZoomPressed = false;
        } else if (button == GLUT_MIDDLE_BUTTON) {
            if (mouseZoomPressed == false) {
                lastZoom = y;
                mouseMovePressed = false;
                mouseRotatePressed = false;
                mouseZoomPressed = true;
            }
        }
    }
    idle ();
}

void motion (int x, int y) {
    if (mouseRotatePressed == true) {
        camera.rotate (x, y);
    }
    else if (mouseMovePressed == true) {
        camera.move ((x-lastX)/static_cast<float>(SCREENWIDTH), (lastY-y)/static_cast<float>(SCREENHEIGHT), 0.0);
        lastX = x;
        lastY = y;
    }
    else if (mouseZoomPressed == true) {
        camera.zoom (float (y-lastZoom)/SCREENHEIGHT);
        lastZoom = y;
    }
}


void reshape(int w, int h) {
    camera.resize (w, h);
}





int main(int argc, char ** argv) {
    if (argc > 2) {
        printUsage ();
        exit (EXIT_FAILURE);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (SCREENWIDTH, SCREENHEIGHT);
    window = glutCreateWindow ("gMini");

    init ();
    glutIdleFunc (idle);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutReshapeFunc (reshape);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    key ('?', 0, 0);


    camera.move(0., 0., -3.1);
    selected_scene=0;
    scenes.resize(3);
    scenes[0].setup_single_sphere();
    scenes[1].setup_single_square();
    scenes[2].setup_cornell_box();

    glutMainLoop ();
    return EXIT_SUCCESS;
}

