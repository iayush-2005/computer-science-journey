#include <GL/glut.h>
#include <iostream>
using namespace std;

const int MAX_POINTS = 50;

// --------------------- Point Class ---------------------
class Point {
public:
    float x, y;
    Point() {}
    Point(float _x, float _y) {
        x = _x;
        y = _y;
    }
};

// --------------------- Polygon Class ---------------------
class Polygon {
public:
    Point points[MAX_POINTS];
    int n;

    Polygon() {
        n = 0;
    }

    void addPoint(float x, float y) {
        if (n < MAX_POINTS)
            points[n++] = Point(x, y);
    }

    void drawOutline(float r, float g, float b) {
        glColor3f(r, g, b);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < n; i++)
            glVertex2f(points[i].x, points[i].y);
        glEnd();
    }

    void fill(float r, float g, float b, float alpha = 0.3f) {
        glColor4f(r, g, b, alpha);
        glBegin(GL_POLYGON);
        for (int i = 0; i < n; i++)
            glVertex2f(points[i].x, points[i].y);
        glEnd();
    }
};

// --------------------- Clipper Class ---------------------
class Clipper {
public:
    float xmin, ymin, xmax, ymax;
    Point intersections[MAX_POINTS];
    int intCount;

    Clipper(float left, float bottom, float right, float top) {
        xmin = left;
        ymin = bottom;
        xmax = right;
        ymax = top;
        intCount = 0;
    }

    bool inside(Point p, char edge) {
        if (edge == 'L') return p.x >= xmin;
        if (edge == 'R') return p.x <= xmax;
        if (edge == 'B') return p.y >= ymin;
        if (edge == 'T') return p.y <= ymax;
        return false;
    }

    Point intersection(Point p1, Point p2, char edge) {
        float x, y, m;
        if (p2.x - p1.x != 0)
            m = (p2.y - p1.y) / (p2.x - p1.x);
        else
            m = 1e6; // vertical line handling

        switch (edge) {
        case 'L': x = xmin; y = p1.y + m * (xmin - p1.x); break;
        case 'R': x = xmax; y = p1.y + m * (xmax - p1.x); break;
        case 'B': y = ymin; x = p1.x + (ymin - p1.y) / m; break;
        case 'T': y = ymax; x = p1.x + (ymax - p1.y) / m; break;
        }

        // Store intersection for visualization
        if (intCount < MAX_POINTS)
            intersections[intCount++] = Point(x, y);

        return Point(x, y);
    }

    Polygon clipPolygon(Polygon input) {
        Polygon output = input;
        char edges[4] = { 'L', 'R', 'B', 'T' };

        for (int e = 0; e < 4; e++) {
            Polygon temp;
            for (int i = 0; i < output.n; i++) {
                Point curr = output.points[i];
                Point next = output.points[(i + 1) % output.n];

                bool currIn = inside(curr, edges[e]);
                bool nextIn = inside(next, edges[e]);

                if (currIn && nextIn) {
                    temp.addPoint(next.x, next.y);
                }
                else if (currIn && !nextIn) {
                    Point I = intersection(curr, next, edges[e]);
                    temp.addPoint(I.x, I.y);
                }
                else if (!currIn && nextIn) {
                    Point I = intersection(curr, next, edges[e]);
                    temp.addPoint(I.x, I.y);
                    temp.addPoint(next.x, next.y);
                }
            }
            output = temp;
        }
        return output;
    }

    void drawWindow(float r, float g, float b) {
        glColor3f(r, g, b);
        glBegin(GL_LINE_LOOP);
        glVertex2f(xmin, ymin);
        glVertex2f(xmax, ymin);
        glVertex2f(xmax, ymax);
        glVertex2f(xmin, ymax);
        glEnd();
    }

    void drawIntersections() {
        glPointSize(6);
        glColor3f(1, 0, 0); // red points
        glBegin(GL_POINTS);
        for (int i = 0; i < intCount; i++)
            glVertex2f(intersections[i].x, intersections[i].y);
        glEnd();
    }
};

// --------------------- Globals ---------------------
Polygon subjectPolygon, clippedPolygon;
Clipper* clipper;

// --------------------- Display ---------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Clipping window (yellow)
    clipper->drawWindow(1, 1, 0);

    // Original polygon (cyan)
    subjectPolygon.fill(0, 1, 1, 0.3);
    subjectPolygon.drawOutline(0, 1, 1);

    // Clipped polygon (green)
    clippedPolygon.fill(0, 1, 0, 0.4);
    clippedPolygon.drawOutline(0, 1, 0);

    // Intersection points (red)
    clipper->drawIntersections();

    glFlush();
}

// --------------------- Init ---------------------
void init() {
    glClearColor(0, 0, 0, 1);
    gluOrtho2D(0, 500, 0, 500);
}

// --------------------- Main ---------------------
int main(int argc, char** argv) {
    float xmin = 150, ymin = 150, xmax = 350, ymax = 350;
    clipper = new Clipper(xmin, ymin, xmax, ymax);

    int n;
    cout << "Enter number of vertices in polygon: ";
    cin >> n;

    cout << "Enter polygon vertices (x y):" << endl;
    for (int i = 0; i < n; i++) {
        float x, y;
        cin >> x >> y;
        subjectPolygon.addPoint(x, y);
    }

    // Perform clipping
    clippedPolygon = clipper->clipPolygon(subjectPolygon);

    // Initialize OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Sutherland-Hodgman Polygon Clipping with Intersections");
    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
