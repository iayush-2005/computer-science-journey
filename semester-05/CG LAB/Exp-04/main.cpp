#include <GL/glut.h>
#include <iostream>
#include <iomanip>
using namespace std;

int radius;
int axis_choice; // 1 = (0, r), 2 = (r, 0)

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

// --------- LOGIC 1: Starting at (0, r) [Y-Axis] ----------
void bresenham_Yaxis() {
    int x = 0;
    int y = radius;
    int P = 3 - 2 * radius;

    cout << "Starting at (0, r) → (0, " << radius << ") [Y-axis]\n";
    cout << "\nDecision Table:\n";
    cout << left << setw(8) << "xk" << setw(8) << "yk" << setw(10) << "Pk"
         << setw(10) << "xInc" << setw(10) << "yInc" << endl;

    while (x <= y) {
        int curr_x = x, curr_y = y;
        int xInc = x + 1;
        int yInc = y;

        if (P < 0) {
            P = P + 4 * x + 6;
        } else {
            yInc = y - 1;
            P = P + 4 * (x - y) + 10;
            y = yInc;
        }

        cout << setw(8) << curr_x << setw(8) << curr_y << setw(10) << P
             << setw(10) << xInc << setw(10) << yInc << endl;

        plot8Points(curr_x, curr_y);
        x = xInc;
    }
}

// --------- LOGIC 2: Starting at (r, 0) [X-Axis] ----------
void bresenham_Xaxis() {
    int x = radius;
    int y = 0;
    int P = 3 - 2 * radius;

    cout << "Starting at (r, 0) → (" << radius << ", 0) [X-axis]\n";
    cout << "\nDecision Table:\n";
    cout << left << setw(8) << "xk" << setw(8) << "yk" << setw(10) << "Pk"
         << setw(10) << "xInc" << setw(10) << "yInc" << endl;

    while (y <= x) {
        int curr_x = x, curr_y = y;
        int yInc = y + 1;
        int xInc = x;

        if (P < 0) {
            P = P + 4 * y + 6;
        } else {
            xInc = x - 1;
            P = P + 4 * (y - x) + 10;
            x = xInc;
        }

        cout << setw(8) << curr_x << setw(8) << curr_y << setw(10) << P
             << setw(10) << xInc << setw(10) << yInc << endl;

        plot8Points(curr_x, curr_y);
        y = yInc;
    }
}

// --------- GLUT Display Function ----------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);  // white color

    if (axis_choice == 1)
        bresenham_Yaxis();   // (0, r)
    else
        bresenham_Xaxis();   // (r, 0)

    glFlush();
}

// --------- OpenGL Setup ----------
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);  // black background
    gluOrtho2D(-500, 500, -500, 500); // 2D orthographic view
}

// --------- Main Function ----------
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
    glutCreateWindow("Bresenham Circle Drawing - Origin");

    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
