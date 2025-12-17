#include <GL/glut.h>
#include <iostream>
#include <algorithm>

using namespace std;

// ==== Circle Drawing Class ====
class CircleDrawer {
    int algo;

    void putPixel(int x, int y) {
        glBegin(GL_POINTS);
        glVertex2i(x, y);
        glEnd();
    }

    void plotCirclePoints(int xc, int yc, int x, int y) {
        putPixel(xc + x, yc + y);
        putPixel(xc - x, yc + y);
        putPixel(xc + x, yc - y);
        putPixel(xc - x, yc - y);
        putPixel(xc + y, yc + x);
        putPixel(xc - y, yc + x);
        putPixel(xc + y, yc - x);
        putPixel(xc - y, yc - x);
    }

    void midpointCircle(int xc, int yc, int r) {
        int x = 0, y = r;
        int d = 1 - r;
        plotCirclePoints(xc, yc, x, y);
        while (x < y) {
            if (d < 0) d += 2 * x + 3;
            else {
                d += 2 * (x - y) + 5;
                y--;
            }
            x++;
            plotCirclePoints(xc, yc, x, y);
        }
    }

    void bresenhamCircle(int xc, int yc, int r) {
        int x = 0, y = r;
        int d = 3 - 2 * r;
        plotCirclePoints(xc, yc, x, y);
        while (y >= x) {
            x++;
            if (d > 0) {
                y--;
                d += 4 * (x - y) + 10;
            } else {
                d += 4 * x + 6;
            }
            plotCirclePoints(xc, yc, x, y);
        }
    }

public:
    CircleDrawer(int algorithm) : algo(algorithm) {}

    void drawCircle(int xc, int yc, int r) {
        glColor3f(1.0f, 0.6f, 0.2f); // Soft orange
        if (algo == 1) midpointCircle(xc, yc, r);
        else if (algo == 2) bresenhamCircle(xc, yc, r);
        else cout << "Invalid algorithm choice\n";
    }
};

// ==== Rectangle Class ====
class Rectangle {
    int x, y;
    int length, width;

public:
    Rectangle(int _x, int _y, int l, int w) : x(_x), y(_y), length(l), width(w) {}

    void draw() {
        glColor3f(1.0f, 1.0f, 0.5f); // Light yellow
        glBegin(GL_LINE_LOOP);
        glVertex2i(x, y);
        glVertex2i(x + length, y);
        glVertex2i(x + length, y + width);
        glVertex2i(x, y + width);
        glEnd();
    }

    // Circle centers at midpoint of each side, exactly radius away from edge
    void getCircleCenters(int centers[4][2], int radius) {
        // Bottom center
        centers[0][0] = x + length / 2;
        centers[0][1] = y - radius;

        // Right center
        centers[1][0] = x + length + radius;
        centers[1][1] = y + width / 2;

        // Top center
        centers[2][0] = x + length / 2;
        centers[2][1] = y + width + radius;

        // Left center
        centers[3][0] = x - radius;
        centers[3][1] = y + width / 2;
    }
};

// ==== Globals ====
Rectangle* rect;
CircleDrawer* circleDrawer;
int circleRadius;

// ==== Utility ====
int allocateCircleRadius(int width) {
    return max(8, width / 4);  // Based only on height of rectangle
}

void drawGradientBackground() {
    glBegin(GL_QUADS);
    glColor3f(0.2f, 0.4f, 0.7f); glVertex2i(0, 0);
    glColor3f(0.2f, 0.4f, 0.7f); glVertex2i(800, 0);
    glColor3f(0.0f, 0.1f, 0.4f); glVertex2i(800, 600);
    glColor3f(0.0f, 0.1f, 0.4f); glVertex2i(0, 600);
    glEnd();
}

// ==== Display ====
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawGradientBackground();
    rect->draw();

    int centers[4][2];
    rect->getCircleCenters(centers, circleRadius);

    for (int i = 0; i < 4; i++) {
        circleDrawer->drawCircle(centers[i][0], centers[i][1], circleRadius);
    }

    glFlush();
}

void initGL() {
    glClearColor(0, 0, 0, 0);
    gluOrtho2D(0, 800, 0, 600);

    glPointSize(3);  // Smooth points
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
}

// ==== Main ====
int main(int argc, char** argv) {
    int length, width, algoChoice;

    cout << "Enter rectangle length (horizontal): ";
    cin >> length;
    cout << "Enter rectangle width (vertical): ";
    cin >> width;

    circleRadius = allocateCircleRadius(width);
    cout << "\nCircle radius set to: " << circleRadius << endl;

    cout << "\nChoose circle drawing algorithm:\n1. Midpoint Circle\n2. Bresenham Circle\nChoice: ";
    cin >> algoChoice;

    // Center the rectangle
    int startX = (800 - length) / 2;
    int startY = (600 - width) / 2;
    rect = new Rectangle(startX, startY, length, width);
    circleDrawer = new CircleDrawer(algoChoice);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Customized Shape - Rectangle Surrounded by Circles");
    initGL();
    glutDisplayFunc(display);
    glutMainLoop();

    delete rect;
    delete circleDrawer;
    return 0;
}
