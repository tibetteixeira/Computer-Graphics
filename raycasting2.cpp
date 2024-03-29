#include <cmath>
#include <iostream>
#include "scenario.h"
#include "Mat4.h"
#include "Vec3.h"

#ifdef __APPLE__
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#pragma comment(lib, "glew32.lib")
#endif

#define PI 3.14159265

using namespace std;

#define canvasH 500
#define canvasW 500

float camera_rot_phi = 3 * PI / 2;
float camera_rot_theta = 0;

float d = -500;                     // Position of window
int windowW = 500;                  // Width of window
int windowH = 500;                  // Height of window
int dy = windowH/canvasH;           // Size of y delta
int dx = windowW/canvasW;           // Size of x delta

// Initialization routine.
void setup(void) {
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

// Origin's point of camera
Point O(250, 250, 750); // Front
// Point O(250, 250, 2300); // Back
// Point O(-500, 250, 1500); // Left
// Point O(1000, 250, 1500); // Right
// Point O(250, 1000, 1480); // Top
// Point O(250, -500, 1480); // Bottom

Point origin_camera = O;

Point LookAt(250, 250, 1500);
Point ViewUp(LookAt.x, LookAt.y + 100, LookAt.z);
Vec3 K_camera(O.x - LookAt.x, O.y - LookAt.y, O.z - LookAt.z);
Vec3 k_camera = Vec3::normalize(K_camera);
Vec3 O_UP(ViewUp.x - LookAt.x, ViewUp.y - LookAt.y, ViewUp.z - LookAt.z);
Vec3 I_camera = Vec3::cross(O_UP, k_camera);
Vec3 i_camera = Vec3::normalize(I_camera);
Vec3 J_camera = Vec3::cross(k_camera, i_camera);
Vec3 j_camera = Vec3::normalize(J_camera);

Point light_center(-1000, 1500, 0);
Vec3 light_color(1, 1, 1);
Light light(light_center, light_color);
    
// Setting snow material
Vec3 snow_env_material(0.8, 0.8, 0.8);   // Material's enviroment component factors
Vec3 snow_dif_material(0.7, 0.7, 0.8);   // Material's difuse component factors
Vec3 snow_spe_material(0.1, 0.1, 0.1);   // Material's specular component factors
Material snow_material(snow_env_material, snow_dif_material, snow_spe_material);

// Setting eye material
Vec3 eye_env_material(0.2, 0.2, 0.2);    // Material's enviroment component factors
Vec3 eye_dif_material(0.2, 0.2, 0.2);    // Material's difuse component factors
Vec3 eye_spe_material(0.2, 0.2, 0.2);    // Material's specular component factors
Material eye_material(eye_env_material, eye_dif_material, eye_spe_material);

// Setting button material
Vec3 button_env_material(0.3, 0.1, 0.2); // Material's enviroment component factors
Vec3 button_dif_material(0.89803, 0.580392, 0.070588); // Material's difuse component factors
Vec3 button_spe_material(0.2, 0.1, 0.1); // Material's specular component factors
Material button_material(button_env_material, button_dif_material, button_spe_material);

// Snowman definition
Point floor_center(250, -4430, 1500); // Center of floor sphere
Sphere floor_s(floor_center, 4500, snow_material);

Point body_center(250, 150, 1500); // Center of body sphere
Sphere body(body_center, 100, snow_material);

Point head_center(250, 280, 1500);    // Center of head sphere
Sphere head(head_center, 50, snow_material);

Point eye1_center(230, 286, 1453);  // Center of eye1 sphere
Sphere eye1(eye1_center, 8, eye_material);
Point eye2_center(270, 286, 1453); // Center of eye2 sphere
Sphere eye2(eye2_center, 8, eye_material);

Point button1_center(250, 190, 1410); // Center of button1 sphere
Sphere button1(button1_center, 10, button_material);
Point button2_center(250, 150, 1400); // Center of button2 sphere
Sphere button2(button2_center, 10, button_material);
Point button3_center(250, 110, 1410); // Center of button3 sphere
Sphere button3(button3_center, 10, button_material);

int objects_len = 8;                       // Number of objects (spheres) in scenario
Sphere objects[] = {body, head, button2, button3, button1, eye1, eye2, floor_s}; // List of these objects

Point worldToCamera(Point Po) {
    Vec3 OO(origin_camera.x, origin_camera.y, origin_camera.z);
    Vec4 OP(Po.x, Po.y, Po.z, 1);

    Vec4 c1(i_camera[0], j_camera[0], k_camera[0], 0);
    Vec4 c2(i_camera[1], j_camera[1], k_camera[1], 0);
    Vec4 c3(i_camera[2], j_camera[2], k_camera[2], 0);
    Vec4 c4(-(Vec3::dot(i_camera, OO)), -(Vec3::dot(j_camera, OO)), -(Vec3::dot(k_camera, OO)), 1);

    return Point(
        c1[0] * OP[0] + c2[0] * OP[1] + c3[0] * OP[2] + c4[0] * OP[3],
        c1[1] * OP[0] + c2[1] * OP[1] + c3[1] * OP[2] + c4[1] * OP[3],
        c1[2] * OP[0] + c2[2] * OP[1] + c3[2] * OP[2] + c4[2] * OP[3]
    );
}

Point cameraToWorld(Point Po) {
    Vec3 OO(origin_camera.x, origin_camera.y, origin_camera.z);
    Vec4 OP(Po.x, Po.y, Po.z, 1);

    Vec4 c1(i_camera[0], i_camera[1], i_camera[2], 0);
    Vec4 c2(j_camera[0], j_camera[1], j_camera[2], 0);
    Vec4 c3(k_camera[0], k_camera[1], k_camera[2], 0);
    Vec4 c4(OO[0], OO[1], OO[2], 1);

    return Point(
        c1[0] * OP[0] + c2[0] * OP[1] + c3[0] * OP[2] + c4[0] * OP[3],
        c1[1] * OP[0] + c2[1] * OP[1] + c3[1] * OP[2] + c4[1] * OP[3],
        c1[2] * OP[0] + c2[2] * OP[1] + c3[2] * OP[2] + c4[2] * OP[3]);
}

unsigned char *readBMP(char *filename) {
    int i;
    FILE *f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
    int width = *(int *)&info[18];
    int height = *(int *)&info[22];

    int size = 3 * width * height;
    unsigned char *data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f);   // read the rest of the data at once
    fclose(f);

    for (i = 0; i < size; i += 3) {
        unsigned char tmp = data[i];
        data[i] = data[i + 2];
        data[i + 2] = tmp;
    }

    return data;
}

char *filename = (char *)("./mountain-snow-rgb.bmp");
unsigned char *imageData = readBMP(filename);

int buffer[canvasH * canvasW * 3];

// Drawing routine.
void drawScene(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_PROGRAM_POINT_SIZE_EXT);
    glPointSize(1);
    glBegin(GL_POINTS);
        for (int i = 0; i < canvasH; i += 1) {
            for (int j = 0; j < canvasW; j += 1) {
                Point Pix(j * dx + dx / 2 - windowW / 2, i * dy + dy / 2 - windowH / 2, d); // Pixel point in window
                Vec3 V(Pix.x - O.x, Pix.y - O.y, Pix.z - O.z); // Vector from camera to Pixel

                float t_norm = 1000000000;

                // Setting the enviroment light;
                Vec3 Ienv(0.5764705, 0.6470588, 0.756862);
                glColor3ub(imageData[3 * (i * canvasW + j)], imageData[3 * (i * canvasW + j) + 1], imageData[3 * (i * canvasW + j) + 2]);

                int object = -1;
                for (int k = 0; k < objects_len; k++) {
                    bool intercepted = false;
                    float t_int = objects[k].checkInterception(intercepted, V, O);

                    if (intercepted && t_int < t_norm) {
                        object = k;
                        t_norm = t_int;
                    }
                }

                if (object >= 0) {
                    V *= t_norm;
                    int k = object;

                    // Find the intercepted point
                    Point P(V[0], V[1], V[2]);

                    Vec3 N(P.x - objects[k].center.x, P.y - objects[k].center.y, P.z - objects[k].center.z);
                    Vec3 n = Vec3::normalize(N); // Normal vector to point in sphere surface

                    Vec3 L(light.center.x - P.x, light.center.y - P.y, light.center.z - P.z);
                    Vec3 l = Vec3::normalize(L); // Nomalized vector from point to light

                    Vec3 Kenv(objects[k].material.env_material[0], objects[k].material.env_material[1], objects[k].material.env_material[2]); // Components factors to enviroment light

                    // Generating the final color for current pixel
                    Vec3 Color(Ienv[0], Ienv[1], Ienv[2]);
                    Color = Vec3::at(Color, Kenv);

                    Vec3 Kdif(objects[k].material.dif_material[0], objects[k].material.dif_material[1], objects[k].material.dif_material[2]); // Components factors to difuse light
                    Vec3 Kspe(objects[k].material.spe_material[0], objects[k].material.spe_material[1], objects[k].material.spe_material[2]); // Components factors to specular light

                    Vec3 If(light.color[0], light.color[1], light.color[2]); // Light rate

                    // Calculating the difusing rate
                    Vec3 Idif(If[0] * Kdif[0], If[1] * Kdif[1], If[2] * Kdif[2]);
                    Idif *= Vec3::dot(l, n);

                    // Calculating the specular rate
                    Vec3 Ispe(If[0] * Kspe[0], If[1] * Kspe[1], If[2] * Kspe[2]);
                    Vec3 r = (n * (2 * Vec3::dot(l, n))) - l;
                    Vec3 PO(O.x-P.x, O.y-P.y, O.z-P.z);
                    Vec3 v = Vec3::normalize(PO);
                    Ispe *= pow(Vec3::dot(r, v), 3);

                    Color = Vec3::at(Color, Kenv);
                    Color += Idif;
                    Color += Ispe;

                    Vec3 defaultColor (Color[0], Color[1], Color[2]);

                    for (int q = 0; q < objects_len; q++) {
                        if (q != k) {
                            // Checking if the ball is intercepted by light ray
                            bool shadow_intercepted = false;
                            float t_shadow_int = objects[q].checkInterception(shadow_intercepted, l, P, true);
                            if (t_shadow_int && t_shadow_int <= 0) {
                                Color = defaultColor;
                                Vec3 shadow_fact(0.3, 0.3, 0.3);
                                Color = Vec3::at(Color, shadow_fact);
                                q = objects_len;
                            }
                        }
                    }

                    glColor3f(Color[0], Color[1], Color[2]);
                }
                glVertex2f(j, i);
            }
        }
    glEnd();

    glFlush();
}

// OpenGL window reshape routine.
void resize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, canvasW, 0.0, canvasH, 10.0, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void updateCoordinatesToWorld(void) {
    for (int k = 0; k < objects_len; k++) {
        objects[k].center = cameraToWorld(objects[k].center);
    }

    light.center = cameraToWorld(light.center);
    O = origin_camera;
}

void updateCoordinatesToCamera(void) {
    K_camera = Vec3(O.x - LookAt.x, O.y - LookAt.y, O.z - LookAt.z);
    k_camera = Vec3::normalize(K_camera);
    I_camera = Vec3::cross(O_UP, k_camera);
    i_camera = Vec3::normalize(I_camera);
    J_camera = Vec3::cross(k_camera, i_camera);
    j_camera = Vec3::normalize(J_camera);

    origin_camera = O;
    for (int k = 0; k < objects_len; k++) {
        objects[k].center = worldToCamera(objects[k].center);
    }

    light.center = worldToCamera(light.center);
    O = worldToCamera(O);
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
            break;
        case 'w':
            camera_rot_theta -= PI / 6;
            updateCoordinatesToWorld();
            O.y = 250 - sin(camera_rot_theta) * 750;
            O.z = 1500 + sin(camera_rot_phi) * cos(camera_rot_theta) * 750;
            updateCoordinatesToCamera();
            glutPostRedisplay();
            break;
        case 's':
            camera_rot_theta += PI / 6;
            updateCoordinatesToWorld();
            O.y = 250 - sin(camera_rot_theta) * 750;
            O.z = 1500 + sin(camera_rot_phi) * cos(camera_rot_theta) * 750;
            updateCoordinatesToCamera();
            glutPostRedisplay();
            break;
        case 'a':
            camera_rot_phi -= PI / 6;
            updateCoordinatesToWorld();
            O.x = 250 - cos(camera_rot_phi) * 750;
            O.z = 1500 + sin(camera_rot_phi) * cos(camera_rot_theta) * 750;
            updateCoordinatesToCamera();
            glutPostRedisplay();
            break;
        case 'd'    :
            camera_rot_phi += PI / 6;
            updateCoordinatesToWorld();
            O.x = 250 - cos(camera_rot_phi) * 750;
            O.z = 1500 + sin(camera_rot_phi) * cos(camera_rot_theta) * 750;
            updateCoordinatesToCamera();
            glutPostRedisplay();
            break;
        default:
            break;
    }
}

// Main routine.
int main(int argc, char **argv) {
    glutInit(&argc, argv);

    updateCoordinatesToCamera();

    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
    glutInitWindowSize(canvasW, canvasH);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Ray Casting");

    // GLfloat verts[] = {
    //     0, 1,
    //     -1, -1,
    //     1, -1
    // };

    // GLuint myBufferID = 0;
    // glGenBuffers(1, &myBufferID);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, myBufferID);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    // glEnableVertexAttribArray(0);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glutDisplayFunc(drawScene);

    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);

   // glewExperimental = GL_TRUE;
   // glewInit();

    setup();

    glutMainLoop();
}
