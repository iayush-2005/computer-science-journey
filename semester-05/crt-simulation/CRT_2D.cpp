// CRT_2D.cpp
// 2D CRT Simulation

// Build (Ubuntu):
//   sudo apt-get install build-essential freeglut3-dev
//   g++ CRT_2D.cpp -o CRT_2D -lGL -lGLU -lglut -std=c++11
//   ./CRT_2D
// ------------------------------------------------------------------------------

#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

// ------------------------------- Configuration --------------------------------
static const int WIN_W = 900;
static const int WIN_H = 600;

// Ortho world coords
static const float worldLeft = 0.0f;
static const float worldRight = 450.0f;
static const float worldBottom = 0.0f;
static const float worldTop = 500.0f;

// Screen/CRT geometry
static const float filamentY = 270.0f;
static const float filamentX0 = 107.0f;
static const float anodeX0 = 120.0f;
static const float anodeX1 = 185.0f;
static const float platesX = 237.0f;
static const float screenCX = 370.0f;
static const float screenCY = 270.0f;
static const float screenA = 40.0f;
static const float screenB = 130.0f;

// Animation timing
static const int TIMER_MS = 16;        // ~60 FPS
static const float beamSpeed = 0.02f;  // stage progress
static const int INTRO_AUTO_MS = 6000; // 6 sec auto start

// Viewport inset size - REDUCED to prevent overlap
static const int INSET_PIX = 150;
static const int INSET_MARGIN = 10;

// ------------------------------- Project Info ---------------------------------
const char* PROJECT_TITLE = "SIMULATION OF CATHODE RAY TUBE (CRT)";
const char* COURSE_NAME = "Computer Graphics Lab";
const char* COURSE_CODE = "18B17Cl575";
const char* MEMBERS[] = {
    "Ayush Thakur (231030143)",
    "Krish Monga (231030134)",
    "Shashwat Bhardwaj (231030135)",
    "Imon Chaudhary (231030136)"
};
const int MEMBER_COUNT = 4;

// ------------------------------- State ----------------------------------------
enum BeamStage { STAGE_FILAMENT = 0, STAGE_ANODE, STAGE_DEFLECTION, STAGE_SCREEN };
bool showIntro = true;
bool paused = true;
BeamStage beamStage = STAGE_FILAMENT;
float beamProgress = 0.0f;
float screenTheta = 0.0f;

// Beam position
float beamX = filamentX0, beamY = filamentY;

// Intro state
float introAlpha = 0.0f;
float introSlide = -40.0f;
bool introEffectsDone = false;
bool typingDone = false;
int typingIndex = 0;
int lastTypingMs = 0;

// Particles
struct Particle {
    float t;
    float speed;
    float phase;
    float life;
};
std::vector<Particle> particles;
const int NUM_PARTICLES = 120;

// Path history
std::vector<std::pair<float,float>> pathHistory;
const int PATH_HISTORY_MAX = 800;

// ------------------------------- Utilities -----------------------------------

// Precise text centering using GLUT bitmap length
static void drawStringCentered(float cx, float y, const std::string &s, void* font = GLUT_BITMAP_TIMES_ROMAN_24) {
    if (s.empty()) return;
    int lenPix = glutBitmapLength(font, (const unsigned char*)s.c_str());
    float worldRatio = (worldRight - worldLeft) / (float)WIN_W;
    float worldLen = lenPix * worldRatio;
    glRasterPos2f(cx - worldLen * 0.5f, y);
    for (char c : s) glutBitmapCharacter(font, c);
}

static void drawString(float x, float y, const std::string &s, void* font = GLUT_BITMAP_HELVETICA_12) {
    glRasterPos2f(x, y);
    for (char c : s) glutBitmapCharacter(font, c);
}

static float frand(float a=0.0f, float b=1.0f) {
    return a + (b-a) * (rand() / (float)RAND_MAX);
}

// ------------------------------- Init ---------------------------------------
static void initGL() {
    srand((unsigned)time(nullptr));
    glClearColor(0.94f, 0.96f, 1.0f, 1.0f);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    particles.clear();
    for (int i = 0; i < NUM_PARTICLES; ++i) {
        Particle p;
        p.t = frand(-0.5f, 1.0f);
        p.speed = 0.004f + frand(0.0f, 0.012f);
        p.phase = frand(0.0f, 6.283f);
        particles.push_back(p);
    }
}

// ------------------------------- Background ----------------------------------
static void drawBackgroundGrid() {
    glLineWidth(1.0f);
    glColor4f(0.8f, 0.85f, 0.9f, 0.5f);

    glBegin(GL_LINES);
    for(float x = worldLeft; x <= worldRight; x+=25.0f) {
        glVertex2f(x, worldBottom); glVertex2f(x, worldTop);
    }
    for(float y = worldBottom; y <= worldTop; y+=25.0f) {
        glVertex2f(worldLeft, y); glVertex2f(worldRight, y);
    }
    glEnd();
}

// ------------------------------- CRT Drawing ---------------------------------
static void drawLabelWithArrow(float lx, float ly, float tx, float ty, const std::string& text) {
    glColor3f(0.1f, 0.1f, 0.1f);
    drawString(lx, ly, text, GLUT_BITMAP_HELVETICA_12);

    glColor4f(0.4f, 0.4f, 0.4f, 0.6f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(lx + 10, ly - 4);
        glVertex2f(tx, ty);
    glEnd();
}

static void drawCRTStructure() {
    // 1. Outer Body
    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
        glVertex2f(70,320); glVertex2f(240,320);
        glVertex2f(70,220); glVertex2f(240,220);
        glVertex2f(70,210); glVertex2f(70,330);
        glVertex2f(240,320); glVertex2f(370,400);
        glVertex2f(240,220); glVertex2f(370,140);
        glVertex2f(45,210); glVertex2f(45,330);
        glVertex2f(45,210); glVertex2f(70,210);
        glVertex2f(45,330); glVertex2f(70,330);
    glEnd();

    // Pins
    glColor3f(0.2f, 0.2f, 0.2f);
    int pinY[] = {225,245,267,290,310};
    for (int i=0;i<5;i++) {
        glBegin(GL_POLYGON);
            glVertex2f(35, pinY[i]); glVertex2f(45, pinY[i]);
            glVertex2f(45, pinY[i]+5); glVertex2f(35, pinY[i]+5);
        glEnd();
    }

    // 2. Filament
    glColor3f(0.8f, 0.2f, 0.2f);
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex2f(70, 292); glVertex2f(115, 292);
        glVertex2f(70, 248); glVertex2f(115, 248);
        glVertex2f(115, 248); glVertex2f(115, 292);
        glVertex2f(70, 272); glVertex2f(95, 272);
        glVertex2f(70, 267); glVertex2f(95, 267);
    glEnd();
    glColor3f(0.2f, 0.2f, 0.2f);
    glBegin(GL_POLYGON);
        glVertex2f(95, 267); glVertex2f(108, 267);
        glVertex2f(108, 272); glVertex2f(95, 272);
    glEnd();
    drawLabelWithArrow(60, 340, 95, 292, "Electron Gun (Cathode)");

    // 3. Anodes
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
        glVertex2f(120, 248); glVertex2f(120, 292);
        glVertex2f(120, 248); glVertex2f(180, 248);
        glVertex2f(120, 292); glVertex2f(180, 292);
    glEnd();
    for (int xpos = 130; xpos <= 170; xpos += 10) {
        glBegin(GL_LINES);
            glVertex2f(xpos, 248); glVertex2f(xpos, 265);
            glVertex2f(xpos, 274); glVertex2f(xpos, 292);
        glEnd();
    }
    glBegin(GL_LINES);
        glVertex2f(185, 244); glVertex2f(185, 296);
        glVertex2f(175, 244); glVertex2f(184, 244);
        glVertex2f(175, 296); glVertex2f(184, 296);
    glEnd();
    drawLabelWithArrow(130, 315, 150, 292, "Focusing Anodes");

    // 4. Deflection Plates
    glColor3f(0.3f,0.3f,0.3f);
    glBegin(GL_POLYGON);
        glVertex2f(190, 280); glVertex2f(205, 280);
        glVertex2f(215, 289); glVertex2f(200, 289);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(190, 250); glVertex2f(205, 250);
        glVertex2f(215, 259); glVertex2f(200, 259);
    glEnd();

    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_POLYGON);
        glVertex2f(230, 261); glVertex2f(245, 261);
        glVertex2f(245, 285); glVertex2f(230, 285);
    glEnd();
    glColor3f(0.4f,0.4f,0.4f);
    glBegin(GL_POLYGON);
        glVertex2f(220, 258); glVertex2f(235, 258);
        glVertex2f(235, 282); glVertex2f(220, 282);
    glEnd();
    drawLabelWithArrow(190, 340, 205, 289, "Y-Deflection Plates");
    drawLabelWithArrow(220, 200, 235, 258, "X-Deflection Plates");

    // 5. Screen
    glPointSize(2);
    glColor3f(0.1f,0.1f,0.1f);
    glBegin(GL_POINTS);
    for (float a=0;a<=360.0f;a+=0.5f) {
        float t = a * 3.14159f/180.0f;
        float px = screenCX + screenA * std::cos(t);
        float py = screenCY + screenB * std::sin(t);
        glVertex2f(px, py);
    }
    glEnd();

    glColor4f(0.2f, 0.8f, 0.2f, 0.1f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(screenCX, screenCY);
    for (float a=0;a<=360.0f;a+=5.0f) {
        float t = a * 3.14159f/180.0f;
        glVertex2f(screenCX + screenA*std::cos(t), screenCY + screenB*std::sin(t));
    }
    glEnd();

    drawString(screenCX+20, screenCY+screenB-20, "Phosphor Screen", GLUT_BITMAP_HELVETICA_12);
}

// ------------------------------- Beam Logic -----------------------------------

static void drawBeamSpreadRegion() {
    float startX = 245.0f;
    float startY = 270.0f;
    float endX = screenCX;
    float endY_Top = screenCY + screenB - 10;
    float endY_Bot = screenCY - screenB + 10;

    glLineWidth(1.0f);
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xAAAA);
    glColor4f(0.6f, 0.6f, 0.6f, 0.5f);

    glBegin(GL_LINES);
        glVertex2f(startX, startY); glVertex2f(endX, endY_Top);
        glVertex2f(startX, startY); glVertex2f(endX, endY_Bot);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    if (beamStage == STAGE_SCREEN || beamStage == STAGE_DEFLECTION) {
        glColor4f(0.0f, 0.5f, 1.0f, 0.05f);
        glBegin(GL_TRIANGLES);
            glVertex2f(startX, startY);
            glVertex2f(endX, endY_Top);
            glVertex2f(endX, endY_Bot);
        glEnd();
        glColor3f(0.4f, 0.4f, 0.6f);
        drawString(280, 420, "Deflection Region", GLUT_BITMAP_HELVETICA_10);
    }
}

static void computeBeamTip() {
    if (beamStage == STAGE_FILAMENT) {
        beamX = filamentX0 + (anodeX0 - filamentX0) * beamProgress;
        beamY = filamentY;
    } else if (beamStage == STAGE_ANODE) {
        beamX = anodeX0 + (anodeX1 - anodeX0) * beamProgress;
        beamY = filamentY;
    } else if (beamStage == STAGE_DEFLECTION) {
        beamX = anodeX1 + (platesX - anodeX1) * beamProgress;
        beamY = filamentY;
    } else { // STAGE_SCREEN
        float tx = screenCX + screenA * std::cos(screenTheta);
        float ty = screenCY + screenB * std::sin(screenTheta);
        beamX = platesX + (tx - platesX) * beamProgress;
        beamY = filamentY + (ty - filamentY) * beamProgress;
    }
}

static void drawBeamAndParticles(float timeMs) {
    float tx = screenCX + screenA * std::cos(screenTheta);
    float ty = screenCY + screenB * std::sin(screenTheta);

    glPointSize(3.0f);
    for (int i = 0; i < (int)particles.size(); ++i) {
        Particle &p = particles[i];
        p.t += p.speed;
        if (p.t > 1.1f) p.t = -0.1f;

        float curX, curY;
        float t_mapped = p.t;

        float px_start = filamentX0;
        float px_mid = platesX;
        float px_end = tx;

        if (t_mapped < 0.3f) {
            float sub_t = t_mapped / 0.3f;
            curX = px_start + (px_mid - px_start) * sub_t;
            curY = filamentY;
        } else {
            float sub_t = (t_mapped - 0.3f) / 0.7f;
            curX = px_mid + (px_end - px_mid) * sub_t;
            float targetY = ty;
            curY = filamentY + (targetY - filamentY) * sub_t;

            float wobble = 5.0f * sin(p.phase + timeMs * 0.005f);
            curY += wobble * (1.0f - sub_t);
        }

        float alpha = 0.8f;
        if (p.t < 0.0f || p.t > 1.0f) alpha = 0.0f;
        glColor4f(0.2f, 0.2f, 0.9f, alpha);

        glBegin(GL_POINTS); glVertex2f(curX, curY); glEnd();
    }

    computeBeamTip();
    glColor3f(0.0f, 0.0f, 1.0f);
    glPointSize(6.0f);
    glBegin(GL_POINTS); glVertex2f(beamX, beamY); glEnd();
}

// ------------------------------- Viewport Inset ------------------------------
static void drawInsetViewport() {
    // 1. Get DYNAMIC window dimensions
    // This ensures that if the user maximizes, we use the real edges
    int current_W = glutGet(GLUT_WINDOW_WIDTH);
    int current_H = glutGet(GLUT_WINDOW_HEIGHT);

    // 2. Position: Bottom Right (Dynamic)
    // px is distance from left edge.
    // py is distance from bottom edge.
    int px = current_W - INSET_PIX - INSET_MARGIN;
    int py = INSET_MARGIN;

    // 3. Set the Viewport
    glPushAttrib(GL_VIEWPORT_BIT);
    glViewport(px, py, INSET_PIX, INSET_PIX);

    // 4. Draw the Box
    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();

    // White background
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
      glVertex2f(0,0); glVertex2f(1,0); glVertex2f(1,1); glVertex2f(0,1);
    glEnd();

    // Border
    glLineWidth(2);
    glColor3f(0.0f, 0.2f, 0.6f);
    glBegin(GL_LINE_LOOP);
      glVertex2f(0.01f,0.01f); glVertex2f(0.99f,0.01f); glVertex2f(0.99f,0.99f); glVertex2f(0.01f,0.99f);
    glEnd();

    // Map world coords to inset
    float Wx0 = 230.0f, Wx1 = 420.0f;
    float Wy0 = 130.0f, Wy1 = 410.0f;
    auto mapX = [&](float wx)->float { return (wx - Wx0) / (Wx1 - Wx0); };
    auto mapY = [&](float wy)->float { return (wy - Wy0) / (Wy1 - Wy0); };

    glLineWidth(1.5f);
    int total = (int)pathHistory.size();
    glBegin(GL_LINE_STRIP);
      for (int i=0; i<total; i++) {
          float alpha = (float)i / total;
          glColor4f(0.0f, 0.5f, 1.0f, alpha);
          float mx = mapX(pathHistory[i].first);
          float my = mapY(pathHistory[i].second);
          if (mx >= 0 && mx <= 1 && my >= 0 && my <= 1)
            glVertex2f(mx, my);
      }
    glEnd();

    glColor3f(0.8f, 0.8f, 0.8f);
    glBegin(GL_LINES);
      glVertex2f(0, 0.5f); glVertex2f(1, 0.5f);
      glVertex2f(0.5f, 0); glVertex2f(0.5f, 1);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(0.05f, 0.92f);
    std::string title = "Trace Path";
    for(char c: title) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

// ------------------------------- Intro ---------------------------------------
static void displayIntro(float timeMs) {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawBackgroundGrid();

    float a = introAlpha;
    if (a > 1.0f) a = 1.0f;

    float slide = introSlide;
    float cx = (worldLeft + worldRight) * 0.5f;
    float startY = 380.0f + slide;

    glColor4f(0.0f, 0.1f, 0.4f, a);
    drawStringCentered(cx, startY, PROJECT_TITLE, GLUT_BITMAP_TIMES_ROMAN_24);

    glColor4f(0.2f, 0.2f, 0.2f, a);
    drawStringCentered(cx, startY - 40, COURSE_NAME, GLUT_BITMAP_HELVETICA_18);
    drawStringCentered(cx, startY - 65, std::string("Course Code: ") + COURSE_CODE, GLUT_BITMAP_HELVETICA_18);

    glColor4f(0.0f, 0.0f, 0.0f, a);
    drawStringCentered(cx, startY - 110, "Group Members:", GLUT_BITMAP_HELVETICA_18);

    float my = startY - 140;
    for(int i=0; i<MEMBER_COUNT; i++) {
        glColor4f(0.1f, 0.1f, 0.1f, a);
        drawStringCentered(cx, my, MEMBERS[i], GLUT_BITMAP_HELVETICA_18);
        my -= 25.0f;
    }

    glColor4f(0.5f, 0.0f, 0.0f, a);
    drawStringCentered(cx, 80, "Press [S] to Start Simulation", GLUT_BITMAP_HELVETICA_18);
}

// ------------------------------- Main Display --------------------------------

static void setupOrtho() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(worldLeft, worldRight, worldBottom, worldTop);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

static void display() {
    int timeMs = glutGet(GLUT_ELAPSED_TIME);

    if (showIntro) {
        setupOrtho();
        displayIntro((float)timeMs);
        glutSwapBuffers();
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);

    setupOrtho();

    drawBackgroundGrid();
    drawBeamSpreadRegion();
    drawCRTStructure();
    drawBeamAndParticles((float)timeMs);

    pathHistory.emplace_back(beamX, beamY);
    if (pathHistory.size() > PATH_HISTORY_MAX) pathHistory.erase(pathHistory.begin());

    drawInsetViewport();

    glColor3f(0.4f, 0.4f, 0.4f);
    drawString(10, 15, "Controls: [S] Start/Skip  [P] Pause  [R] Reset  [Esc] Exit", GLUT_BITMAP_HELVETICA_12);

    glutSwapBuffers();
}

// ------------------------------- Reshape Handler ------------------------------
static void reshape(int w, int h) {
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    float worldAspect = (worldRight - worldLeft) / (worldTop - worldBottom);
    float windowAspect = (float)w / (float)h;

    if (windowAspect > worldAspect) {
        float newWidth = (worldTop - worldBottom) * windowAspect;
        float center = (worldLeft + worldRight) / 2.0f;
        gluOrtho2D(center - newWidth/2.0f, center + newWidth/2.0f, worldBottom, worldTop);
    } else {
        float newHeight = (worldRight - worldLeft) / windowAspect;
        float center = (worldBottom + worldTop) / 2.0f;
        gluOrtho2D(worldLeft, worldRight, center - newHeight/2.0f, center + newHeight/2.0f);
    }

    glMatrixMode(GL_MODELVIEW);
}

// ------------------------------- Timer and Input ------------------------------

static void timerFunc(int) {
    if (showIntro && !introEffectsDone) {
        introAlpha += 0.015f;
        introSlide += 0.8f;
        if (introAlpha >= 1.0f) introAlpha = 1.0f;
        if (introSlide >= 0.0f) introSlide = 0.0f;
        if (introAlpha >= 1.0f && introSlide >= 0.0f) introEffectsDone = true;
    }

    if (!showIntro && !paused) {
        beamProgress += beamSpeed;
        if (beamProgress > 1.0f) beamProgress = 1.0f;

        if (beamStage == STAGE_SCREEN) {
            screenTheta += 0.02f;
            if (screenTheta > 6.283f) screenTheta -= 6.283f;
            if (beamProgress >= 1.0f) beamProgress = 0.0f;
        } else {
            if (beamProgress >= 1.0f) {
                beamProgress = 0.0f;
                beamStage = (BeamStage)(beamStage + 1);
            }
        }
    }
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, timerFunc, 0);
}

// ------------------------------- Input --------------------------------------
static void keyboardHandler(unsigned char key, int, int) {
    switch (key) {
        case 27: std::exit(0); break;
        case 's': case 'S': showIntro = false; paused = false; break;
        case 'p': case 'P': paused = !paused; break;
        case 'r': case 'R':
            paused = true;
            beamStage = STAGE_FILAMENT;
            beamProgress = 0.0f;
            pathHistory.clear();
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(WIN_W, WIN_H);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("CRT Simulation");

    initGL();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboardHandler);
    glutTimerFunc(TIMER_MS, timerFunc, 0);

    glutMainLoop();
    return 0;
}