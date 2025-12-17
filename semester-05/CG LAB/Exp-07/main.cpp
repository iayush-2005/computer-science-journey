#include <GL/glut.h>
#include <iostream>

class FloodFill {
private:
    unsigned char targetColor[3];
    unsigned char fillColor[3];

public:
    FloodFill(unsigned char r, unsigned char g, unsigned char b) {
        fillColor[0] = r;
        fillColor[1] = g;
        fillColor[2] = b;
    }

    void getPixelColor(int x, int y, unsigned char* color) {
        glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);
    }

    bool isSameColor(unsigned char* c1, unsigned char* c2) {
        return c1[0] == c2[0] && c1[1] == c2[1] && c1[2] == c2[2];
    }

    void fill(int x, int y) {
        unsigned char currentColor[3];
        getPixelColor(x, y, currentColor);

        if (!isSameColor(currentColor, targetColor)) return;
        if (isSameColor(currentColor, fillColor)) return;

        glColor3ub(fillColor[0], fillColor[1], fillColor[2]);
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();
        glFlush();

        // Recurse in 4 directions
        fill(x + 1, y);
        fill(x - 1, y);
        fill(x, y + 1);
        fill(x, y - 1);
    }

    void start(int x, int y) {
        getPixelColor(x, y, targetColor); // Get color at clicked pixel
        fill(x, y);
    }
};

FloodFill* floodFill;

void displayFlood() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3ub(0, 0, 255); // Blue polygon
    glBegin(GL_POLYGON);
    glVertex2i(150, 150);
    glVertex2i(350, 150);
    glVertex2i(350, 350);
    glVertex2i(150, 350);
    glEnd();
    glFlush();
}

void mouseFlood(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        int winHeight = glutGet(GLUT_WINDOW_HEIGHT);
        y = winHeight - y;
        floodFill->start(x, y);
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Flood Fill Algorithm");

    glClearColor(1.0, 1.0, 1.0, 1.0); // White background
    gluOrtho2D(0, 500, 0, 500);

    floodFill = new FloodFill(255, 255, 0); // Fill color = Yellow

    glutDisplayFunc(displayFlood);
    glutMouseFunc(mouseFlood);
    glutMainLoop();

    delete floodFill;
    return 0;
}
