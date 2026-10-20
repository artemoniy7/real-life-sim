// HumanViewer.cpp — standalone OpenGL human model viewer.
// Build (Windows, with GLFW + OpenGL/GLU installed):
//   g++ HumanViewer.cpp -o HumanViewer.exe -lglfw3 -lopengl32 -lglu32
// Build (Linux):
//   g++ HumanViewer.cpp -o human_viewer -lglfw -lGL -lGLU
// Uses the compatibility OpenGL pipeline for a dependency-light procedural model.

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#ifdef _WIN32
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <algorithm>
#include <cmath>
#include <cstdio>

namespace {
constexpr double PI = 3.14159265358979323846;
GLUquadric* quadric = nullptr;
double yaw = 24.0, pitch = 10.0, distance = 5.8;
double lastX = 0.0, lastY = 0.0;
bool dragging = false;
bool wireframe = false;

void sphere(double x, double y, double z, double sx, double sy, double sz) {
    glPushMatrix();
    glTranslated(x, y, z);
    glScaled(sx, sy, sz);
    gluSphere(quadric, 1.0, 28, 20);
    glPopMatrix();
}

void capsuleBetween(const double a[3], const double b[3], double radiusA, double radiusB) {
    const double dx=b[0]-a[0], dy=b[1]-a[1], dz=b[2]-a[2];
    const double length=std::sqrt(dx*dx+dy*dy+dz*dz);
    if (length < 1e-5) return;
    glPushMatrix();
    glTranslated(a[0],a[1],a[2]);
    const double angle=std::acos(std::clamp(dz/length,-1.0,1.0))*180.0/PI;
    glRotated(angle,-dy,dx,0.0);
    gluCylinder(quadric,radiusA,radiusB,length,20,4);
    gluDisk(quadric,0.0,radiusA,20,1);
    glTranslated(0,0,length);
    gluDisk(quadric,0.0,radiusB,20,1);
    glPopMatrix();
}

void segment(double ax,double ay,double az,double bx,double by,double bz,double ra,double rb) {\n    const double a[3]={ax,ay,az}, b[3]={bx,by,bz};\n    capsuleBetween(a,b,ra,rb);\n}\n\nvoid limb(const double hip[3], const double knee[3], const double ankle[3],
          double upperRadius, double lowerRadius) {
    sphere(hip[0],hip[1],hip[2],upperRadius,upperRadius,upperRadius);
    capsuleBetween(hip,knee,upperRadius*0.94,upperRadius*0.72);
    sphere(knee[0],knee[1],knee[2],upperRadius*0.73,upperRadius*0.73,upperRadius*0.73);
    capsuleBetween(knee,ankle,lowerRadius*0.78,lowerRadius*0.52);
    sphere(ankle[0],ankle[1],ankle[2],lowerRadius*0.55,lowerRadius*0.55,lowerRadius*0.55);
}

void drawHuman() {
    // All geometry is matte white, with anatomically proportioned overlapping forms.
    glColor3d(0.94,0.95,0.97);
    // Shoes and feet
    sphere(-0.13,0.10,0.12,0.16,0.10,0.27);
    sphere( 0.13,0.10,0.12,0.16,0.10,0.27);
    // Legs: hip, knee, ankle (Z is vertical)
    const double lh[3]={-0.145,0.0,1.02}, lk[3]={-0.16,0.015,0.57}, la[3]={-0.15,0.0,0.16};
    const double rh[3]={ 0.145,0.0,1.02}, rk[3]={ 0.16,0.025,0.57}, ra[3]={ 0.15,0.0,0.16};
    limb(lh,lk,la,0.125,0.085); limb(rh,rk,ra,0.125,0.085);
    // Pelvis and torso
    sphere(0,0,1.05,0.255,0.155,0.19);
    sphere(0,0,1.31,0.245,0.145,0.30);       // abdomen
    sphere(0,0,1.57,0.32,0.18,0.26);         // chest
    sphere(0,0,1.72,0.335,0.17,0.16);        // shoulders / upper chest
    // Neck and head
    segment(0,0,1.73, 0,0,1.88, 0.085,0.075);
    sphere(0,0,2.04,0.145,0.135,0.205);      // skull
    sphere(0,0.105,1.99,0.105,0.07,0.105);   // face plane / jaw
    // Ears
    sphere(-0.148,0,2.02,0.027,0.035,0.05);
    sphere( 0.148,0,2.02,0.027,0.035,0.05);
    // Arms, slightly away from the torso
    const double ls[3]={-0.32,0,1.67}, le[3]={-0.43,0.005,1.35}, lw[3]={-0.45,0.035,1.08};
    const double rs[3]={ 0.32,0,1.67}, re[3]={ 0.43,0.005,1.35}, rw[3]={ 0.45,0.035,1.08};
    sphere(ls[0],ls[1],ls[2],0.115,0.12,0.12);
    sphere(rs[0],rs[1],rs[2],0.115,0.12,0.12);
    limb(ls,le,lw,0.085,0.065); limb(rs,re,rw,0.085,0.065);
    sphere(-0.45,0.04,1.00,0.065,0.055,0.10);
    sphere( 0.45,0.04,1.00,0.065,0.055,0.10);
    // Simple hands and fingers
    for (int side : {-1,1}) {
        const double x=side*0.45;
        for(int i=0;i<4;++i)
            capsuleBetween((double[3]){x+side*(i-1.5)*0.022,0.045,0.97},
                           (double[3]){x+side*(i-1.5)*0.026,0.045,0.90},0.012,0.009);
        capsuleBetween((double[3]){x-side*0.045,0.045,0.995},
                       (double[3]){x-side*0.075,0.045,0.95},0.015,0.010);
    }
}

void drawGrid() {
    glDisable(GL_LIGHTING);
    glColor3d(0.24,0.26,0.29);
    glBegin(GL_LINES);
    for(int i=-10;i<=10;++i) {
        glVertex3d(i,-10,0); glVertex3d(i,10,0);
        glVertex3d(-10,i,0); glVertex3d(10,i,0);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void framebufferSize(GLFWwindow*, int width, int height) {
    if(height < 1) height=1;
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(45.0,static_cast<double>(width)/height,0.05,100.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouseButton(GLFWwindow* w, int button, int action, int) {
    if(button==GLFW_MOUSE_BUTTON_LEFT) {
        dragging=(action==GLFW_PRESS);
        glfwGetCursorPos(w,&lastX,&lastY);
    }
}
void cursorPos(GLFWwindow* w, double x, double y) {
    if(dragging) {
        yaw += (x-lastX)*0.45;
        pitch = std::clamp(pitch+(y-lastY)*0.35,-75.0,75.0);
    }
    lastX=x; lastY=y;
    (void)w;
}
void scroll(GLFWwindow*, double, double y) {
    distance=std::clamp(distance-y*0.35,2.4,12.0);
}
void key(GLFWwindow* w, int key, int, int action, int) {
    if(action!=GLFW_PRESS) return;
    if(key==GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(w,GLFW_TRUE);
    if(key==GLFW_KEY_R) { yaw=24; pitch=10; distance=5.8; }
    if(key==GLFW_KEY_F) wireframe=!wireframe;
}
}

int main() {
    if(!glfwInit()) {
        std::fprintf(stderr,"GLFW initialization failed. Install GLFW and OpenGL development libraries.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    GLFWwindow* window=glfwCreateWindow(1100,760,"REAL LIFE SIM | White Human Viewer",nullptr,nullptr);
    if(!window) {
        std::fprintf(stderr,"Could not create OpenGL window. Check graphics drivers / GLFW installation.\n");
        glfwTerminate(); return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window,framebufferSize);
    glfwSetMouseButtonCallback(window,mouseButton);
    glfwSetCursorPosCallback(window,cursorPos);
    glfwSetScrollCallback(window,scroll);
    glfwSetKeyCallback(window,key);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    const GLfloat lightPos[]={-3.0f,-4.0f,7.0f,1.0f};
    const GLfloat ambient[]={0.32f,0.32f,0.35f,1.0f};
    const GLfloat diffuse[]={0.92f,0.92f,0.95f,1.0f};
    glLightfv(GL_LIGHT0,GL_POSITION,lightPos);
    glLightfv(GL_LIGHT0,GL_AMBIENT,ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    const GLfloat matSpec[]={0.12f,0.12f,0.12f,1.0f};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,matSpec);
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,18.0f);
    glClearColor(0.055f,0.065f,0.085f,1.0f);
    quadric=gluNewQuadric();
    gluQuadricNormals(quadric,GLU_SMOOTH);

    while(!glfwWindowShouldClose(window)) {
        int width,height; glfwGetFramebufferSize(window,&width,&height);
        framebufferSize(window,width,height);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW); glLoadIdentity();
        const double yr=yaw*PI/180.0, pr=pitch*PI/180.0;
        const double targetZ=1.02;
        const double cx=distance*std::cos(pr)*std::sin(yr);
        const double cy=-distance*std::cos(pr)*std::cos(yr);
        const double cz=targetZ+distance*std::sin(pr);
        gluLookAt(cx,cy,cz,0,0,targetZ,0,0,1);

        drawGrid();
        glPushMatrix();
        glPolygonMode(GL_FRONT_AND_BACK,wireframe?GL_LINE:GL_FILL);
        drawHuman();
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
        glPopMatrix();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    gluDeleteQuadric(quadric);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
