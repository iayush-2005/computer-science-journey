#include <GL/glut.h>

// Draws a rectangle given center, width, height
void drawRectangle(float cx, float cy, float width, float height) {
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx - width / 2, cy + height / 2); // Top-left
        glVertex2f(cx + width / 2, cy + height / 2); // Top-right
        glVertex2f(cx + width / 2, cy - height / 2); // Bottom-right
        glVertex2f(cx - width / 2, cy - height / 2); // Bottom-left
    glEnd();
}

// Main stickman drawing function
void drawStickman() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.1f, 0.1f, 0.1f);  // Dark grey stickman

    // ===== HEAD =====
    float headW = 0.3f, headH = 0.35f;
    float headCenterX = 0.0f, headTopY = 0.75f;
    float headCenterY = headTopY - headH / 2;
    drawRectangle(headCenterX, headCenterY, headW, headH);

    // ===== BODY =====
    float bodyW = 0.25f, bodyH = 0.5f;
    float bodyCenterY = headCenterY - (headH / 2 + bodyH / 2);
    drawRectangle(headCenterX, bodyCenterY, bodyW, bodyH);

    // ===== FACE FEATURES =====
    float eyeW = 0.06f, eyeH = 0.03f;
    float eyeY = headCenterY + 0.05f;
    glBegin(GL_QUADS);
        // Left Eye
        glVertex2f(-0.1f, eyeY + eyeH / 2);
        glVertex2f(-0.1f + eyeW, eyeY + eyeH / 2);
        glVertex2f(-0.1f + eyeW, eyeY - eyeH / 2);
        glVertex2f(-0.1f, eyeY - eyeH / 2);
        // Right Eye
        glVertex2f(0.1f - eyeW, eyeY + eyeH / 2);
        glVertex2f(0.1f, eyeY + eyeH / 2);
        glVertex2f(0.1f, eyeY - eyeH / 2);
        glVertex2f(0.1f - eyeW, eyeY - eyeH / 2);
    glEnd();

    // Nose (vertical line)
    glBegin(GL_LINES);
        glVertex2f(0.0f, headCenterY + 0.02f);
        glVertex2f(0.0f, headCenterY - 0.05f);
    glEnd();

    // Mouth (thicker horizontal line)
    glLineWidth(2.5f);
    glBegin(GL_LINES);
        glVertex2f(-0.08f, headCenterY - 0.1f);
        glVertex2f(0.08f, headCenterY - 0.1f);
    glEnd();
    glLineWidth(1.0f); // Reset

    // ===== ARMS =====
    float shoulderY = headCenterY - headH / 2;
    float shoulderXLeft = -bodyW / 2;
    float shoulderXRight = bodyW / 2;
    float armLength = 0.35f;

    glBegin(GL_LINES);
        // Left arm
        glVertex2f(shoulderXLeft, shoulderY);
        glVertex2f(shoulderXLeft - armLength, shoulderY - armLength);

        // Right arm
        glVertex2f(shoulderXRight, shoulderY);
        glVertex2f(shoulderXRight + armLength, shoulderY - armLength);
    glEnd();

    // ===== LEGS =====
    float hipY = bodyCenterY - bodyH / 2;
    float legLength = 0.35f;

    glBegin(GL_LINES);
        // Left leg
        glVertex2f(-0.05f, hipY);
        glVertex2f(-0.05f, hipY - legLength);

        // Right leg
        glVertex2f(0.05f, hipY);
        glVertex2f(0.05f, hipY - legLength);
    glEnd();

    glFlush(); // Render now
}

// Setup canvas background and projection
void setupCanvas() {
    glClearColor(0.99f, 0.97f, 0.94f, 1.0f); // background
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("CG Lab: Custom Stickman Shape with OpenGL");
    setupCanvas();
    glutDisplayFunc(drawStickman);
    glutMainLoop();
    return 0;
}
