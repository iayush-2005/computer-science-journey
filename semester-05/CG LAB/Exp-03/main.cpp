#include <iostream>
#include <cmath>
#include <iomanip>
#include <GL/glut.h>

using namespace std;

const int MAX_POINTS = 1000;

// DDA Line Drawing Algorithm class
class DDA {
private:
    float x1, y1, x2, y2;
    float delX, delY, m;
    float xInc, yInc;

public:
    int pointCount;
    float pointsX[MAX_POINTS];
    float pointsY[MAX_POINTS];

    // Constructor
    DDA(float x_start, float y_start, float x_end, float y_end) {
        x1 = x_start;
        y1 = y_start;
        x2 = x_end;
        y2 = y_end;
        pointCount = 0;
    }

    // Compute DDA points between (x1, y1) and (x2, y2)
    void computeDDA() {
        delX = x2 - x1;
        delY = y2 - y1;

        cout << fixed << setprecision(2);

        // Handle vertical line (undefined slope)
        if (delX == 0) {
            cout << "Vertical line detected. Slope undefined." << endl;
            float yStart = min(y1, y2);
            float yEnd = max(y1, y2);
            cout << "\nStep\tX\tY\tXInc\tYInc\n";
            int step = 0;
            for (float y = yStart; y <= yEnd && pointCount < MAX_POINTS; y += 1.0f) {
                cout << step << "\t" << x1 << "\t" << y << "\t0\t1" << endl;
                pointsX[pointCount] = x1;
                pointsY[pointCount] = y;
                pointCount++;
                step++;
            }
            return;
        }

        m = delY / delX;

        float absDelX = fabs(delX);
        float absDelY = fabs(delY);

        // Determine xInc and yInc based on slope
        if (fabs(m) < 1 && absDelX >= absDelY) {
            xInc = (delX > 0) ? 1 : -1;
            yInc = m * ((delX > 0) ? 1 : -1);
        } else if (fabs(m) > 1 && absDelX < absDelY) {
            yInc = (delY > 0) ? 1 : -1;
            xInc = (1 / m) * ((delY > 0) ? 1 : -1);
        } else if (fabs(m) == 1) {
            xInc = (delX > 0) ? 1 : -1;
            yInc = (delY > 0) ? 1 : -1;
        } else {
            xInc = (delX > 0) ? 1 : -1;
            yInc = m * ((delX > 0) ? 1 : -1);
        }

        cout << "\nStep\tX\tY\tXInc\tYInc\n";

        float x = x1;
        float y = y1;
        int step = 0;
        int steps = max((int)absDelX, (int)absDelY);

        // Generate points
        for (int i = 0; i <= steps && pointCount < MAX_POINTS; i++) {
            cout << step << "\t" << x << "\t" << y << "\t" << xInc << "\t" << yInc << endl;
            pointsX[pointCount] = x;
            pointsY[pointCount] = y;
            pointCount++;
            x += xInc;
            y += yInc;
            step++;
        }
    }
};

DDA* globalDDA = NULL;
int windowWidth = 500;
int windowHeight = 500;

// Normalize points to [-1, 1] range for OpenGL
void normalizePoints(float* xs, float* ys, int count, float* normX, float* normY) {
    if (count == 0) return;

    float minX = xs[0], maxX = xs[0];
    float minY = ys[0], maxY = ys[0];

    for (int i = 1; i < count; i++) {
        minX = min(minX, xs[i]);
        maxX = max(maxX, xs[i]);
        minY = min(minY, ys[i]);
        maxY = max(maxY, ys[i]);
    }

    float rangeX = (maxX - minX == 0) ? 1.0f : (maxX - minX);
    float rangeY = (maxY - minY == 0) ? 1.0f : (maxY - minY);
    float margin = 0.1f;

    // Normalize to OpenGL space [-1, 1]
    for (int i = 0; i < count; i++) {
        float nx = ((xs[i] - minX) / rangeX) * (1 - 2 * margin) + margin;
        float ny = ((ys[i] - minY) / rangeY) * (1 - 2 * margin) + margin;
        normX[i] = nx * 2.0f - 1.0f;
        normY[i] = ny * 2.0f - 1.0f;
    }
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (globalDDA == NULL || globalDDA->pointCount == 0) {
        glutSwapBuffers();
        return;
    }

    float normX[MAX_POINTS], normY[MAX_POINTS];
    normalizePoints(globalDDA->pointsX, globalDDA->pointsY, globalDDA->pointCount, normX, normY);

    // Draw line segments
    glColor3f(1.0f, 1.0f, 1.0f); // white
    glLineWidth(2);
    glBegin(GL_LINES);
    for (int i = 0; i < globalDDA->pointCount - 1; i++) {
        glVertex2f(normX[i], normY[i]);
        glVertex2f(normX[i + 1], normY[i + 1]);
    }
    glEnd();

    glPointSize(8);

    // Start point - red
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    glVertex2f(normX[0], normY[0]);
    glEnd();

    // End point - red
    glBegin(GL_POINTS);
    glVertex2f(normX[globalDDA->pointCount - 1], normY[globalDDA->pointCount - 1]);
    glEnd();

    // Intermediate points - yellow
    if (globalDDA->pointCount > 2) {
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_POINTS);
        for (int i = 1; i < globalDDA->pointCount - 1; i++) {
            glVertex2f(normX[i], normY[i]);
        }
        glEnd();
    }

    glutSwapBuffers();
}

// OpenGL initialization
void initGL() {
    glClearColor(0, 0, 0, 1); // black background
}

// Launch OpenGL window
void runOpenGL(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("CG Lab: DDA Line Drawing Algorithm Visualization");

    initGL();
    glutDisplayFunc(display);
    glutMainLoop();
}

// Main function
int main(int argc, char** argv) {
    float x_start, y_start, x_end, y_end;

    // Input start and end coordinates
    cout << "Enter starting point (x1 y1): ";
    cin >> x_start >> y_start;

    cout << "Enter ending point (x2 y2): ";
    cin >> x_end >> y_end;

    DDA dda(x_start, y_start, x_end, y_end);
    dda.computeDDA(); // calculate line points

    globalDDA = &dda;
    runOpenGL(argc, argv); // display line using OpenGL

    return 0;
}
