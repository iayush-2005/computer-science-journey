#include <GL/glut.h>
#include <iostream>
#include <string>
using namespace std;

// Rotation parameters
float angle = 0.0f;
float rotX = 0.0f, rotY = 0.0f, rotZ = 0.0f;

void drawAxes() {
    glBegin(GL_LINES);
    glColor3f(0.6f, 0.6f, 0.6f); // Gray axes

    // X axis
    glVertex3f(-2, 0, 0); glVertex3f(2, 0, 0);
    // Y axis
    glVertex3f(0, -2, 0); glVertex3f(0, 2, 0);
    // Z axis
    glVertex3f(0, 0, -2); glVertex3f(0, 0, 2);

    glEnd();
}

void drawOriginalCube() {
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray
    glutSolidCube(1.0);
}

void drawRotatedCube() {
    glPushMatrix();
    glRotatef(angle, rotX, rotY, rotZ); // Apply rotation
    glColor3f(0.3f, 0.6f, 0.9f);        // blue
    glutWireCube(1.0);                  // cube
    glPopMatrix();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(4, 4, 4, 0, 0, 0, 0, 1, 0); // Camera
    drawAxes();
    drawOriginalCube();
    drawRotatedCube();
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
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f); // Dark gray background
}

// Function to get rotation input
void getUserInput() {
    int choice;
    char axis;
    float value;

    cout << "\n----- 3D Rotation Menu -----\n";
    cout << "1. Rotate around X-axis\n";
    cout << "2. Rotate around Y-axis\n";
    cout << "3. Rotate around Z-axis\n";
    cout << "4. Exit\n";
    cout << "Enter your choice: ";
    cin >> choice;

    if (choice == 4) {
        exit(0);
    }

    cout << "Enter rotation angle (degrees): ";
    cin >> value;

    angle = value;
    rotX = rotY = rotZ = 0.0f;

    switch (choice) {
        case 1: rotX = 1.0f; break;
        case 2: rotY = 1.0f; break;
        case 3: rotZ = 1.0f; break;
        default: cout << "Invalid choice.\n";
    }
}

// Menu handler
void menu(int option) {
    if (option == 4)
        exit(0);

    angle = 0.0f;
    rotX = rotY = rotZ = 0.0f;

    switch (option) {
        case 1: rotX = 1.0f; angle += 30.0f; break;
        case 2: rotY = 1.0f; angle += 30.0f; break;
        case 3: rotZ = 1.0f; angle += 30.0f; break;
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    getUserInput();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(600, 600);
    glutCreateWindow("3D Rotation of Cube");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    // create right-click menu
    glutCreateMenu(menu);
    glutAddMenuEntry("Rotate around X-axis", 1);
    glutAddMenuEntry("Rotate around Y-axis", 2);
    glutAddMenuEntry("Rotate around Z-axis", 3);
    glutAddMenuEntry("Exit", 4);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    glutMainLoop();
    return 0;
}
