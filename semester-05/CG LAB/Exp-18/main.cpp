#include <GL/glut.h>
#include <iostream>
#include <cmath>
using namespace std;

struct Point {
    float x, y;
    bool isIntersection, isEntry;
    int id;
    Point(float a=0, float b=0) : x(a), y(b), isIntersection(false), isEntry(false), id(-1) {}
};

Point subjectPoly[20], clipPoly[20], subjectList[50], clipList[50], resultPoly[50];
int subjectSize=0, clipSize=0, subListSize=0, clipListSize=0, resultSize=0, intersectionCount=0;

bool isInside(Point p, Point poly[], int n) {
    bool inside = false;
    for (int i=0, j=n-1; i<n; j=i++)
        if (((poly[i].y>p.y) != (poly[j].y>p.y)) && 
            (p.x < (poly[j].x-poly[i].x)*(p.y-poly[i].y)/(poly[j].y-poly[i].y)+poly[i].x))
            inside = !inside;
    return inside;
}

bool getIntersection(Point p1, Point p2, Point p3, Point p4, Point& inter) {
    float d = (p1.x-p2.x)*(p3.y-p4.y) - (p1.y-p2.y)*(p3.x-p4.x);
    if (d < 0.0001 && d > -0.0001) return false;
    float t = ((p1.x-p3.x)*(p3.y-p4.y) - (p1.y-p3.y)*(p3.x-p4.x)) / d;
    float u = -((p1.x-p2.x)*(p1.y-p3.y) - (p1.y-p2.y)*(p1.x-p3.x)) / d;
    if (t>0 && t<1 && u>0 && u<1) {
        inter.x = p1.x + t*(p2.x-p1.x);
        inter.y = p1.y + t*(p2.y-p1.y);
        return true;
    }
    return false;
}

void buildLists() {
    subListSize = clipListSize = intersectionCount = 0;
    for (int i=0; i<subjectSize; i++) {
        subjectList[subListSize++] = subjectPoly[i];
        for (int j=0; j<clipSize; j++) {
            Point inter;
            if (getIntersection(subjectPoly[i], subjectPoly[(i+1)%subjectSize], 
                               clipPoly[j], clipPoly[(j+1)%clipSize], inter)) {
                inter.isIntersection = true;
                inter.id = intersectionCount++;
                Point mid((subjectPoly[i].x+inter.x)/2, (subjectPoly[i].y+inter.y)/2);
                inter.isEntry = !isInside(mid, clipPoly, clipSize);
                subjectList[subListSize++] = inter;
            }
        }
    }
    for (int j=0; j<clipSize; j++) {
        clipList[clipListSize++] = clipPoly[j];
        for (int i=0; i<subjectSize; i++) {
            Point inter;
            if (getIntersection(clipPoly[j], clipPoly[(j+1)%clipSize], 
                               subjectPoly[i], subjectPoly[(i+1)%subjectSize], inter)) {
                inter.isIntersection = true;
                for (int k=0; k<subListSize; k++)
                    if (subjectList[k].isIntersection && 
                        abs(subjectList[k].x-inter.x)<0.001 && abs(subjectList[k].y-inter.y)<0.001) {
                        inter.id = subjectList[k].id;
                        inter.isEntry = subjectList[k].isEntry;
                        break;
                    }
                clipList[clipListSize++] = inter;
            }
        }
    }
}

void printTable() {
    cout << "\n" << string(90,'=') << "\nSUBJECT LIST                        |  CLIPPING LIST\n" << string(90,'=') << "\n";
    int maxRows = max(subListSize, clipListSize);
    for (int i=0; i<maxRows; i++) {
        if (i<subListSize) {
            cout << i << "(" << subjectList[i].x << "," << subjectList[i].y << ") ";
            cout << (subjectList[i].isIntersection ? "I"+to_string(subjectList[i].id)+" " : "V ");
            cout << (subjectList[i].isIntersection ? (subjectList[i].isEntry?"ENTRY":"EXIT ") : "     ");
        } else cout << "                                    ";
        cout << "|  ";
        if (i<clipListSize) {
            cout << i << "(" << clipList[i].x << "," << clipList[i].y << ") ";
            cout << (clipList[i].isIntersection ? "I"+to_string(clipList[i].id)+" " : "V ");
            cout << (clipList[i].isIntersection ? (clipList[i].isEntry?"ENTRY":"EXIT") : "");
        }
        cout << "\n";
    }
    cout << string(90,'=') << "\n";
}

void performClipping() {
    resultSize = 0;
    bool visited[50] = {false};
    for (int start=0; start<subListSize; start++) {
        if (subjectList[start].isIntersection && subjectList[start].isEntry && !visited[start]) {
            int curr = start;
            bool inSubject = true;
            do {
                if (inSubject) {
                    visited[curr] = true;
                    resultPoly[resultSize++] = subjectList[curr];
                    if (subjectList[curr].isIntersection && !subjectList[curr].isEntry) {
                        inSubject = false;
                        int intersectionID = subjectList[curr].id;
                        for (int i=0; i<clipListSize; i++)
                            if (clipList[i].isIntersection && clipList[i].id == intersectionID) {
                                curr = i; break;
                            }
                    }
                    curr = (curr+1) % subListSize;
                } else {
                    resultPoly[resultSize++] = clipList[curr];
                    if (clipList[curr].isIntersection && clipList[curr].isEntry) {
                        inSubject = true;
                        int intersectionID = clipList[curr].id;
                        for (int i=0; i<subListSize; i++)
                            if (subjectList[i].isIntersection && subjectList[i].id == intersectionID) {
                                curr = i; break;
                            }
                        break;
                    }
                    curr = (curr+1) % clipListSize;
                }
            } while (!(inSubject && curr == start));
        }
    }
    cout << "Clipped vertices: " << resultSize << "\n";
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.2,0.2,0.3); glBegin(GL_QUADS); glVertex2f(-250,250); glVertex2f(250,250); 
    glVertex2f(250,-250); glVertex2f(-250,-250); glEnd();
    glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.2,0.6,1.0,0.3); glBegin(GL_POLYGON);
    for (int i=0; i<subjectSize; i++) glVertex2f(subjectPoly[i].x, subjectPoly[i].y);
    glEnd(); glColor3f(0.2,0.6,1.0); glLineWidth(2); glBegin(GL_LINE_LOOP);
    for (int i=0; i<subjectSize; i++) glVertex2f(subjectPoly[i].x, subjectPoly[i].y);
    glEnd(); glColor4f(1.0,0.3,0.3,0.3); glBegin(GL_POLYGON);
    for (int i=0; i<clipSize; i++) glVertex2f(clipPoly[i].x, clipPoly[i].y);
    glEnd(); glColor3f(1.0,0.3,0.3); glBegin(GL_LINE_LOOP);
    for (int i=0; i<clipSize; i++) glVertex2f(clipPoly[i].x, clipPoly[i].y);
    glEnd();
    if (resultSize > 0) {
        glColor4f(0.3,1.0,0.4,0.7); glBegin(GL_POLYGON);
        for (int i=0; i<resultSize; i++) glVertex2f(resultPoly[i].x, resultPoly[i].y);
        glEnd(); glColor3f(0.0,0.7,0.2); glLineWidth(3); glBegin(GL_LINE_LOOP);
        for (int i=0; i<resultSize; i++) glVertex2f(resultPoly[i].x, resultPoly[i].y);
        glEnd();
    }
    glColor3f(1.0,1.0,0.0); glPointSize(8); glBegin(GL_POINTS);
    for (int i=0; i<subListSize; i++) if (subjectList[i].isIntersection) glVertex2f(subjectList[i].x, subjectList[i].y);
    glEnd(); glColor3f(1.0,0.8,0.0); glLineWidth(1.5);
    for (int i=0; i<subListSize; i++) {
        if (subjectList[i].isIntersection) {
            glBegin(GL_LINE_LOOP);
            for (int j=0; j<20; j++) {
                float angle = 6.28318 * j / 20;
                glVertex2f(subjectList[i].x + 6*cos(angle), subjectList[i].y + 6*sin(angle));
            }
            glEnd();
        }
    }
    glutSwapBuffers();
}

void init() { glMatrixMode(GL_PROJECTION); gluOrtho2D(-250, 250, -250, 250); }

int main(int argc, char** argv) {
    cout << "\n*** WEILER-ATHERTON POLYGON CLIPPING ***\n\n";
    cout << "Enter subject polygon vertices: "; cin >> subjectSize;
    for (int i=0; i<subjectSize; i++) {
        cout << "V" << i << " (x y): "; cin >> subjectPoly[i].x >> subjectPoly[i].y;
    }
    cout << "\nEnter clipping polygon vertices: "; cin >> clipSize;
    for (int i=0; i<clipSize; i++) {
        cout << "V" << i << " (x y): "; cin >> clipPoly[i].x >> clipPoly[i].y;
    }
    buildLists(); printTable(); performClipping();
    glutInit(&argc, argv); glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800); glutCreateWindow("Weiler-Atherton Clipping");
    init(); glutDisplayFunc(display);
    cout << "\nColors: Blue=Subject | Red=Clipping | Green=Result | Yellow=Intersections\n";
    glutMainLoop();
    return 0;
}