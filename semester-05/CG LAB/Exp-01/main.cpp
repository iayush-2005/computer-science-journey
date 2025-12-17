#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

// User shape choice
int shapeChoice = 1;
int windowWidth = 500, windowHeight = 500;

// OpenGL init
void initOpenGL() {
    glClearColor(1.0, 1.0, 1.0, 1.0);     // White background
    glColor3f(0.0, 0.0, 0.0);            // Default draw color
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);            // 2D coordinate system
}

// ------------------ Shape Functions ------------------

void drawLine() {
    glColor3f(0.0, 0.0, 0.0);            // Black
    glBegin(GL_LINES);
        glVertex2f(-0.5f, 0.0f);
        glVertex2f(0.5f, 0.0f);
    glEnd();
}

void drawTriangle() {
    glColor3f(1.0, 0.2, 0.2);            // Red
    glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.4f);
        glVertex2f(0.5f, -0.4f);
        glVertex2f(0.0f, 0.5f);
    glEnd();
}

void drawRectangle() {
    glColor3f(0.2, 0.7, 0.2);            // Green
    glBegin(GL_QUADS);
        glVertex2f(-0.5f, -0.4f);
        glVertex2f(0.5f, -0.4f);
        glVertex2f(0.5f, 0.4f);
        glVertex2f(-0.5f, 0.4f);
    glEnd();
}

void drawPentagon(float radius = 0.4f) {
    glColor3f(0.9, 0.5, 0.1);            // Orange
    glBegin(GL_POLYGON);
    for (int i = 0; i < 5; ++i) {
        float angle = 2 * M_PI * i / 5;
        glVertex2f(radius * cos(angle), radius * sin(angle));
    }
    glEnd();
}

// ------------------ Display Callback ------------------

void displayCallback() {
    glClear(GL_COLOR_BUFFER_BIT);

    switch (shapeChoice) {
        case 1: drawLine(); break;
        case 2: drawTriangle(); break;
        case 3: drawRectangle(); break;
        case 4: drawPentagon(); break;
        default: cout << "Invalid shape!" << endl; break;
    }

    glFlush();
}

// ------------------ Main ------------------

int main(int argc, char** argv) {
    cout << "Choose a shape to draw:\n";
    cout << "1. Line\n";
    cout << "2. Triangle\n";
    cout << "3. Rectangle\n";
    cout << "4. Pentagon\n";
    cout << "Enter your choice (1-4): ";
    cin >> shapeChoice;

    if (shapeChoice < 1 || shapeChoice > 4) {
        cout << "Invalid choice. Exiting.\n";
        return 0;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("CG Lab - Basic 2D Shape Drawing");

    initOpenGL();
    glutDisplayFunc(displayCallback);
    glutMainLoop();

    return 0;
}
