/*
  3D HUMAN ANATOMY VISUALIZATION
  Computer Graphics mini-project using C++ and OpenGL (FreeGLUT + GLU)

  Controls:
    1 = Show complete body
    2 = Show skeleton
    3 = Show organs
    B = Toggle body surface
    S = Toggle skeleton overlay
    O = Toggle organs
    Arrow keys = Rotate model
    + / - = Zoom in / out
    R = Reset view
    Left-click a body region = Show its description
    ESC = Exit

  Build (MinGW example):
    g++ main.cpp -o Anatomy3D.exe -lfreeglut -lopengl32 -lglu32
*/

#include <windows.h>
#include <GL/freeglut.h>
#include <GL/glu.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

enum PartId { PART_NONE=0, PART_HEAD, PART_CHEST, PART_ABDOMEN,
              PART_LEFT_ARM, PART_RIGHT_ARM, PART_LEFT_LEG, PART_RIGHT_LEG };

float rotX = 8.0f, rotY = -15.0f, zoomValue = 1.0f;
bool showBody = true, showSkeleton = false, showOrgans = true;
int winW = 1000, winH = 750;
int selectedPart = PART_NONE;
GLUquadric* quad = nullptr;

struct PartInfo { int id; const char* name; const char* detail; };
PartInfo parts[] = {
    {PART_HEAD, "Head / Skull", "The skull protects the brain. The brain controls movement, senses, memory and many body functions."},
    {PART_CHEST, "Chest / Thorax", "The chest contains the heart and lungs. The heart pumps blood; the lungs exchange oxygen and carbon dioxide."},
    {PART_ABDOMEN, "Abdomen", "The abdomen contains organs such as the stomach, liver, kidneys and intestines. They help digestion and other vital functions."},
    {PART_LEFT_ARM, "Left arm", "The arm contains the humerus, radius and ulna bones, along with muscles that help lift, push and pull."},
    {PART_RIGHT_ARM, "Right arm", "The arm contains the humerus, radius and ulna bones, along with muscles that help lift, push and pull."},
    {PART_LEFT_LEG, "Left leg", "The leg contains the femur, tibia and fibula. These bones support body weight and enable walking and running."},
    {PART_RIGHT_LEG, "Right leg", "The leg contains the femur, tibia and fibula. These bones support body weight and enable walking and running."}
};

void setMaterial(float r, float g, float b, float shine=25.0f) {
    GLfloat color[] = {r,g,b,1.0f};
    GLfloat spec[] = {0.35f,0.35f,0.35f,1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shine);
}
void sphere(float x,float y,float z,float sx,float sy,float sz) {
    glPushMatrix(); glTranslatef(x,y,z); glScalef(sx,sy,sz);
    glutSolidSphere(1.0,28,22); glPopMatrix();
}
void line3(float x1,float y1,float z1,float x2,float y2,float z2,float width=5) {
    glLineWidth(width); glBegin(GL_LINES);
    glVertex3f(x1,y1,z1); glVertex3f(x2,y2,z2); glEnd();
}
void cylinderBetween(float x1,float y1,float z1,float x2,float y2,float z2,float radius) {
    float dx=x2-x1, dy=y2-y1, dz=z2-z1;
    float len=sqrt(dx*dx+dy*dy+dz*dz);
    glPushMatrix(); glTranslatef(x1,y1,z1);
    float ax=-dy, ay=dx, az=0;
    float d=sqrt(dx*dx+dy*dy);
    if (d < 0.0001f) { ax=1; ay=0; az=0; }
    float angle=acosf(dy/len)*180.0f/(float)M_PI;
    glRotatef(angle, ax, ay, az);
    gluCylinder(quad,radius,radius,len,16,2);
    glPopMatrix();
}

void drawBody() {
    setMaterial(0.78f,0.57f,0.43f,18);
    // Head, neck and torso
    sphere(0,2.55f,0,0.48f,0.58f,0.43f);
    sphere(0,1.88f,0,0.19f,0.28f,0.19f);
    sphere(0,0.75f,0,0.82f,1.18f,0.42f);
    sphere(0,-0.45f,0,0.62f,0.62f,0.38f);
    // Arms
    sphere(-0.98f,1.35f,0,0.25f,0.32f,0.25f);
    sphere(-1.18f,0.72f,0,0.20f,0.65f,0.20f);
    sphere(-1.25f,0.05f,0,0.18f,0.58f,0.18f);
    sphere(0.98f,1.35f,0,0.25f,0.32f,0.25f);
    sphere(1.18f,0.72f,0,0.20f,0.65f,0.20f);
    sphere(1.25f,0.05f,0,0.18f,0.58f,0.18f);
    // Legs and feet
    sphere(-0.34f,-1.45f,0,0.27f,0.95f,0.28f);
    sphere(-0.34f,-2.42f,0,0.21f,0.82f,0.22f);
    sphere(-0.34f,-3.18f,0.13f,0.28f,0.16f,0.48f);
    sphere(0.34f,-1.45f,0,0.27f,0.95f,0.28f);
    sphere(0.34f,-2.42f,0,0.21f,0.82f,0.22f);
    sphere(0.34f,-3.18f,0.13f,0.28f,0.16f,0.48f);
}

void drawSkeleton() {
    setMaterial(0.91f,0.88f,0.73f,12);
    // Skull and spine
    sphere(0,2.55f,0,0.39f,0.48f,0.35f);
    line3(0,2.02f,0,0,0.0f,0,8);
    // Rib cage
    for(int i=0;i<5;i++) {
        float y=1.55f-i*0.25f;
        float w=0.42f-i*0.025f;
        line3(0,y,0,-w,y-0.12f,0,3);
        line3(0,y,0,w,y-0.12f,0,3);
        line3(-w,y-0.12f,0,w,y-0.12f,0,2);
    }
    // Pelvis
    line3(-0.48f,-0.35f,0,0.48f,-0.35f,0,7);
    line3(-0.48f,-0.35f,0,-0.28f,-0.72f,0,6);
    line3(0.48f,-0.35f,0,0.28f,-0.72f,0,6);
    // Arms and legs
    cylinderBetween(-0.48f,1.55f,0,-1.15f,0.72f,0,0.075f);
    cylinderBetween(-1.15f,0.72f,0,-1.28f,-0.48f,0,0.055f);
    cylinderBetween(0.48f,1.55f,0,1.15f,0.72f,0,0.075f);
    cylinderBetween(1.15f,0.72f,0,1.28f,-0.48f,0,0.055f);
    cylinderBetween(-0.28f,-0.62f,0,-0.36f,-2.0f,0,0.11f);
    cylinderBetween(-0.36f,-2.0f,0,-0.36f,-3.1f,0,0.075f);
    cylinderBetween(0.28f,-0.62f,0,0.36f,-2.0f,0,0.11f);
    cylinderBetween(0.36f,-2.0f,0,0.36f,-3.1f,0,0.075f);
    // Shoulder and joint markers
    sphere(-0.48f,1.55f,0,0.12f,0.12f,0.12f);
    sphere(0.48f,1.55f,0,0.12f,0.12f,0.12f);
    sphere(-0.36f,-2.0f,0,0.12f,0.12f,0.12f);
    sphere(0.36f,-2.0f,0,0.12f,0.12f,0.12f);
}

void drawOrgans() {
    // Organs are simplified educational models, not medical-grade anatomy.
    // Heart
    setMaterial(0.78f,0.08f,0.12f,45);
    sphere(-0.18f,1.12f,0.43f,0.22f,0.30f,0.16f);
    sphere(0.02f,1.12f,0.43f,0.22f,0.30f,0.16f);
    // Lungs
    setMaterial(0.78f,0.30f,0.38f,20);
    sphere(-0.42f,1.05f,0.35f,0.28f,0.48f,0.16f);
    sphere(0.42f,1.05f,0.35f,0.28f,0.48f,0.16f);
    // Liver
    setMaterial(0.43f,0.18f,0.08f,20);
    sphere(-0.30f,0.15f,0.40f,0.38f,0.20f,0.14f);
    // Stomach
    setMaterial(0.88f,0.55f,0.18f,20);
    sphere(0.20f,0.05f,0.42f,0.23f,0.34f,0.14f);
    // Kidneys
    setMaterial(0.55f,0.12f,0.18f,20);
    sphere(-0.48f,-0.05f,0.22f,0.13f,0.22f,0.12f);
    sphere(0.48f,-0.05f,0.22f,0.13f,0.22f,0.12f);
    // Intestines (stylized loops)
    setMaterial(0.88f,0.65f,0.35f,10);
    for(int i=0;i<3;i++) {
        glPushMatrix(); glTranslatef(0,-0.52f+i*0.13f,0.43f);
        glScalef(0.30f,0.07f,0.08f); glutWireTorus(0.35,0.55,8,24); glPopMatrix();
    }
    // Brain visible inside head
    setMaterial(0.95f,0.55f,0.62f,25);
    sphere(0,2.60f,0.28f,0.29f,0.30f,0.16f);
}

void drawGroundAndStars() {
    glDisable(GL_LIGHTING);
    glPointSize(2.0f); glColor3f(0.75f,0.82f,0.95f);
    glBegin(GL_POINTS);
    for(int i=0;i<180;i++) {
        float x=fmodf(i*37.17f,18.0f)-9.0f;
        float y=fmodf(i*19.31f,12.0f)-6.0f;
        float z=-4.0f;
        glVertex3f(x,y,z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawText(float x,float y,const char* s) {
    glRasterPos2f(x,y);
    for(const char* p=s;*p;p++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*p);
}
void drawOverlay() {
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0,winW,0,winH);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_LIGHTING); glColor3f(0.92f,0.95f,1.0f);
    drawText(20,winH-30,"3D HUMAN ANATOMY VISUALIZATION");
    glColor3f(0.65f,0.82f,1.0f);
    drawText(20,winH-56,"1: Body   2: Skeleton   3: Organs   B/S/O: Toggle layers   Arrows: Rotate   +/-: Zoom");
    char mode[160];
    sprintf(mode,"Layers: Body %s | Skeleton %s | Organs %s",
        showBody?"ON":"OFF",showSkeleton?"ON":"OFF",showOrgans?"ON":"OFF");
    drawText(20,winH-82,mode);
    if(selectedPart!=PART_NONE) {
        for(const auto& p:parts) if(p.id==selectedPart) {
            glColor3f(1.0f,0.83f,0.35f);
            drawText(20,70,p.name);
            glColor3f(0.92f,0.95f,1.0f);
            // Split description into short lines for the overlay.
            std::string d=p.detail; int line=0;
            while(!d.empty()) {
                size_t n=d.find(' ',70);
                if(n==std::string::npos) n=d.size();
                std::string chunk=d.substr(0,n);
                glRasterPos2f(20,45-line*20);
                for(char c:chunk) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12,c);
                if(n==d.size()) break;
                d.erase(0,n+1); line++;
            }
            break;
        }
    } else {
        glColor3f(0.9f,0.9f,0.9f); drawText(20,35,"Click a body region to learn about it.");
    }
    glEnable(GL_LIGHTING);
    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix(); glMatrixMode(GL_MODELVIEW);
}

void drawScene(bool picking=false) {
    if(showBody) drawBody();
    if(showSkeleton) drawSkeleton();
    if(showOrgans) drawOrgans();

    if(picking) {
        // Named hit targets are drawn as simple spheres in OpenGL selection mode.
        glInitNames();
        glPushName(PART_HEAD); sphere(0,2.55f,0,0.52f,0.62f,0.48f); glPopName();
        glPushName(PART_CHEST); sphere(0,1.05f,0,0.75f,0.75f,0.55f); glPopName();
        glPushName(PART_ABDOMEN); sphere(0,-0.15f,0,0.65f,0.65f,0.48f); glPopName();
        glPushName(PART_LEFT_ARM); sphere(-1.18f,0.65f,0,0.28f,0.95f,0.28f); glPopName();
        glPushName(PART_RIGHT_ARM); sphere(1.18f,0.65f,0,0.28f,0.95f,0.28f); glPopName();
        glPushName(PART_LEFT_LEG); sphere(-0.35f,-2.15f,0,0.32f,1.25f,0.32f); glPopName();
        glPushName(PART_RIGHT_LEG); sphere(0.35f,-2.15f,0,0.32f,1.25f,0.32f); glPopName();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    gluLookAt(0,0,11, 0,0,0, 0,1,0);
    drawGroundAndStars();
    glPushMatrix();
    glScalef(zoomValue,zoomValue,zoomValue);
    glRotatef(rotX,1,0,0); glRotatef(rotY,0,1,0);
    drawScene(false);
    glPopMatrix();
    drawOverlay();
    glutSwapBuffers();
}

void reshape(int w,int h) {
    winW=w; winH=h>0?h:1;
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(45.0,(double)w/(double)h,0.1,100.0);
    glMatrixMode(GL_MODELVIEW);
}

int performPick(int x,int y) {
    GLuint buffer[512];
    GLint hits;
    glSelectBuffer(512,buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    GLint viewport[4]; glGetIntegerv(GL_VIEWPORT,viewport);
    gluPickMatrix((GLdouble)x,(GLdouble)(viewport[3]-y),8.0,8.0,viewport);
    gluPerspective(45.0,(double)winW/(double)winH,0.1,100.0);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    gluLookAt(0,0,11,0,0,0,0,1,0);
    glScalef(zoomValue,zoomValue,zoomValue);
    glRotatef(rotX,1,0,0); glRotatef(rotY,0,1,0);
    drawScene(true);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    hits=glRenderMode(GL_RENDER);
    if(hits<=0) return PART_NONE;
    GLuint* ptr=buffer; GLuint nearest=0xFFFFFFFF; int picked=PART_NONE;
    for(int i=0;i<hits;i++) {
        GLuint n=*ptr++; GLuint zmin=*ptr++; ptr++; // skip max depth
        if(zmin<nearest && n>0) { nearest=zmin; picked=(int)ptr[n-1]; }
        ptr+=n;
    }
    return picked;
}

void mouse(int button,int state,int x,int y) {
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN) {
        selectedPart=performPick(x,y);
        glutPostRedisplay();
    }
}
void keyboard(unsigned char key,int,int) {
    switch(key) {
        case '1': showBody=true; showSkeleton=false; showOrgans=false; break;
        case '2': showBody=false; showSkeleton=true; showOrgans=false; break;
        case '3': showBody=false; showSkeleton=false; showOrgans=true; break;
        case 'b': case 'B': showBody=!showBody; break;
        case 's': case 'S': showSkeleton=!showSkeleton; break;
        case 'o': case 'O': showOrgans=!showOrgans; break;
        case '+': case '=': zoomValue*=1.1f; break;
        case '-': case '_': zoomValue/=1.1f; break;
        case 'r': case 'R': rotX=8; rotY=-15; zoomValue=1; selectedPart=PART_NONE; break;
        case 27: std::exit(0);
    }
    if(zoomValue<0.45f) zoomValue=0.45f;
    if(zoomValue>2.5f) zoomValue=2.5f;
    glutPostRedisplay();
}
void specialKeys(int key,int,int) {
    if(key==GLUT_KEY_LEFT) rotY-=5;
    if(key==GLUT_KEY_RIGHT) rotY+=5;
    if(key==GLUT_KEY_UP) rotX-=5;
    if(key==GLUT_KEY_DOWN) rotX+=5;
    glutPostRedisplay();
}

void initGL() {
    glClearColor(0.015f,0.025f,0.07f,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    GLfloat pos[]={-4,6,8,1};
    GLfloat ambient[]={0.25f,0.25f,0.30f,1};
    GLfloat diffuse[]={0.95f,0.92f,0.86f,1};
    glLightfv(GL_LIGHT0,GL_POSITION,pos);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);
    quad=gluNewQuadric();
}

int main(int argc,char** argv) {
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(winW,winH);
    glutInitWindowPosition(100,60);
    glutCreateWindow("3D Human Anatomy Visualization - OpenGL");
    initGL();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
