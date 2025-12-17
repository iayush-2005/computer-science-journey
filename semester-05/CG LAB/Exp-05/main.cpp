#include <GL/glut.h>
#include <iostream>
#include <iomanip>
using namespace std;

int radius;
int axis_choice; // 1 = (0,r), 2 = (r,0)

// Function to plot 8 symmetric points around origin
void plot8Points(int x, int y) {
    glBegin(GL_POINTS);
        glVertex2i( x,  y);
        glVertex2i( y,  x);
        glVertex2i(-x,  y);
        glVertex2i(-y,  x);
        glVertex2i(-x, -y);
        glVertex2i(-y, -x);
        glVertex2i( x, -y);
        glVertex2i( y, -x);
    glEnd();
}

// Midpoint circle starting at (0, r)
void midpoint_Yaxis() {
    int x = 0;
    int y = radius;
    int P = 1 - radius;  // Step 2: decision parameter

    cout << "Starting at (0, r) → (0, " << radius << ") [Y-axis]\n";
    cout << "\nDecision Table:\n";
    cout << left << setw(8) << "xk" << setw(8) << "yk" << setw(10) << "Pk"
         << setw(10) << "xInc" << setw(10) << "yInc" << endl;

    while (x <= y) {
        int curr_x = x, curr_y = y;
        int xInc = x + 1;
        int yInc = y;

        if (P < 0) {
            // Pk+1 = Pk + 2*xk + 3
            P = P + 2 * x + 3;
        } else {
            // Pk+1 = Pk + 2*(xk - yk) + 5
            yInc = y - 1;
            P = P + 2 * (x - y) + 5;
            y = yInc;
        }

        cout << setw(8) << curr_x << setw(8) << curr_y << setw(10) << P
             << setw(10) << xInc << setw(10) << yInc << endl;

        plot8Points(curr_x, curr_y);
        x = xInc;
    }
}

// Midpoint circle starting at (r, 0)
void midpoint_Xaxis() {
    int x = radius;
    int y = 0;
    int P = 1 - radius;  // Step 2: decision parameter

    cout << "Starting at (r, 0) → (" << radius << ", 0) [X-axis]\n";
    cout << "\nDecision Table:\n";
    cout << left << setw(8) << "xk" << setw(8) << "yk" << setw(10) << "Pk"
         << setw(10) << "xInc" << setw(10) << "yInc" << endl;

    while (y <= x) {
        int curr_x = x, curr_y = y;
        int xInc = x;
        int yInc = y + 1;

        if (P < 0) {
            // Pk+1 = Pk + 2*yk + 3
            P = P + 2 * y + 3;
        } else {
            // Pk+1 = Pk + 2*(yk - xk) + 5
            xInc = x - 1;
            P = P + 2 * (y - x) + 5;
            x = xInc;
        }

        cout << setw(8) << curr_x << setw(8) << curr_y << setw(10) << P
             << setw(10) << xInc << setw(10) << yInc << endl;

        plot8Points(curr_x, curr_y);
        y = yInc;
    }
}

void display() {
    static bool printed = false;  // flag to print decision table only once

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);

    if (!printed) {
        if (axis_choice == 1)
            midpoint_Yaxis();
        else
            midpoint_Xaxis();

        printed = true;
    }

    glFlush();
}

void init() {
    glClearColor(0, 0, 0, 1);
    gluOrtho2D(-500, 500, -500, 500);
}

int main(int argc, char** argv) {
    cout << "Enter radius of circle: ";
    cin >> radius;

    cout << "Choose starting axis for radius:\n";
    cout << "1. Y-axis (0, r)\n";
    cout << "2. X-axis (r, 0)\n";
    cout << "Enter choice (1 or 2): ";
    cin >> axis_choice;

    if (axis_choice != 1 && axis_choice != 2) {
        cout << "Invalid choice. Exiting...\n";
        return 1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Midpoint Circle Drawing Algorithm");

    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}