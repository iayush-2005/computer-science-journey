#include <GL/glut.h>
#include <iostream>
#include <iomanip>
#include <cmath>
using namespace std;

class EllipseDrawer {
private:
    float xc, yc; // center
    float rx, ry; // radii

    void plotPoints(float x, float y) {
        glVertex2f(xc + x, yc + y);
        glVertex2f(xc - x, yc + y);
        glVertex2f(xc + x, yc - y);
        glVertex2f(xc - x, yc - y);
    }

    void printTableHeader() {
        cout << "\n+--------+--------+--------+------------+----------+----------+\n";
        cout << "| Region |   x    |   y    |     p      |   dx     |   dy     |\n";
        cout << "+--------+--------+--------+------------+----------+----------+\n";
    }

    void printTableRow(int region, float x, float y, float p, float dx, float dy) {
        cout << "|" << setw(7) << region
             << " |" << setw(7) << (int)x
             << " |" << setw(7) << (int)y
             << " |" << setw(11) << (int)p
             << " |" << setw(9) << (int)dx
             << " |" << setw(9) << (int)dy << " |\n";
    }

public:
    void getInput() {
        cout << "Enter center (xc yc): ";
        cin >> xc >> yc;
        cout << "Enter radius along X (rx): ";
        cin >> rx;
        cout << "Enter radius along Y (ry): ";
        cin >> ry;
    }

    void drawEllipse() {
        float x = 0, y = ry;
        float dx = 2 * ry * ry * x;
        float dy = 2 * rx * rx * y;
        float p1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx);

        glBegin(GL_POINTS);
        printTableHeader();

        // Region 1
        while (dx < dy) {
            plotPoints(x, y);
            printTableRow(1, x, y, p1, dx, dy);

            if (p1 < 0) {
                x++;
                dx += 2 * ry * ry;
                p1 += dx + (ry * ry);
            } else {
                x++;
                y--;
                dx += 2 * ry * ry;
                dy -= 2 * rx * rx;
                p1 += dx - dy + (ry * ry);
            }
        }

        // Region 2
        float p2 = ((ry * ry) * ((x + 0.5) * (x + 0.5))) +
                   ((rx * rx) * ((y - 1) * (y - 1))) -
                   (rx * rx * ry * ry);

        while (y >= 0) {
            plotPoints(x, y);
            printTableRow(2, x, y, p2, dx, dy);

            if (p2 > 0) {
                y--;
                dy -= 2 * rx * rx;
                p2 += (rx * rx) - dy;
            } else {
                y--;
                x++;
                dx += 2 * ry * ry;
                dy -= 2 * rx * rx;
                p2 += dx - dy + (rx * rx);
            }
        }

        cout << "+--------+--------+--------+------------+----------+----------+\n";
        glEnd();
        glFlush();
    }
};

// Global instance
EllipseDrawer ellipseObj;

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    ellipseObj.drawEllipse();
}

void init() {
    glClearColor(0, 0, 0, 1);
    glColor3f(1, 1, 1);
    glPointSize(2.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-500, 500, -500, 500);
}

int main(int argc, char** argv) {
    ellipseObj.getInput();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Midpoint Ellipse Algorithm");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
