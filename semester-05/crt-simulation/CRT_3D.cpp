// CRT_3D_Labeled.cpp
//
// Build (Ubuntu):
//  g++ CRT_3D_Labeled.cpp -o CRT_3D -lGL -lGLU -lglut -std=c++11
//   ./CRT_3D
// ------------------------------------------------------------------------------

#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

// --------------------------- Configuration ------------------------------------
static const int WIN_W = 1000;
static const int WIN_H = 700;

// Camera System
float camTheta = 45.0f;   // Horizontal angle
float camPhi = 20.0f;     // Vertical angle
float camDist = 28.0f;    // Zoom (Slightly further back to see labels)
int lastMouseX = -1, lastMouseY = -1;
bool isDragging = false;

// CRT Geometry constants
const float SCREEN_W = 4.0f;
const float SCREEN_H = 3.0f;

// Animation State
enum BeamStage { STAGE_FILAMENT = 0, STAGE_ANODE, STAGE_DEFLECTION, STAGE_SCREEN };
bool showIntro = true;
bool paused = true;
BeamStage beamStage = STAGE_FILAMENT;
float beamProgress = 0.0f;
float screenTheta = 0.0f;

// Beam Data
float beamTipX = 0, beamTipY = 0, beamTipZ = 0;

// Intro Text State
float introAlpha = 0.0f;
bool introEffectsDone = false;

// 2D Trace History
std::vector<std::pair<float,float>> traceHistory;

// Particles
struct Particle {
    float t;
    float speed;
    float offsetR;
    float offsetA;
};
std::vector<Particle> particles;
const int NUM_PARTICLES = 150;

// --------------------------- Project Info -------------------------------------
const char* PROJECT_TITLE = "SIMULATION OF CATHODE RAY TUBE (3D)";
const char* COURSE_NAME = "Computer Graphics Lab";
const char* COURSE_CODE = "18B17Cl575";
const char* MEMBERS[] = {
    "Ayush Thakur (231030143)",
    "Krish Monga (231030134)",
    "Shashwat Bhardwaj (231030135)",
    "Imon Chaudhary (231030136)"
};
const int MEMBER_COUNT = 4;

// --------------------------- Utilities ----------------------------------------

static float frand(float a=0.0f, float b=1.0f) {
    return a + (b-a) * (rand() / (float)RAND_MAX);
}

static void drawStringCentered(float cx, float y, const std::string &s, void* font = GLUT_BITMAP_TIMES_ROMAN_24) {
    if (s.empty()) return;
    int lenPix = glutBitmapLength(font, (const unsigned char*)s.c_str());
    glRasterPos2f(cx - lenPix/2.0f, y);
    for (char c : s) glutBitmapCharacter(font, c);
}

static void drawString(float x, float y, const std::string &s) {
    glRasterPos2f(x, y);
    for (char c : s) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}

// Draw a Label with a "Leader Line" pointing to the object
static void drawLabelWithLeader(float textX, float textY, float textZ,
                                float targetX, float targetY, float targetZ,
                                const std::string &text) {

    // 1. Draw the Leader Line
    glColor4f(0.4f, 0.4f, 0.4f, 0.6f); // Grey
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex3f(textX, textY - 0.2f, textZ); // Start slightly below text
        glVertex3f(targetX, targetY, targetZ);  // End at object
    glEnd();

    // 2. Draw the Text
    glColor3f(0.0f, 0.0f, 0.0f); // Black Text
    glRasterPos3f(textX, textY, textZ);
    for (char c : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}

// --------------------------- Initialization -----------------------------------
void init() {
    srand(time(0));
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // White Background

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    for(int i=0; i<NUM_PARTICLES; i++) {
        Particle p;
        p.t = frand(-0.5f, 1.0f);
        p.speed = 0.005f + frand(0.0f, 0.01f);
        p.offsetR = frand(0.0f, 0.05f);
        p.offsetA = frand(0.0f, 6.28f);
        particles.push_back(p);
    }
}

// --------------------------- Drawing Helpers ----------------------------------

void drawTechBox(float w, float h, float d, float r, float g, float b) {
    glPushMatrix();
    glScalef(w, h, d);
    glColor4f(r, g, b, 0.8f);
    glutSolidCube(1.0f);
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5f);
    glutWireCube(1.002f);
    glPopMatrix();
}

void drawCylinderPart(float baseR, float topR, float height, float r, float g, float b, float alpha) {
    GLUquadric* quad = gluNewQuadric();
    glColor4f(r, g, b, alpha);
    gluQuadricDrawStyle(quad, GLU_FILL);
    gluCylinder(quad, baseR, topR, height, 32, 1);
    glColor4f(0.2f, 0.2f, 0.2f, 0.5f);
    gluQuadricDrawStyle(quad, GLU_LINE);
    gluCylinder(quad, baseR*1.001, topR*1.001, height, 16, 1);
    gluDeleteQuadric(quad);
}

// --------------------------- 3D Scene -----------------------------------------

void drawCRT() {
    // --- 1. ELECTRON GUN SECTION ---

    // Heater / Filament (Base)
    glPushMatrix();
    glTranslatef(0, 0, -0.5f);
    drawTechBox(0.4f, 0.4f, 0.5f, 0.8f, 0.4f, 0.1f); // Orange
    glPopMatrix();
    drawLabelWithLeader(0.0f, -2.0f, -0.5f, 0.0f, -0.2f, -0.5f, "Heater / Filament");

    // Cathode (Source of electrons)
    glPushMatrix();
    glTranslatef(0, 0, 0.5f);
    drawTechBox(0.8f, 0.8f, 1.0f, 0.8f, 0.3f, 0.3f); // Reddish
    glPopMatrix();
    drawLabelWithLeader(-2.5f, 1.5f, 0.5f, -0.4f, 0.4f, 0.5f, "Cathode (Negative Potential)");

    // --- 2. FOCUSING SECTION ---

    // Anodes (Focusing & Accelerating)
    glPushMatrix();
    glTranslatef(0, 0, 2.5f);
    drawCylinderPart(0.5f, 0.5f, 2.0f, 0.7f, 0.7f, 0.7f, 0.9f); // Grey Cylinder
    glPopMatrix();
    drawLabelWithLeader(2.0f, 2.0f, 3.5f, 0.5f, 0.5f, 3.5f, "Focusing & Accelerating Anodes");

    // --- 3. DEFLECTION SYSTEM ---

    // Y-Deflection Plates (Horizontal plates moving beam vertically)
    glPushMatrix();
    glTranslatef(0, 0, 5.5f);
    // Top Plate
    glPushMatrix(); glTranslatef(0, 0.6f, 0); drawTechBox(1.0f, 0.1f, 1.5f, 0.4f, 0.4f, 0.4f); glPopMatrix();
    // Bottom Plate
    glPushMatrix(); glTranslatef(0, -0.6f, 0); drawTechBox(1.0f, 0.1f, 1.5f, 0.4f, 0.4f, 0.4f); glPopMatrix();
    glPopMatrix();

    drawLabelWithLeader(2.5f, 1.0f, 5.5f, 0.5f, 0.6f, 5.5f, "Vertical Deflection Plates (Y-Plates)");

    // X-Deflection Plates (Vertical plates moving beam horizontally)
    glPushMatrix();
    glTranslatef(0, 0, 7.5f);
    // Left Plate
    glPushMatrix(); glTranslatef(-0.6f, 0, 0); drawTechBox(0.1f, 1.0f, 1.5f, 0.3f, 0.3f, 0.3f); glPopMatrix();
    // Right Plate
    glPushMatrix(); glTranslatef(0.6f, 0, 0); drawTechBox(0.1f, 1.0f, 1.5f, 0.3f, 0.3f, 0.3f); glPopMatrix();
    glPopMatrix();

    drawLabelWithLeader(-3.0f, -1.5f, 7.5f, -0.6f, 0.0f, 7.5f, "Horizontal Deflection Plates (X-Plates)");

    // --- 4. GLASS ENVELOPE ---

    glPushMatrix();
    glTranslatef(0, 0, 8.5f);
    // Glass funnel
    drawCylinderPart(0.6f, 2.5f, 4.0f, 0.8f, 0.9f, 1.0f, 0.15f); // Very transparent blue
    glPopMatrix();

    // --- 5. SCREEN ---

    glPushMatrix();
    glTranslatef(0, 0, 12.5f);
    glColor4f(0.9f, 0.95f, 0.9f, 0.9f); // Phosphor color
    glBegin(GL_QUADS);
        glVertex3f(-SCREEN_W/2, -SCREEN_H/2, 0);
        glVertex3f( SCREEN_W/2, -SCREEN_H/2, 0);
        glVertex3f( SCREEN_W/2,  SCREEN_H/2, 0);
        glVertex3f(-SCREEN_W/2,  SCREEN_H/2, 0);
    glEnd();

    // Screen Border
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
        glVertex3f(-SCREEN_W/2, -SCREEN_H/2, 0);
        glVertex3f( SCREEN_W/2, -SCREEN_H/2, 0);
        glVertex3f( SCREEN_W/2,  SCREEN_H/2, 0);
        glVertex3f(-SCREEN_W/2,  SCREEN_H/2, 0);
    glEnd();
    glPopMatrix();

    drawLabelWithLeader(0.0f, 2.5f, 12.5f, 0.0f, 1.5f, 12.5f, "Phosphor Coated Screen");
}

// --------------------------- Beam Logic ---------------------------------------

void calculateBeam() {
    float zEnd = 12.5f;
    float finalX = 0, finalY = 0;

    if (beamStage == STAGE_SCREEN || beamStage == STAGE_DEFLECTION) {
        finalX = (SCREEN_W/2.5f) * std::cos(screenTheta);
        finalY = (SCREEN_H/2.5f) * std::sin(screenTheta * 2.0f);
    }

    if (beamStage == STAGE_FILAMENT) {
        beamTipZ = 2.5f * beamProgress;
        beamTipX = 0; beamTipY = 0;
    }
    else if (beamStage == STAGE_ANODE) {
        beamTipZ = 2.5f + (5.5f - 2.5f) * beamProgress;
        beamTipX = 0; beamTipY = 0;
    }
    else if (beamStage == STAGE_DEFLECTION) {
        beamTipZ = 5.5f + (8.5f - 5.5f) * beamProgress;
        beamTipX = finalX * 0.1f * beamProgress;
        beamTipY = finalY * 0.1f * beamProgress;
    }
    else { // SCREEN
        beamTipZ = 8.5f + (12.5f - 8.5f) * beamProgress;
        float startX = finalX * 0.1f;
        float startY = finalY * 0.1f;
        beamTipX = startX + (finalX - startX) * beamProgress;
        beamTipY = startY + (finalY - startY) * beamProgress;
    }
}

void drawBeam() {
    // 1. Deflection Region (Envelope)
    if (beamStage == STAGE_SCREEN || beamStage == STAGE_DEFLECTION) {
        glPushMatrix();
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glColor4f(0.0f, 0.5f, 1.0f, 0.05f);
        glBegin(GL_TRIANGLE_FAN);
            glVertex3f(0, 0, 8.5f);
            glVertex3f(-SCREEN_W/2, -SCREEN_H/2, 12.5f);
            glVertex3f( SCREEN_W/2, -SCREEN_H/2, 12.5f);
            glVertex3f( SCREEN_W/2,  SCREEN_H/2, 12.5f);
            glVertex3f(-SCREEN_W/2,  SCREEN_H/2, 12.5f);
            glVertex3f(-SCREEN_W/2, -SCREEN_H/2, 12.5f);
        glEnd();
        glDepthMask(GL_TRUE);
        glPopMatrix();
    }

    // 2. Particles
    glPointSize(4.0f);
    glBegin(GL_POINTS);
    for(auto &p : particles) {
        p.t += p.speed;
        if(p.t > 1.0f) p.t = -0.2f;

        float tGlobal = p.t;
        float curZ = tGlobal * 12.5f;
        if (curZ > beamTipZ) continue;

        float curX = 0, curY = 0;
        float tx = (SCREEN_W/2.5f) * std::cos(screenTheta);
        float ty = (SCREEN_H/2.5f) * std::sin(screenTheta * 2.0f);

        if (curZ > 5.5f) {
             float factor = (curZ - 5.5f) / (12.5f - 5.5f);
             curX = tx * factor;
             curY = ty * factor;
        }

        float jitter = 0.05f;
        curX += cos(p.offsetA + curZ) * p.offsetR;
        curY += sin(p.offsetA + curZ) * p.offsetR;

        glColor4f(0.1f, 0.2f, 1.0f, 0.8f);
        glVertex3f(curX, curY, curZ);
    }
    glEnd();

    // 3. Beam Line
    glLineWidth(2.0f);
    glColor4f(0.0f, 0.0f, 1.0f, 0.6f);
    glBegin(GL_LINE_STRIP);
        glVertex3f(0,0,0);
        glVertex3f(0,0,5.5f);
        glVertex3f(beamTipX, beamTipY, beamTipZ);
    glEnd();

    // 4. Glow Spot
    glPointSize(8.0f);
    glColor3f(0.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
        glVertex3f(beamTipX, beamTipY, beamTipZ);
    glEnd();

    // Beam Label
    if(beamTipZ > 2.0f) {
        drawLabelWithLeader(2.0f, -2.0f, 4.0f, 0.0f, 0.0f, 4.0f, "Electron Beam");
    }
}

// --------------------------- 2D Overlay (HUD) ---------------------------------

void drawHUD() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    // Controls
    glColor3f(0.3f, 0.3f, 0.3f);
    drawString(20, 40, "Orbit: Left Mouse Drag  |  Zoom: Scroll");
    drawString(20, 20, "Controls: [S] Start/Skip  [P] Pause  [R] Reset");

    // Inset Trace
    int insetSize = 150;
    int margin = 20;
    int px = w - insetSize - margin;
    int py = margin;

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(px, py);
        glVertex2f(px + insetSize, py);
        glVertex2f(px + insetSize, py + insetSize);
        glVertex2f(px, py + insetSize);
    glEnd();

    glLineWidth(2.0f);
    glColor3f(0.1f, 0.2f, 0.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(px, py);
        glVertex2f(px + insetSize, py);
        glVertex2f(px + insetSize, py + insetSize);
        glVertex2f(px, py + insetSize);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    drawString(px + 5, py + insetSize - 15, "Screen Trace");

    float mapMin = -2.5f;
    float mapMax = 2.5f;

    glLineWidth(1.5f);
    glBegin(GL_LINE_STRIP);
    for(size_t i=0; i<traceHistory.size(); i++) {
        float alpha = (float)i / traceHistory.size();
        glColor4f(0.0f, 0.4f, 1.0f, alpha);

        float nx = (traceHistory[i].first - mapMin) / (mapMax - mapMin);
        float ny = (traceHistory[i].second - mapMin) / (mapMax - mapMin);

        glVertex2f(px + nx * insetSize, py + ny * insetSize);
    }
    glEnd();

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawIntro() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);

    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    float cx = w / 2.0f;
    float cy = h / 2.0f;

    glColor4f(0.9f, 0.9f, 0.95f, 1.0f);
    glBegin(GL_LINES);
    for(int i=0; i<w; i+=40) { glVertex2f(i,0); glVertex2f(i,h); }
    for(int i=0; i<h; i+=40) { glVertex2f(0,i); glVertex2f(w,i); }
    glEnd();

    float a = introAlpha;
    glColor4f(0.0f, 0.1f, 0.5f, a);
    drawStringCentered(cx, cy + 100, PROJECT_TITLE);

    glColor4f(0.2f, 0.2f, 0.2f, a);
    drawStringCentered(cx, cy + 60, COURSE_NAME, GLUT_BITMAP_HELVETICA_18);
    drawStringCentered(cx, cy + 35, std::string("Code: ") + COURSE_CODE, GLUT_BITMAP_HELVETICA_18);

    drawStringCentered(cx, cy - 20, "Group Members:", GLUT_BITMAP_HELVETICA_18);
    for(int i=0; i<MEMBER_COUNT; i++) {
        drawStringCentered(cx, cy - 50 - (i*25), MEMBERS[i], GLUT_BITMAP_HELVETICA_18);
    }

    glColor4f(0.6f, 0.0f, 0.0f, a);
    drawStringCentered(cx, 50, "Press [S] to Start Simulation", GLUT_BITMAP_HELVETICA_18);

    glEnable(GL_DEPTH_TEST);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// --------------------------- Rendering ----------------------------------------

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if(showIntro) {
        drawIntro();
    } else {
        glLoadIdentity();
        float radPhi = camPhi * 3.14159f / 180.0f;
        float radTheta = camTheta * 3.14159f / 180.0f;

        float eyeX = camDist * cos(radPhi) * sin(radTheta);
        float eyeY = camDist * sin(radPhi);
        float eyeZ = camDist * cos(radPhi) * cos(radTheta);

        gluLookAt(eyeX, eyeY, eyeZ + 6.0f,  0, 0, 6.0f,  0, 1, 0);

        drawCRT();
        drawBeam();
        drawHUD();
    }
    glutSwapBuffers();
}

void reshape(int w, int h) {
    if(h==0) h=1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w/h, 1.0f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

// --------------------------- Logic --------------------------------------------

void timer(int val) {
    if(showIntro && !introEffectsDone) {
        introAlpha += 0.02f;
        if(introAlpha >= 1.0f) { introAlpha = 1.0f; introEffectsDone = true; }
    }

    if(!showIntro && !paused) {
        calculateBeam();
        beamProgress += 0.015f;
        if(beamProgress > 1.0f) beamProgress = 1.0f;

        if(beamStage == STAGE_SCREEN) {
            screenTheta += 0.05f;
            if(screenTheta > 6.28f) screenTheta -= 6.28f;
            if(beamProgress >= 1.0f) beamProgress = 0.0f;
            traceHistory.push_back({beamTipX, beamTipY});
            if(traceHistory.size() > 500) traceHistory.erase(traceHistory.begin());
        } else {
            if(beamProgress >= 1.0f) {
                beamProgress = 0.0f;
                beamStage = (BeamStage)(beamStage + 1);
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// --------------------------- Input --------------------------------------------

void keyboard(unsigned char key, int x, int y) {
    if(key == 27) exit(0);
    if(key == 's' || key == 'S') { showIntro = false; paused = false; }
    if(key == 'p' || key == 'P') paused = !paused;
    if(key == 'r' || key == 'R') {
        paused = true;
        beamStage = STAGE_FILAMENT;
        beamProgress = 0.0f;
        traceHistory.clear();
    }
}

void mouse(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) { isDragging = true; lastMouseX = x; lastMouseY = y; }
        else isDragging = false;
    }
    if(button == 3) camDist -= 1.0f;
    if(button == 4) camDist += 1.0f;
    if(camDist < 5.0f) camDist = 5.0f;
    if(camDist > 60.0f) camDist = 60.0f;
}

void motion(int x, int y) {
    if(isDragging) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        camTheta -= dx * 0.5f;
        camPhi += dy * 0.5f;
        if(camPhi > 89.0f) camPhi = 89.0f;
        if(camPhi < -89.0f) camPhi = -89.0f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("CRT Simulation 3D");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(16, timer, 0);
    glutMainLoop();
    return 0;
}