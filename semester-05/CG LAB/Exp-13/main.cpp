#include <GL/glut.h>
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;

#define INSIDE 0
#define LEFT   1
#define RIGHT  2
#define BOTTOM 4
#define TOP    8

struct Point { double x, y; };
struct Rectangle { double xMin, yMin, xMax, yMax; };

Point A, B, clippedA, clippedB;
Rectangle clipWindow;
bool visible = false;

// Compute region code
int computeCode(const Point& p) {
    int code = INSIDE;
    if (p.x < clipWindow.xMin) code |= LEFT;
    else if (p.x > clipWindow.xMax) code |= RIGHT;
    if (p.y < clipWindow.yMin) code |= BOTTOM;
    else if (p.y > clipWindow.yMax) code |= TOP;
    return code;
}

// Cohen-Sutherland Line Clipping
bool cohenSutherlandClip(Point &p1, Point &p2) {
    int code1 = computeCode(p1);
    int code2 = computeCode(p2);

    while (true) {
        if ((code1 | code2) == 0) return true;
        else if (code1 & code2) return false;
        else {
            int codeOut = (code1 != 0) ? code1 : code2;
            double x, y;
            double dx = p2.x - p1.x;
            double dy = p2.y - p1.y;
            double m = (fabs(dx) < 1e-10) ? 1e10 : dy/dx;

            if (codeOut & TOP)      { y = clipWindow.yMax; x = p1.x + (1.0/m)*(y - p1.y); }
            else if (codeOut & BOTTOM) { y = clipWindow.yMin; x = p1.x + (1.0/m)*(y - p1.y); }
            else if (codeOut & RIGHT)  { x = clipWindow.xMax; y = p1.y + m*(x - p1.x); }
            else                       { x = clipWindow.xMin; y = p1.y + m*(x - p1.x); }

            if (codeOut == code1) { p1 = {x, y}; code1 = computeCode(p1); }
            else { p2 = {x, y}; code2 = computeCode(p2); }
        }
    }
}

// Normalize coordinates to [0,1]
double normalizeX(double x) { return (x - 0) / (15.0 - 0); }
double normalizeY(double y) { return (y - 0) / (15.0 - 0); }

// OpenGL display
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw clipping rectangle 
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(2.0);

    glColor3f(1.0, 1.0, 1.0); // white
    glBegin(GL_LINE_LOOP);
        glVertex2d(normalizeX(clipWindow.xMin), normalizeY(clipWindow.yMin));
        glVertex2d(normalizeX(clipWindow.xMax), normalizeY(clipWindow.yMin));
        glVertex2d(normalizeX(clipWindow.xMax), normalizeY(clipWindow.yMax));
        glVertex2d(normalizeX(clipWindow.xMin), normalizeY(clipWindow.yMax));
    glEnd();

    // Draw original line 
    glColor3f(0.0, 1.0, 1.0); // cyan
    glBegin(GL_LINES);
        glVertex2d(normalizeX(A.x), normalizeY(A.y));
        glVertex2d(normalizeX(B.x), normalizeY(B.y));
    glEnd();

    // Draw clipped line 
    if (visible) {
        glColor3f(1.0, 1.0, 0.0); // yellow
        glBegin(GL_LINES);
            glVertex2d(normalizeX(clippedA.x), normalizeY(clippedA.y));
            glVertex2d(normalizeX(clippedB.x), normalizeY(clippedB.y));
        glEnd();
    }

    glFlush();
}

void init() {
    glClearColor(0,0,0,1); // black background
    gluOrtho2D(0, 1, 0, 1); // normalized coordinates

    // blending for smooth lines
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char** argv) {
    cout << "Enter clipping window (xMin yMin xMax yMax): ";
    cin >> clipWindow.xMin >> clipWindow.yMin >> clipWindow.xMax >> clipWindow.yMax;

    cout << "Enter line start point (x y): ";
    cin >> A.x >> A.y;
    cout << "Enter line end point (x y): ";
    cin >> B.x >> B.y;

    clippedA = A; clippedB = B;
    visible = cohenSutherlandClip(clippedA, clippedB);

    cout << fixed << setprecision(2);
    if (visible) {
        cout << "Clipped line: (" << clippedA.x << "," << clippedA.y << ") to ("
             << clippedB.x << "," << clippedB.y << ")\n";
    } else cout << "Line rejected (completely outside).\n";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500,500);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Cohen-Sutherland Line Clipping");
    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
