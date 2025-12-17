#include <GL/glut.h>
#include <iostream>
#include <string>
using namespace std;

enum TransformType { SCALE, TRANSLATE };
TransformType transformChoice;

float scaleX = 1.0f, scaleY = 1.0f, scaleZ = 1.0f;
float transX = 0.0f, transY = 0.0f, transZ = 0.0f;

void drawAxes() {
    glBegin(GL_LINES);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray
    glVertex3f(-2, 0, 0); glVertex3f(2, 0, 0);   // X
    glVertex3f(0, -2, 0); glVertex3f(0, 2, 0);   // Y
    glVertex3f(0, 0, -2); glVertex3f(0, 0, 2);   // Z
    glEnd();
}

void drawOriginalCube() {
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray
    glutSolidCube(1.0);
}

void drawTransformedCube() {
    glPushMatrix();
    if (transformChoice == TRANSLATE)
        glTranslatef(transX, transY, transZ);
    else if (transformChoice == SCALE)
        glScalef(scaleX, scaleY, scaleZ);

    glColor3f(0.3f, 0.6f, 0.9f); // Blue
    glutWireCube(1.0);
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(4, 4, 4, 0, 0, 0, 0, 1, 0); // Camera
    drawAxes();
    drawOriginalCube();
    drawTransformedCube();
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (float)w / h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void getUserInput() {
    string choice;
    char axis;
    float value;

    cout << "Choose [1] Translate or [2] Scale: ";
    cin >> choice;

    if (choice == "1") {
        transformChoice = TRANSLATE;
        cout << "Axis (x/y/z): ";
        cin >> axis;
        cout << "Translate value: ";
        cin >> value;

        switch (axis) {
            case 'x': transX = value; break;
            case 'y': transY = value; break;
            case 'z': transZ = value; break;
            default: cout << "Invalid axis.\n";
        }
    } 
    else if (choice == "2") {
        transformChoice = SCALE;
        cout << "Axis (x/y/z): ";
        cin >> axis;
        cout << "Scale factor: ";
        cin >> value;

        switch (axis) {
            case 'x': scaleX = value; break;
            case 'y': scaleY = value; break;
            case 'z': scaleZ = value; break;
            default: cout << "Invalid axis.\n";
        }
    } 
    else {
        cout << "Invalid choice.\n";
    }
}

int main(int argc, char** argv) {
    getUserInput();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("3D Translation and 3D Scaling on 3D Objects");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();
    return 0;
}
