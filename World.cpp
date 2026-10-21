// World.cpp — lightweight low-poly first-person city prototype.
// Dependencies: GLFW 3, OpenGL compatibility profile, GLU.
// Windows (MinGW, installed GLFW): g++ World.cpp -std=c++17 -O2 -o World.exe -lglfw3 -lopengl32 -lglu32
// Linux: g++ World.cpp -std=c++17 -O2 -o world -lglfw -lGL -lGLU
//
// Controls: WASD move, mouse look, Shift sprint, Space jump, Esc release cursor/exit.
// No player mesh is rendered: the camera is the player's eyes.

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
#include <vector>

namespace {
constexpr double PI = 3.14159265358979323846;
constexpr double WORLD_LIMIT = 92.0;
constexpr double ROAD_SPACING = 24.0;
constexpr double ROAD_HALF = 4.2;
constexpr double PLAYER_RADIUS = 0.34;
constexpr double EYE_HEIGHT = 1.68;

struct Building {
    double x, z, w, d, h;
    double r, g, b;
    int floors;
};
struct Tree { double x, z, scale; };

std::vector<Building> buildings;
std::vector<Tree> trees;
GLFWwindow* window = nullptr;
GLUquadric* quadric = nullptr;

double px = 0.0, pz = 13.0;
double yaw = -90.0, pitch = 0.0;
double velocityY = 0.0;
bool grounded = true;
bool firstMouse = true;
double lastX = 0.0, lastY = 0.0;
bool showHelp = true;

double radians(double degrees) { return degrees * PI / 180.0; }

void color(double r, double g, double b) { glColor3d(r, g, b); }

void cube(double x, double y, double z, double sx, double sy, double sz,
          double r, double g, double b) {
    const double x0=x-sx/2, x1=x+sx/2, y0=y-sy/2, y1=y+sy/2;
    const double z0=z-sz/2, z1=z+sz/2;
    glBegin(GL_QUADS);
    color(r*0.78,g*0.78,b*0.78); glNormal3d(0,0,1);
    glVertex3d(x0,y0,z1); glVertex3d(x1,y0,z1); glVertex3d(x1,y1,z1); glVertex3d(x0,y1,z1);
    color(r*0.65,g*0.65,b*0.65); glNormal3d(0,0,-1);
    glVertex3d(x1,y0,z0); glVertex3d(x0,y0,z0); glVertex3d(x0,y1,z0); glVertex3d(x1,y1,z0);
    color(r*0.92,g*0.92,b*0.92); glNormal3d(1,0,0);
    glVertex3d(x1,y0,z1); glVertex3d(x1,y0,z0); glVertex3d(x1,y1,z0); glVertex3d(x1,y1,z1);
    color(r*0.72,g*0.72,b*0.72); glNormal3d(-1,0,0);
    glVertex3d(x0,y0,z0); glVertex3d(x0,y0,z1); glVertex3d(x0,y1,z1); glVertex3d(x0,y1,z0);
    color(r,g,b); glNormal3d(0,1,0);
    glVertex3d(x0,y1,z1); glVertex3d(x1,y1,z1); glVertex3d(x1,y1,z0); glVertex3d(x0,y1,z0);
    color(r*0.58,g*0.58,b*0.58); glNormal3d(0,-1,0);
    glVertex3d(x0,y0,z0); glVertex3d(x1,y0,z0); glVertex3d(x1,y0,z1); glVertex3d(x0,y0,z1);
    glEnd();
}

void flatRect(double x1,double y1,double z1,double x2,double y2,double z2,
              double x3,double y3,double z3,double x4,double y4,double z4,
              double r,double g,double b) {
    color(r,g,b);
    glBegin(GL_QUADS);
    glVertex3d(x1,y1,z1); glVertex3d(x2,y2,z2);
    glVertex3d(x3,y3,z3); glVertex3d(x4,y4,z4);
    glEnd();
}

bool onRoad(double x, double z) {
    const double rx = std::remainder(x, ROAD_SPACING);
    const double rz = std::remainder(z, ROAD_SPACING);
    return std::abs(rx) < ROAD_HALF || std::abs(rz) < ROAD_HALF;
}

void makeCity() {
    // Buildings are deliberately spaced away from the street corridors.
    const double palette[][3] = {
        {0.72,0.65,0.54}, {0.62,0.69,0.72}, {0.76,0.73,0.64},
        {0.57,0.65,0.60}, {0.78,0.61,0.48}, {0.66,0.62,0.73},
        {0.83,0.78,0.65}
    };
    // Place each building in the middle of a city block, never on an asphalt lane.
    // Road centers are multiples of ROAD_SPACING; block centers sit halfway between them.
    for (int gx=-3; gx<=2; ++gx) {
        for (int gz=-3; gz<=2; ++gz) {
            // Keep the central block open as a small park.
            if (gx==0 && gz==0) continue;
            const double bx = (gx+0.5)*ROAD_SPACING + (((gx+gz)&1) ? 0.8 : -0.8);
            const double bz = (gz+0.5)*ROAD_SPACING + (((gx-gz)&1) ? -0.8 : 0.8);
            const double w = 7.2 + ((gx*gx+gz*gz)%3)*1.0;
            const double d = 7.0 + ((std::abs(gx+gz)+2)%3)*0.9;
            const int floors = 2 + ((std::abs(gx*7+gz*3)+1)%5);
            const double h = floors*2.75;
            const int p = (std::abs(gx*11+gz*5)+gx*gx+gz*gz)%7;
            buildings.push_back({bx,bz,w,d,h,palette[p][0],palette[p][1],palette[p][2],floors});
        }
    }
    // Small park in the central block, plus trees along the streets.
    for (int x=-3; x<=3; ++x)
        for (int z=-3; z<=3; ++z)
            if ((x*x+z*z)%3 != 1)
                trees.push_back({x*2.15+10.0,z*2.15+10.0,0.8+((x+z+8)%3)*0.12});
    for (int k=-3; k<=3; ++k) {
        for (int side : {-1,1}) {
            trees.push_back({k*ROAD_SPACING+side*5.9, -WORLD_LIMIT*0.65, 0.9});
            trees.push_back({-WORLD_LIMIT*0.65, k*ROAD_SPACING+side*5.9, 0.85});
        }
    }
}

void drawRoads() {
    // Grass base.
    cube(0,-0.22,0,WORLD_LIMIT*2,0.4,WORLD_LIMIT*2,0.36,0.52,0.31);
    // A grid of asphalt roads and simple sidewalks.
    for (int i=-3; i<=3; ++i) {
        const double c=i*ROAD_SPACING;
        // Asphalt strips. Their overlap at intersections is intentional.
        cube(c,0.005,0,ROAD_HALF*2,0.03,WORLD_LIMIT*2,0.20,0.22,0.23);
        cube(0,0.006,c,WORLD_LIMIT*2,0.03,ROAD_HALF*2,0.20,0.22,0.23);

        // Sidewalks are built as separate block-length segments. Leaving gaps at
        // every crossing prevents pavement from covering the intersecting road.
        for (int side : {-1,1}) {
            const double edge = c+side*(ROAD_HALF+0.75);
            for (int block=-4; block<=3; ++block) {
                const double start=block*ROAD_SPACING+ROAD_HALF;
                const double end=(block+1)*ROAD_SPACING-ROAD_HALF;
                if (end<=-WORLD_LIMIT || start>=WORLD_LIMIT) continue;
                const double z0=std::max(start,-WORLD_LIMIT);
                const double z1=std::min(end,WORLD_LIMIT);
                if (z1>z0)
                    cube(edge,0.035,(z0+z1)/2,1.15,0.10,z1-z0,0.62,0.61,0.56);

                const double x0=std::max(start,-WORLD_LIMIT);
                const double x1=std::min(end,WORLD_LIMIT);
                if (x1>x0)
                    cube((x0+x1)/2,0.035,edge,x1-x0,0.10,1.15,0.62,0.61,0.56);
            }
        }
        // Dashed center markings.
        for (int p=-7; p<=7; ++p) {
            cube(c,0.027,p*11.0,0.12,0.015,4.1,0.83,0.80,0.66);
            cube(p*11.0,0.028,c,4.1,0.015,0.12,0.83,0.80,0.66);
        }
    }
}

void drawBuilding(const Building& b) {
    cube(b.x,b.h/2,b.z,b.w,b.h,b.d,b.r,b.g,b.b);
    // Flat roof rim, kept very low-poly.
    cube(b.x,b.h+0.10,b.z,b.w+0.25,0.20,b.d+0.25,b.r*0.75,b.g*0.75,b.b*0.75);
    // Repeated dark windows on each visible street-facing side.
    const double winW=0.72, winH=1.05, spacing=1.35;
    for (int floor=0; floor<b.floors; ++floor) {
        const double wy=1.0+floor*2.75;
        if (wy+winH > b.h-0.25) continue;
        for (double off=-b.w/2+0.85; off<b.w/2-0.35; off+=spacing) {
            const double wx=b.x+off;
            flatRect(wx-winW/2,wy,b.z+b.d/2+0.008, wx+winW/2,wy,b.z+b.d/2+0.008,
                     wx+winW/2,wy+winH,b.z+b.d/2+0.008, wx-winW/2,wy+winH,b.z+b.d/2+0.008,
                     0.20,0.31,0.37);
        }
        for (double off=-b.d/2+0.85; off<b.d/2-0.35; off+=spacing) {
            const double wz=b.z+off;
            flatRect(b.x+b.w/2+0.008,wy,wz-winW/2, b.x+b.w/2+0.008,wy,wz+winW/2,
                     b.x+b.w/2+0.008,wy+winH,wz+winW/2, b.x+b.w/2+0.008,wy+winH,wz-winW/2,
                     0.18,0.29,0.34);
        }
    }
    // Shopfront at ground level on the street side.
    flatRect(b.x-0.9,0.22,b.z+b.d/2+0.012, b.x+0.9,0.22,b.z+b.d/2+0.012,
             b.x+0.9,1.7,b.z+b.d/2+0.012, b.x-0.9,1.7,b.z+b.d/2+0.012,
             0.24,0.34,0.37);
}

void drawTree(const Tree& t) {
    const double h=2.4*t.scale;
    glPushMatrix();
    glTranslated(t.x,0,t.z);
    // GLU cylinders grow along local +Z; rotate that axis to world +Y so trees stand upright.
    glRotated(-90.0,1.0,0.0,0.0);
    color(0.34,0.24,0.15);
    gluCylinder(quadric,0.16*t.scale,0.11*t.scale,h*0.52,5,1);
    glTranslated(0,0,h*0.35);
    color(0.25,0.43,0.25);
    gluCylinder(quadric,0.0,0.95*t.scale,h*0.72,6,1);
    glTranslated(0,0,h*0.42);
    color(0.34,0.55,0.30);
    gluCylinder(quadric,0.0,0.66*t.scale,h*0.58,6,1);
    glPopMatrix();
}

void drawStreetFurniture() {
    for (int i=-3;i<=3;++i) {
        for (int side : {-1,1}) {
            const double c=i*ROAD_SPACING+side*(ROAD_HALF+1.25);
            // Keep lamp posts on the sidewalk, not in the middle of a crossing.
            for (int segment=-3; segment<=3; ++segment) {
                const double along=segment*ROAD_SPACING+12.0;
                if (std::abs(along)>WORLD_LIMIT-4) continue;
                cube(c,1.8,along,0.10,3.6,0.10,0.24,0.25,0.24);
                cube(c,3.55,along+0.3,0.55,0.12,0.45,0.88,0.79,0.48);
                cube(along,1.8,c,0.10,3.6,0.10,0.24,0.25,0.24);
                cube(along+0.3,3.55,c,0.45,0.12,0.55,0.88,0.79,0.48);
            }
        }
    }
}

bool collides(double x,double z) {
    if (std::abs(x)>WORLD_LIMIT-2 || std::abs(z)>WORLD_LIMIT-2) return true;
    for (const Building& b: buildings) {
        const double hx=b.w/2+PLAYER_RADIUS, hz=b.d/2+PLAYER_RADIUS;
        if (std::abs(x-b.x)<hx && std::abs(z-b.z)<hz) return true;
    }
    return false;
}

void mouseCallback(GLFWwindow*, double x, double y) {
    if (firstMouse) { lastX=x; lastY=y; firstMouse=false; }
    const double dx=x-lastX, dy=lastY-y;
    lastX=x; lastY=y;
    const double sensitivity=0.095;
    yaw += dx*sensitivity;
    pitch = std::clamp(pitch+dy*sensitivity,-85.0,85.0);
}

void keyCallback(GLFWwindow* w,int key,int,int action,int) {
    if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS)
        glfwSetWindowShouldClose(w,GL_TRUE);
    if (key==GLFW_KEY_F1 && action==GLFW_PRESS)
        showHelp=!showHelp;
}

void update(double dt) {
    dt=std::min(dt,0.05);
    const double yr=radians(yaw);
    const double forwardX=std::cos(yr), forwardZ=std::sin(yr);
    const double rightX=-forwardZ, rightZ=forwardX;
    double dx=0,dz=0;
    if (glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS) { dx+=forwardX; dz+=forwardZ; }
    if (glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS) { dx-=forwardX; dz-=forwardZ; }
    if (glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS) { dx+=rightX; dz+=rightZ; }
    if (glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS) { dx-=rightX; dz-=rightZ; }
    const double mag=std::sqrt(dx*dx+dz*dz);
    if (mag>0.001) {
        const double speed=glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS ? 8.2 : 4.5;
        dx=dx/mag*speed*dt; dz=dz/mag*speed*dt;
        if (!collides(px+dx,pz)) px+=dx;
        if (!collides(px,pz+dz)) pz+=dz;
    }
    if (glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS && grounded) {
        velocityY=5.0; grounded=false;
    }
    velocityY-=14.0*dt;
    double nextY=velocityY*dt;
    if (!grounded || velocityY>0) {
        static double jumpHeight=0.0;
        jumpHeight += nextY;
        if (jumpHeight<=0.0) { jumpHeight=0.0; velocityY=0.0; grounded=true; }
        else { grounded=false; }
        // The offset is stored in the camera height, with landing at zero.
        // Keep a separate persistent value to avoid accumulating camera drift.
    }
}

double jumpOffset=0.0;
void updatePlayer(double dt) {
    dt=std::min(dt,0.05);
    const double yr=radians(yaw);
    const double fx=std::cos(yr), fz=std::sin(yr);
    const double rx=-fz, rz=fx;
    double dx=0,dz=0;
    if (glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS) { dx+=fx; dz+=fz; }
    if (glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS) { dx-=fx; dz-=fz; }
    if (glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS) { dx+=rx; dz+=rz; }
    if (glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS) { dx-=rx; dz-=rz; }
    const double mag=std::sqrt(dx*dx+dz*dz);
    if (mag>0.001) {
        const double speed=glfwGetKey(window,GLFW_KEY_LEFT_SHIFT)==GLFW_PRESS ? 8.2 : 4.5;
        dx=dx/mag*speed*dt; dz=dz/mag*speed*dt;
        if (!collides(px+dx,pz)) px+=dx;
        if (!collides(px,pz+dz)) pz+=dz;
    }
    if (glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS && grounded) {
        velocityY=5.0; grounded=false;
    }
    velocityY-=14.0*dt;
    jumpOffset+=velocityY*dt;
    if (jumpOffset<=0.0) { jumpOffset=0.0; velocityY=0.0; grounded=true; }
}

void renderScene(int width,int height) {
    glViewport(0,0,width,height);
    glClearColor(0.54f,0.72f,0.86f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(72.0,(height>0 ? (double)width/height : 1.0),0.08,240.0);
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    const double yr=radians(yaw), pr=radians(pitch);
    const double lookX=std::cos(yr)*std::cos(pr);
    const double lookY=std::sin(pr);
    const double lookZ=std::sin(yr)*std::cos(pr);
    gluLookAt(px,EYE_HEIGHT+jumpOffset,pz,
              px+lookX,EYE_HEIGHT+jumpOffset+lookY,pz+lookZ,0,1,0);

    // Soft daylight; low-poly objects use flat shading.
    glEnable(GL_LIGHTING); glEnable(GL_LIGHT0);
    const GLfloat ambient[]={0.60f,0.60f,0.60f,1.0f};
    const GLfloat diffuse[]={0.82f,0.82f,0.78f,1.0f};
    const GLfloat position[]={-0.35f,1.0f,0.25f,0.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT,ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE,diffuse);
    glLightfv(GL_LIGHT0,GL_POSITION,position);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glShadeModel(GL_FLAT);

    drawRoads();
    for (const Building& b:buildings) drawBuilding(b);
    for (const Tree& t:trees) drawTree(t);
    drawStreetFurniture();
    glDisable(GL_LIGHTING);

    // Small crosshair makes first-person orientation clear.
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    glOrtho(0,width,height,0,-1,1);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glColor3d(1.0,1.0,1.0);
    glBegin(GL_LINES);
    glVertex2d(width/2-7,height/2); glVertex2d(width/2+7,height/2);
    glVertex2d(width/2,height/2-7); glVertex2d(width/2,height/2+7);
    glEnd();
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW); glPopMatrix();
    glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawHelpText() {
    // GLFW/OpenGL prototype intentionally avoids a font dependency.
    glfwSetWindowTitle(window, showHelp
        ? "Low-poly City | WASD move | Mouse look | Shift sprint | Space jump | F1 hide help | Esc quit"
        : "Low-poly City | WASD move | Mouse look | Shift sprint | Space jump | F1 help | Esc quit");
}
} // namespace

int main() {
    makeCity();
    if (!glfwInit()) {
        std::fprintf(stderr,"Failed to initialize GLFW. Install GLFW 3 and OpenGL/GLU.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,1);
    window=glfwCreateWindow(1280,720,"Low-poly City",nullptr,nullptr);
    if (!window) {
        std::fprintf(stderr,"Could not create an OpenGL window.\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetCursorPosCallback(window,mouseCallback);
    glfwSetKeyCallback(window,keyCallback);
    glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
    quadric=gluNewQuadric();
    if (quadric) gluQuadricNormals(quadric,GLU_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_NORMALIZE);

    double last=glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        const double now=glfwGetTime();
        updatePlayer(now-last); last=now;
        int width,height; glfwGetFramebufferSize(window,&width,&height);
        renderScene(width,height);
        drawHelpText();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    if (quadric) gluDeleteQuadric(quadric);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
