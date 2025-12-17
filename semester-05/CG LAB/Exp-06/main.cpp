#include <GL/glut.h>
#include <iostream>

// Target and boundary color in RGB format [0-255]
class BoundaryFill {
private:
    unsigned char boundaryColor[3];
    unsigned char fillColor[3];

public:
    BoundaryFill(unsigned char r, unsigned char g, unsigned char b) {
        fillColor[0] = r;
        fillColor[1] = g;
        fillColor[2] = b;
        boundaryColor[0] = 255;
        boundaryColor[1] = 0;
        boundaryColor[2] = 0; // red boundary
    }

    void fill(int x, int y) {
        unsigned char pixel[3];
        glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

        // Check if current pixel is neither boundary nor fill color
        if (!(pixel[0] == boundaryColor[0] && pixel[1] == boundaryColor[1] && pixel[2] == boundaryColor[2]) &&
            !(pixel[0] == fillColor[0] && pixel[1] == fillColor[1] && pixel[2] == fillColor[2])) {

            glColor3ub(fillColor[0], fillColor[1], fillColor[2]);
            glBegin(GL_POINTS);
            glVertex2i(x, y);
            glEnd();
            glFlush();

            // Recursive calls
            fill(x + 1, y);
            fill(x - 1, y);
            fill(x, y + 1);
            fill(x, y - 1);
        }
    }
};

BoundaryFill* boundaryFill;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3ub(255, 0, 0); // Red boundary
    glBegin(GL_LINE_LOOP);
    glVertex2i(100, 100);
    glVertex2i(300, 100);
    glVertex2i(300, 300);
    glVertex2i(100, 300);
    glEnd();
    glFlush();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int winHeight = glutGet(GLUT_WINDOW_HEIGHT);
        y = winHeight - y; // Convert y to OpenGL coordinates
        boundaryFill->fill(x, y);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Boundary Fill Algorithm");

    glClearColor(1, 1, 1, 1);
    gluOrtho2D(0, 500, 0, 500);

    boundaryFill = new BoundaryFill(0, 255, 0); // Green fill color

    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();

    delete boundaryFill;
    return 0;
}
