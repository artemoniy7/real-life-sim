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

double px = -6.0, pz = -12.0;
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
    buildings.clear();
    trees.clear();
    const double palette[][3] = {
        {0.72,0.65,0.54}, {0.62,0.69,0.72}, {0.76,0.73,0.64},
        {0.57,0.65,0.60}, {0.78,0.61,0.48}, {0.66,0.62,0.73},
        {0.83,0.78,0.65}
    };
    auto addBuilding = [&](double x,double z,double w,double d,int floors,int p) {
        p=((p%7)+7)%7;
        buildings.push_back({x,z,w,d,floors*2.75,palette[p][0],palette[p][1],palette[p][2],floors});
    };

    // Primary neighborhood from the sketch. Coordinates are arranged around
    // three connected main roads and a compact local-street loop.
    // Main residential blocks: broad 10-storey apartment buildings.
    addBuilding(-34.0, 23.0, 12.0, 9.0, 10, 1);
    addBuilding(-16.0, 28.0, 13.0, 9.5, 10, 2);
    addBuilding(  3.0, 34.0, 12.0, 9.0, 10, 0);
    addBuilding( 22.0, 40.0, 12.0, 9.5, 10, 3);
    addBuilding(-20.0, -8.5, 12.0, 9.0, 10, 4);
    addBuilding( -2.0, -8.5, 13.0, 9.5, 10, 1);
    addBuilding( 17.0, -8.5, 12.0, 9.0, 10, 5);

    // Extra blocks fill the empty fields with a small mixed-use center,
    // a school/community building and a park, all outside the main carriageways.
    addBuilding(-58.0, -20.0, 13.0, 10.0, 8, 2);
    addBuilding(-40.0, -23.0, 14.0, 11.0, 6, 6);
    addBuilding(-18.0, -29.0, 16.0, 12.0, 5, 4);
    addBuilding(  8.0, -29.0, 15.0, 12.0, 7, 0);
    addBuilding( 34.0, -29.0, 17.0, 13.0, 4, 3);
    addBuilding(-55.0,  32.0, 14.0, 11.0, 5, 0);
    addBuilding(-35.0,  35.0, 15.0, 12.0, 7, 5);

    // Trees are deliberately placed in park pockets and along the outer green
    // edges, never in building footprints or on the road centerlines.
    for (int ix=0; ix<5; ++ix)
        for (int iz=0; iz<3; ++iz)
            trees.push_back({42.0+ix*2.7, -48.0+iz*2.7, 0.9+(ix%2)*0.12});
    for (int i=0;i<7;++i) {
        trees.push_back({-73.0+i*5.0, 9.0, 0.9});
        trees.push_back({-73.0+i*5.0, -7.0, 0.9});
    }
}

void drawRoadSegment(double x1,double z1,double x2,double z2,double width,
                     double r,double g,double b,double y=0.012) {
    const double dx=x2-x1, dz=z2-z1;
    const double length=std::sqrt(dx*dx+dz*dz);
    if (length<0.01) return;
    const double angle=std::atan2(dx,dz)*180.0/PI;
    glPushMatrix();
    glTranslated((x1+x2)/2.0,y,(z1+z2)/2.0);
    glRotated(angle,0,1,0);
    cube(0,0,0,width,0.035,length+width*0.18,r,g,b);
    glPopMatrix();
}

void drawSidewalkSegment(double x1,double z1,double x2,double z2,double width) {
    drawRoadSegment(x1,z1,x2,z2,width,0.62,0.61,0.56,0.045);
}

void drawRoads() {
    // One clean road plan: two main avenues, one diagonal approach and
    // a few short local streets. No city-wide grid or overlapping diagonals.
    cube(0,-0.22,0,WORLD_LIMIT*2,0.4,WORLD_LIMIT*2,0.36,0.52,0.31);

    // Main east-west avenue and the north-south connector.
    drawRoadSegment(-WORLD_LIMIT,0.0,WORLD_LIMIT,0.0,8.4,0.20,0.22,0.23);
    drawRoadSegment(55.0,-WORLD_LIMIT,55.0,WORLD_LIMIT,8.4,0.20,0.22,0.23);
    // A secondary eastbound road meets the north-south avenue at a T-junction.
    drawRoadSegment(55.0,15.0,WORLD_LIMIT,15.0,7.0,0.20,0.22,0.23);
    // Diagonal approach from the west into the northern side of the district.
    drawRoadSegment(-70.0,0.0,55.0,39.0,7.2,0.20,0.22,0.23);

    // Local access roads form short connected branches inside the housing area.
    drawRoadSegment(-48.0,8.0,38.0,34.0,4.4,0.27,0.29,0.29);
    drawRoadSegment(-43.0,-4.0,24.0,14.0,4.4,0.27,0.29,0.29);
    drawRoadSegment(-30.0,-15.0,34.0,-15.0,4.4,0.27,0.29,0.29);
    drawRoadSegment(37.0,-15.0,37.0,28.0,4.4,0.27,0.29,0.29);
    drawRoadSegment(-52.0,-27.0,-52.0,-9.0,4.2,0.27,0.29,0.29);

    // Sidewalks follow the main roads as separate strips, kept off the asphalt.
    drawSidewalkSegment(-WORLD_LIMIT,5.0,WORLD_LIMIT,5.0,1.25);
    drawSidewalkSegment(-WORLD_LIMIT,-5.0,WORLD_LIMIT,-5.0,1.25);
    drawSidewalkSegment(60.0,-WORLD_LIMIT,60.0,WORLD_LIMIT,1.25);
    drawSidewalkSegment(50.0,-WORLD_LIMIT,50.0,WORLD_LIMIT,1.25);
    // Short sidewalk sections beside the diagonal and internal streets.
    drawSidewalkSegment(-68.0,4.5,54.0,43.5,1.0);
    drawSidewalkSegment(-72.0,-4.5,56.0,34.5,1.0);

    // Dashed lane markings on the main avenues.
    for (double x=-86.0;x<=86.0;x+=11.0)
        cube(x,0.040,0.0,4.0,0.018,0.12,0.88,0.84,0.70);
    for (double z=-86.0;z<=86.0;z+=11.0)
        cube(55.0,0.040,z,0.12,0.018,4.0,0.88,0.84,0.70);

    // Crosswalks only at the two planned major junctions.
    for (double offset=-3.0;offset<=3.01;offset+=1.0) {
        cube(55.0+offset,0.045,5.8,0.55,0.018,2.6,0.92,0.91,0.83);
        cube(55.0+offset,0.045,24.2,0.55,0.018,2.6,0.92,0.91,0.83);
        cube(49.2,0.045,offset,2.6,0.018,0.55,0.92,0.91,0.83);
        cube(60.8,0.045,offset,2.6,0.018,0.55,0.92,0.91,0.83);
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
    // GLU cylinders grow along local +Z; rotate the axis to world +Y.
    glRotated(-90.0,1.0,0.0,0.0);
    color(0.34,0.24,0.15);
    gluCylinder(quadric,0.19*t.scale,0.13*t.scale,h*0.72,8,1);
    // Build a conventional tree from two upright tapered crowns, each starting
    // with its broad base below its pointed top.
    glTranslated(0,0,h*0.28);
    color(0.22,0.39,0.20);
    gluCylinder(quadric,0.78*t.scale,0.0,h*0.78,8,1);
    glTranslated(0,0,h*0.30);
    color(0.32,0.52,0.27);
    gluCylinder(quadric,0.56*t.scale,0.0,h*0.62,8,1);
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
    // Street furniture omitted on this clean-map pass; it will be placed along the final road plan.
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
