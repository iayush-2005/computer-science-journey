#include <GL/glut.h>
#include <iostream>
#include <cmath>
#include <algorithm>
using namespace std;

// ------------------
// Liang–Barsky Class
// ------------------
class LiangBarsky {
public:
    double xMin, xMax, yMin, yMax;
    double x1, y1, x2, y2;
    double cx1, cy1, cx2, cy2;
    bool isClipped;

    LiangBarsky(double xmin, double xmax, double ymin, double ymax,
                double X1, double Y1, double X2, double Y2) {
        xMin = xmin; xMax = xmax; yMin = ymin; yMax = ymax;
        x1 = X1; y1 = Y1; x2 = X2; y2 = Y2;
        isClipped = false;
    }

    void clipLine() {
        double dx = x2 - x1, dy = y2 - y1;
        double p[4] = {-dx, dx, -dy, dy};
        double q[4] = {x1 - xMin, xMax - x1, y1 - yMin, yMax - y1};
        double t1 = 0.0, t2 = 1.0;
        bool accept = true;

        for (int i = 0; i < 4; i++) {
            if (fabs(p[i]) < 1e-9 && q[i] < 0) { // parallel & outside
                accept = false;
                break;
            }
            if (fabs(p[i]) > 1e-9) {
                double r = q[i] / p[i];
                if (p[i] < 0) t1 = max(t1, r);
                else t2 = min(t2, r);
            }
        }

        if (!accept || t1 > t2) isClipped = false;
        else {
            cx1 = x1 + t1 * dx; cy1 = y1 + t1 * dy;
            cx2 = x1 + t2 * dx; cy2 = y1 + t2 * dy;
            isClipped = true;
        }
    }
};

// ------------------
// Global variables
// ------------------
LiangBarsky* lb;
const int winW = 500, winH = 500;

// ------------------
// Helper Function
// ------------------
void drawRectangle(double xmin, double xmax, double ymin, double ymax) {
    // Rectangle outline
    glColor3f(0, 0, 1);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(xmin, ymin);
        glVertex2f(xmax, ymin);
        glVertex2f(xmax, ymax);
        glVertex2f(xmin, ymax);
    glEnd();

    // Reset width
    glLineWidth(1);
}

void drawLine(double x1, double y1, double x2, double y2, float r, float g, float b, float width=1) {
    glColor3f(r, g, b);
    glLineWidth(width);
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1);
}

// ------------------
// GLUT display function
// ------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw clipping rectangle (blue)
    drawRectangle(lb->xMin, lb->xMax, lb->yMin, lb->yMax);

    // Original line (red)
    drawLine(lb->x1, lb->y1, lb->x2, lb->y2, 1, 0, 0);

    // Clipped line (green)
    if (lb->isClipped) {
        drawLine(lb->cx1, lb->cy1, lb->cx2, lb->cy2, 0, 1, 0, 3);
    }

    glFlush();
}

// ------------------
// Main function
// ------------------
int main(int argc, char** argv) {
    cout << "=== Liang Barsky Line Clipping ===\n";
    double xmin, xmax, ymin, ymax, x1, y1, x2, y2;

    cout << "Clipping window (xmin xmax ymin ymax): "; cin >> xmin >> xmax >> ymin >> ymax;
    cout << "Line coordinates (x1 y1 x2 y2): "; cin >> x1 >> y1 >> x2 >> y2;

    lb = new LiangBarsky(xmin, xmax, ymin, ymax, x1, y1, x2, y2);
    lb->clipLine();

    if (lb->isClipped)
        cout << "Clipped line: (" << lb->cx1 << "," << lb->cy1 << ") -> (" << lb->cx2 << "," << lb->cy2 << ")\n";
    else
        cout << "Line completely outside\n";

    // Initialize GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(winW, winH);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Liang Barsky Line Clipping");

    glClearColor(1, 1, 1, 1); // White background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Adjust coordinate system based on clipping window + line
    double minX = min({xmin, x1, x2}) - 1;
    double maxX = max({xmax, x1, x2}) + 1;
    double minY = min({ymin, y1, y2}) - 1;
    double maxY = max({ymax, y1, y2}) + 1;
    gluOrtho2D(minX, maxX, minY, maxY);

    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}