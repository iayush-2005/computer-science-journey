#include <GL/glut.h>
#include <iostream>
#include <iomanip>
using namespace std;

const int MAXV = 50;
const int screenW = 600, screenH = 600;

class Point {
public:
    int x, y;
    Point(int xv = 0, int yv = 0) { x = xv; y = yv; }
};

struct Edge {
    int yMin, yMax;
    double xAtYMin;
    double invSlope; // dx/dy
};

Point poly[MAXV];
Edge edges[MAXV];
int nVertices = 0, nEdges = 0;

// ---------------- Input ----------------
void readVertices() {
    cout << "Enter number of vertices: ";
    cin >> nVertices;
    if (nVertices < 3) { cout << "Polygon must have >=3 vertices\n"; exit(0); }

    cout << "Enter coordinates (x y):\n";
    for (int i = 0; i < nVertices; i++) {
        cout << "Point " << (i + 1) << ": ";
        cin >> poly[i].x >> poly[i].y;
    }
}

void printVertexTable() {
    cout << "\nVertex Table\n";
    cout << setw(8) << "Index" << setw(10) << "X" << setw(10) << "Y" << "\n";
    cout << string(28, '-') << "\n";
    for (int i = 0; i < nVertices; i++)
        cout << setw(8) << (i + 1)
             << setw(10) << poly[i].x
             << setw(10) << poly[i].y << "\n";
}

// ---------------- Edge Table (GET) ----------------
void buildGET() {
    nEdges = 0;
    for (int i = 0; i < nVertices; i++) {
        Point p1 = poly[i];
        Point p2 = poly[(i + 1) % nVertices];
        if (p1.y == p2.y) continue; // skip horizontal edges

        int yMin, yMax;
        double xMin;
        if (p1.y < p2.y) { yMin = p1.y; yMax = p2.y; xMin = p1.x; }
        else            { yMin = p2.y; yMax = p1.y; xMin = p2.x; }

        edges[nEdges].yMin = yMin;
        edges[nEdges].yMax = yMax;
        edges[nEdges].xAtYMin = xMin;
        edges[nEdges].invSlope = (double)(p2.x - p1.x) / (p2.y - p1.y);
        nEdges++;
    }
}

void printGET() {
    cout << "\nGlobal Edge Table (GET)\n";
    cout << setw(8) << "Edge#"
         << setw(8) << "yMin"
         << setw(8) << "yMax"
         << setw(12) << "xAtYMin"
         << setw(12) << "invSlope" << "\n";
    cout << string(48, '-') << "\n";

    for (int i = 0; i < nEdges; i++) {
        cout << setw(8) << (i + 1)
             << setw(8) << edges[i].yMin
             << setw(8) << edges[i].yMax
             << setw(12) << fixed << setprecision(2) << edges[i].xAtYMin
             << setw(12) << fixed << setprecision(3) << edges[i].invSlope
             << "\n";
    }
}

// ---------------- Bubble sort helper ----------------
void bubbleSortIndices(int idx[], double xvals[], int count) {
    for (int i = 0; i < count - 1; i++)
        for (int j = 0; j < count - 1 - i; j++)
            if (xvals[idx[j]] > xvals[idx[j + 1]])
                swap(idx[j], idx[j + 1]);
}

// ---------------- Scanline Fill ----------------
void scanlineFill() {
    int minY = poly[0].y, maxY = poly[0].y;
    for (int i = 1; i < nVertices; i++) {
        if (poly[i].y < minY) minY = poly[i].y;
        if (poly[i].y > maxY) maxY = poly[i].y;
    }

    int activeIdx[MAXV];
    double xCurr[MAXV];
    int activeCount;

    glColor3f(0.7, 0.5, 0.3);
    glBegin(GL_LINES);

    for (int y = minY; y < maxY; y++) {
        activeCount = 0;

        // collect AET
        for (int e = 0; e < nEdges; e++) {
            if (y >= edges[e].yMin && y < edges[e].yMax) {
                activeIdx[activeCount] = e;
                xCurr[e] = edges[e].xAtYMin + (y - edges[e].yMin) * edges[e].invSlope;
                activeCount++;
            }
        }

        // print AET for this scanline
        if (activeCount > 0) {
            cout << "\nActive Edge Table (AET) at y = " << y << "\n";
            cout << setw(8) << "Edge#" << setw(12) << "xCurr" << "\n";
            cout << string(20, '-') << "\n";
            for (int i = 0; i < activeCount; i++) {
                int e = activeIdx[i];
                cout << setw(8) << (e + 1)
                     << setw(12) << fixed << setprecision(2) << xCurr[e] << "\n";
            }
        }

        if (activeCount < 2) continue;

        int idxList[MAXV];
        for (int i = 0; i < activeCount; i++) idxList[i] = activeIdx[i];
        bubbleSortIndices(idxList, xCurr, activeCount);

        for (int i = 0; i + 1 < activeCount; i += 2) {
            double x1 = xCurr[idxList[i]], x2 = xCurr[idxList[i + 1]];
            glVertex2i((int)x1, y);
            glVertex2i((int)x2, y);
        }
    }

    glEnd();

    // outline
    glColor3f(0, 0, 0);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < nVertices; i++) glVertex2i(poly[i].x, poly[i].y);
    glEnd();
}

// ---------------- GLUT display ----------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    scanlineFill();
    glFlush();
}

int main(int argc, char** argv) {
    readVertices();
    printVertexTable();

    buildGET();
    printGET();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(screenW, screenH);
    glutCreateWindow("Scanline Algorithm");
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, screenW, 0, screenH);
    glClearColor(1, 1, 1, 1);

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
