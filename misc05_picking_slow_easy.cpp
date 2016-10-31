// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <array>
#include <sstream>
#include <math.h>
// Include GLEW
#include <GL/glew.h>
// Include GLFW
#include <glfw3.h>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using namespace glm;
// Include AntTweakBar
#include <AntTweakBar.h>

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>

typedef struct Vertex {
	float XYZW[4];
	float RGBA[4];
	void SetCoords(float *coords) {
		XYZW[0] = coords[0];
		XYZW[1] = coords[1];
		XYZW[2] = coords[2];
		XYZW[3] = coords[3];
	}
	void SetColor(float *color) {
		RGBA[0] = color[0];
		RGBA[1] = color[1];
		RGBA[2] = color[2];
		RGBA[3] = color[3];
	}
};

// ATTN: USE POINT STRUCTS FOR EASIER COMPUTATIONS
typedef struct point {
	float x, y, z;
	point(const float x = 0, const float y = 0, const float z = 0) : x(x), y(y), z(z){};
	point(float *coords) : x(coords[0]), y(coords[1]), z(coords[2]){};
	point operator -(const point& a)const {
		return point(x - a.x, y - a.y, z - a.z);
	}
	point operator +(const point& a)const {
		return point(x + a.x, y + a.y, z + a.z);
	}
	point operator *(const float& a)const {
		return point(x*a, y*a, z*a);
	}
	point operator /(const float& a)const {
		return point(x / a, y / a, z / a);
	}
	float* toArray() {
		float array[] = { x, y, z, 1.0f };
		return array;
	}
};
struct point p[100];
struct point final_point;
struct point con1[5];
int flag1=0;


// function prototypes

int initWindow(void);
void initOpenGL(void);
void createVAOs(Vertex[], unsigned short[], size_t, size_t, int);
point decasteljau(point , point , float);
void create_beziercurve(point ,point ,point ,point ,point);
void createObjects(void);
void pickVertex(void);
void moveVertex(void);
void drawScene(void);
void cleanup(void);
static void mouseCallback(GLFWwindow*, int, int, int);
static void keyCallback(GLFWwindow *window, int, int, int, int);

// GLOBAL VARIABLES
GLFWwindow* window;
const GLuint window_width = 1024, window_height = 768;

glm::mat4 gProjectionMatrix;
glm::mat4 gViewMatrix;

GLuint gPickedIndex;
std::string gMessage;

GLuint programID;
GLuint pickingProgramID;

// ATTN: INCREASE THIS NUMBER AS YOU CREATE NEW OBJECTS
const GLuint NumObjects = 8;	// number of different "objects" to be drawn
GLuint VertexArrayId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7};
GLuint VertexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7};
GLuint IndexBufferId[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7};
size_t NumVert[NumObjects] = { 0, 1, 2, 3, 4, 5, 6, 7};

GLuint MatrixID;
GLuint ViewMatrixID;
GLuint ModelMatrixID;
GLuint PickingMatrixID;
GLuint pickingColorArrayID;
GLuint pickingColorID;
GLuint LightID;

Vertex verti[81];


// Define objects
Vertex Vertices[] =
{
     { { 0.0f, 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // 5
     { { -0.7f, 0.7f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // 1
     { { -1.0f, 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f, 0.0f, 1.0f} }, // 7
     { { -0.7f, -0.7f, 0.0f, 1.0f }, {0.0f, 1.0f, 0.0f, 1.0f } }, // 2
     { { 0.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f} }, // 6
     { { 0.7f, -0.7f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // 3
     { { 1.0f, 0.0f, 0.0f, 1.0f }, {0.0f, 1.0f, 0.0f, 1.0f } }, // 4
     { { 0.7f, 0.7f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // 0
    
};

Vertex Vertices1[] =
{
    { { 0.0f, 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // 5
    { { -0.7f, 0.7f, 0.0f, 1.0f }, {1.0f, 1.0f, 1.0f, 1.0f } }, // 1
    { { -1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f} }, // 7
    { { -0.7f, -0.7f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // 2
    { { 0.0f, -1.0f, 0.0f, 1.0f }, {1.0f, 1.0f, 1.0f, 1.0f} }, // 6
    { { 0.7f, -0.7f, 0.0f, 1.0f }, {1.0f, 1.0f, 1.0f, 1.0f } }, // 3
    { { 1.0f, 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f} }, // 4
    { { 0.7f, 0.7f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f } }, // 0
    
};

Vertex Vertices_a[16], Vertices_b[32], Vertices_c[64], Vertices_d[128];

unsigned short Indices[] = {
	0, 1, 2, 3, 4, 5, 6, 7
};
unsigned short IndicesLine[] = {
    0, 1, 2, 3, 4, 5, 6, 7
};

unsigned short Indices1[16], Indices2[32], Indices3[64], Indices4[128];
unsigned short Indices_b[81], Indices_c[41];



const size_t IndexCount = sizeof(Indices) / sizeof(unsigned short);
// ATTN: DON'T FORGET TO INCREASE THE ARRAY SIZE IN THE PICKING VERTEX SHADER WHEN YOU ADD MORE PICKING COLORS
float pickingColor[IndexCount] = { 0 / 255.0f, 1 / 255.0f, 2 / 255.0f, 3 / 255.0f, 4 / 255.0f, 5 / 255.0f, 6 / 255.0f, 7 / 255.0f};

// ATTN: ADD YOU PER-OBJECT GLOBAL ARRAY DEFINITIONS HERE


GLint k = 0;
GLint flag = 0;
//int q1=0;
void createObjects(void)
{
	// ATTN: DERIVE YOUR NEW OBJECTS HERE:
	// each has one vertices {pos;color} and one indices array (no picking needed here)
    
    for(int i=0;i<80;i++)
    {
        Indices_b[i] = i;
    }
    for(int i=0;i<40;i++)
    {
        Indices_c[i] = i;
    }
    
    p[0].x = Vertices[0].XYZW[0];
    p[0].y = Vertices[0].XYZW[1];
    p[0].z = Vertices[0].XYZW[2];
    
    p[1].x = Vertices[1].XYZW[0];
    p[1].y = Vertices[1].XYZW[1];
    p[1].z = Vertices[1].XYZW[2];
    
    p[2].x = Vertices[2].XYZW[0];
    p[2].y = Vertices[2].XYZW[1];
    p[2].z = Vertices[2].XYZW[2];
    
    p[3].x = Vertices[3].XYZW[0];
    p[3].y = Vertices[3].XYZW[1];
    p[3].z = Vertices[3].XYZW[2];
    
    p[4].x = Vertices[4].XYZW[0];
    p[4].y = Vertices[4].XYZW[1];
    p[4].z = Vertices[4].XYZW[2];
    
    p[5].x = Vertices[5].XYZW[0];
    p[5].y = Vertices[5].XYZW[1];
    p[5].z = Vertices[5].XYZW[2];
    
    p[6].x = Vertices[6].XYZW[0];
    p[6].y = Vertices[6].XYZW[1];
    p[6].z = Vertices[6].XYZW[2];
    
    p[7].x = Vertices[7].XYZW[0];
    p[7].y = Vertices[7].XYZW[1];
    p[7].z = Vertices[7].XYZW[2];
    
    for (int i = 0; i < 8; i++)
    {
        
        
        int m = i - 2;
        if (m < 0)
        {
            m = m + 8;
        }
        int n = i - 1;
        if (n < 0)
        {
            n = n + 8;
        }
        
        
        create_beziercurve(p[m], p[n], p[i], p[(i + 1)%8], p[(i + 2)%8]);
        
    }
    
    for(int x=0;x<16;x++)
        Indices1[x]=x;
    
    for(int x=0;x<32;x++)
        Indices2[x]=x;
    
    for(int x=0;x<64;x++)
        Indices3[x]=x;
    
    for(int x=0;x<128;x++)
        Indices4[x]=x;
    
    //k==1
    int i=0;
    int j=0;
    do{
                
        int p,q,r;
        if(i==0)
        {
            p=6;
            q=7;
            r=0;
        }
        else if(i==1)
        {
            p=7;
            q=0;
            r=1;
        }
        else
        {
            p=i-2;
            q=i-1;
            r=i;
        }
        
        //Vertices_a level 1
                    
        Vertices_a[j].XYZW[0] = (Vertices[p].XYZW[0] + 10*Vertices[q].XYZW[0] + 5*Vertices[r].XYZW[0])/16;
        Vertices_a[j].XYZW[1] = (Vertices[p].XYZW[1] + 10*Vertices[q].XYZW[1] + 5*Vertices[r].XYZW[1])/16;
        Vertices_a[j].XYZW[2] = 0.0;
        Vertices_a[j].XYZW[3] = 1.0;
        Vertices_a[j].RGBA[0] = 0.0;
        Vertices_a[j].RGBA[1] = 0.0;
        Vertices_a[j].RGBA[2] = 1.0;
        Vertices_a[j].RGBA[3] = 1.0;
                    
        j++;
                    
        Vertices_a[j].XYZW[0] = (5*Vertices[q].XYZW[0] + 10*Vertices[r].XYZW[0] + Vertices[(r+1)%8].XYZW[0])/16;
        Vertices_a[j].XYZW[1] = (5*Vertices[q].XYZW[1] + 10*Vertices[r].XYZW[1] + Vertices[(r+1)%8].XYZW[1])/16;
        Vertices_a[j].XYZW[2] = 0.0;
        Vertices_a[j].XYZW[3] = 1.0;
        Vertices_a[j].RGBA[0] = 0.0;
        Vertices_a[j].RGBA[1] = 0.0;
        Vertices_a[j].RGBA[2] = 1.0;
        Vertices_a[j].RGBA[3] = 1.0;
                    
        j++;
        i++;
    }while(i!=8);
    
    //k==2
    i=0;
    j=0;
    do{
        
        int p,q,r;
        if(i==0)
        {
            p=14;
            q=15;
            r=0;
        }
        else if(i==1)
        {
            p=15;
            q=0;
            r=1;
        }
        else
        {
            p=i-2;
            q=i-1;
            r=i;
        }
        
        //Vertices_b level 2
        
        Vertices_b[j].XYZW[0] = (Vertices_a[p].XYZW[0] + 10*Vertices_a[q].XYZW[0] + 5*Vertices_a[r].XYZW[0])/16;
        Vertices_b[j].XYZW[1] = (Vertices_a[p].XYZW[1] + 10*Vertices_a[q].XYZW[1] + 5*Vertices_a[r].XYZW[1])/16;
        Vertices_b[j].XYZW[2] = 0.0;
        Vertices_b[j].XYZW[3] = 1.0;
        Vertices_b[j].RGBA[0] = 0.0;
        Vertices_b[j].RGBA[1] = 0.0;
        Vertices_b[j].RGBA[2] = 1.0;
        Vertices_b[j].RGBA[3] = 1.0;
        
        j++;
        
        Vertices_b[j].XYZW[0] = (5*Vertices_a[q].XYZW[0] + 10*Vertices_a[r].XYZW[0] + Vertices_a[(r+1)%16].XYZW[0])/16;
        Vertices_b[j].XYZW[1] = (5*Vertices_a[q].XYZW[1] + 10*Vertices_a[r].XYZW[1] + Vertices_a[(r+1)%16].XYZW[1])/16;
        Vertices_b[j].XYZW[2] = 0.0;
        Vertices_b[j].XYZW[3] = 1.0;
        Vertices_b[j].RGBA[0] = 0.0;
        Vertices_b[j].RGBA[1] = 0.0;
        Vertices_b[j].RGBA[2] = 1.0;
        Vertices_b[j].RGBA[3] = 1.0;
        
        j++;
        i++;
    }while(i!=16);
    
    //k==3
    i=0;
    j=0;
    do{
        
        int p,q,r;
        if(i==0)
        {
            p=30;
            q=31;
            r=0;
        }
        else if(i==1)
        {
            p=31;
            q=0;
            r=1;
        }
        else
        {
            p=i-2;
            q=i-1;
            r=i;
        }
        
        //Vertices_c level 3
        
        Vertices_c[j].XYZW[0] = (Vertices_b[p].XYZW[0] + 10*Vertices_b[q].XYZW[0] + 5*Vertices_b[r].XYZW[0])/16;
        Vertices_c[j].XYZW[1] = (Vertices_b[p].XYZW[1] + 10*Vertices_b[q].XYZW[1] + 5*Vertices_b[r].XYZW[1])/16;
        Vertices_c[j].XYZW[2] = 0.0;
        Vertices_c[j].XYZW[3] = 1.0;
        Vertices_c[j].RGBA[0] = 0.0;
        Vertices_c[j].RGBA[1] = 0.0;
        Vertices_c[j].RGBA[2] = 1.0;
        Vertices_c[j].RGBA[3] = 1.0;
        
        j++;
        
        Vertices_c[j].XYZW[0] = (5*Vertices_b[q].XYZW[0] + 10*Vertices_b[r].XYZW[0] + Vertices_b[(r+1)%32].XYZW[0])/16;
        Vertices_c[j].XYZW[1] = (5*Vertices_b[q].XYZW[1] + 10*Vertices_b[r].XYZW[1] + Vertices_b[(r+1)%32].XYZW[1])/16;
        Vertices_c[j].XYZW[2] = 0.0;
        Vertices_c[j].XYZW[3] = 1.0;
        Vertices_c[j].RGBA[0] = 0.0;
        Vertices_c[j].RGBA[1] = 0.0;
        Vertices_c[j].RGBA[2] = 1.0;
        Vertices_c[j].RGBA[3] = 1.0;
        
        j++;
        i++;
    }while(i!=32);
    
    //k==4
    i=0;
    j=0;
    do{
        
        int p,q,r;
        if(i==0)
        {
            p=62;
            q=63;
            r=0;
        }
        else if(i==1)
        {
            p=63;
            q=0;
            r=1;
        }
        else
        {
            p=i-2;
            q=i-1;
            r=i;
        }
        
        //Vertices_d level 4
        
        Vertices_d[j].XYZW[0] = (Vertices_c[p].XYZW[0] + 10*Vertices_c[q].XYZW[0] + 5*Vertices_c[r].XYZW[0])/16;
        Vertices_d[j].XYZW[1] = (Vertices_c[p].XYZW[1] + 10*Vertices_c[q].XYZW[1] + 5*Vertices_c[r].XYZW[1])/16;
        Vertices_d[j].XYZW[2] = 0.0;
        Vertices_d[j].XYZW[3] = 1.0;
        Vertices_d[j].RGBA[0] = 0.0;
        Vertices_d[j].RGBA[1] = 0.0;
        Vertices_d[j].RGBA[2] = 1.0;
        Vertices_d[j].RGBA[3] = 1.0;
        
        j++;
        
        Vertices_d[j].XYZW[0] = (5*Vertices_c[q].XYZW[0] + 10*Vertices_c[r].XYZW[0] + Vertices_c[(r+1)%64].XYZW[0])/16;
        Vertices_d[j].XYZW[1] = (5*Vertices_c[q].XYZW[1] + 10*Vertices_c[r].XYZW[1] + Vertices_c[(r+1)%64].XYZW[1])/16;
        Vertices_d[j].XYZW[2] = 0.0;
        Vertices_d[j].XYZW[3] = 1.0;
        Vertices_d[j].RGBA[0] = 0.0;
        Vertices_d[j].RGBA[1] = 0.0;
        Vertices_d[j].RGBA[2] = 1.0;
        Vertices_d[j].RGBA[3] = 1.0;
        
        j++;
        i++;
    }while(i!=64);


}

Vertex v_control[41];
GLint q1=0;
void create_beziercurve(point a,point b,point c,point d,point e)
{
    
    // a = P_i-2, b = P_i-1, c = P_i, d = P_i+1, e = P_i+2
    con1[0].x = (a.x + b.x * 11.0f + c.x * 11.0f + d.x) / 24.0f;
    con1[0].y = (a.y + b.y * 11.0f + c.y * 11.0f + d.y) / 24.0f;
    con1[0].z = 0.0f;
    
    con1[1].x = (b.x * 4.0f + c.x * 7.0f + d.x) / 12.0f;
    con1[1].y = (b.y * 4.0f + c.y * 7.0f + d.y) / 12.0f;
    con1[1].z = 0.0f;
    
    con1[2].x = (b.x * 4.0f + c.x * 16.0f + d.x * 4.0f) / 24.0f;
    con1[2].y = (b.y * 4.0f + c.y * 16.0f + d.y * 4.0f) / 24.0f;
    con1[2].z = 0.0f;
    
    con1[3].x = (b.x + c.x * 7.0f + d.x * 4.0f) / 12.0f;
    con1[3].y = (b.y + c.y * 7.0f + d.y * 4.0f) / 12.0f;
    con1[3].z = 0.0f;
    
    con1[4].x = (b.x + c.x * 11.0f + d.x * 11.0f + e.x) / 24.0f;
    con1[4].y = (b.y + c.y * 11.0f + d.y * 11.0f + e.y) / 24.0f;
    con1[4].z = 0.0f;
    
    v_control[q1].XYZW[0] = con1[0].x;
    v_control[q1].XYZW[1] = con1[0].y;
    v_control[q1].XYZW[2] = con1[0].z;
    v_control[q1].XYZW[3] = 1.0f;
    v_control[q1].RGBA[0] = 1.0f;
    v_control[q1].RGBA[1] = 0.0f;
    v_control[q1].RGBA[2] = 0.0f;
    v_control[q1].RGBA[3] = 1.0f;
    
    v_control[q1+1].XYZW[0] = con1[1].x;
    v_control[q1+1].XYZW[1] = con1[1].y;
    v_control[q1+1].XYZW[2] = con1[1].z;
    v_control[q1+1].XYZW[3] = 1.0f;
    v_control[q1+1].RGBA[0] = 1.0f;
    v_control[q1+1].RGBA[1] = 0.0f;
    v_control[q1+1].RGBA[2] = 0.0f;
    v_control[q1+1].RGBA[3] = 1.0f;
    
    v_control[q1+2].XYZW[0] = con1[2].x;
    v_control[q1+2].XYZW[1] = con1[2].y;
    v_control[q1+2].XYZW[2] = con1[2].z;
    v_control[q1+2].XYZW[3] = 1.0f;
    v_control[q1+2].RGBA[0] = 1.0f;
    v_control[q1+2].RGBA[1] = 0.0f;
    v_control[q1+2].RGBA[2] = 0.0f;
    v_control[q1+2].RGBA[3] = 1.0f;
    
    v_control[q1+3].XYZW[0] = con1[3].x;
    v_control[q1+3].XYZW[1] = con1[3].y;
    v_control[q1+3].XYZW[2] = con1[3].z;
    v_control[q1+3].XYZW[3] = 1.0f;
    v_control[q1+3].RGBA[0] = 1.0f;
    v_control[q1+3].RGBA[1] = 0.0f;
    v_control[q1+3].RGBA[2] = 0.0f;
    v_control[q1+3].RGBA[3] = 1.0f;
    q1+=4;
    if(q1==40)
        q1=0;
    
    struct point a1, b1, c1, d1, e1, f1, g1, h1,i1;
    float t = 0.0;
    
    for (t = 0.0; t < 1;t=t+0.1)
    {
        a1 = decasteljau(con1[0], con1[1], t);
        b1 = decasteljau(con1[1], con1[2], t);
        c1 = decasteljau(con1[2], con1[3], t);
        d1 = decasteljau(con1[3], con1[4], t);
        e1 = decasteljau(a1, b1, t);
        f1 = decasteljau(b1, c1, t);
        g1 = decasteljau(c1, d1, t);
        h1 = decasteljau(e1, f1, t);
        i1 = decasteljau(f1, g1, t);
        
        
        final_point = decasteljau(h1, i1, t);
        
        float co = 0.0f, col = 1.0f;
        
        float x = final_point.x;
        float y = final_point.y;
        
        verti[k].XYZW[0] = x;
        verti[k].XYZW[1] = y;
        verti[k].XYZW[2] = co;
        verti[k].XYZW[3] = col;
        verti[k].RGBA[0] = 1.0;
        verti[k].RGBA[1] = 1.0;
        verti[k].RGBA[2] = 0.0;
        verti[k].RGBA[3] = col;
        
        k++;
        if (k == 80)
        {
            k = 0;
        }
        
    }
    
}
point decasteljau(point b, point c, float t)
{
    struct point a;
    a.x = (1 - t)*b.x + t*c.x;
    a.y = (1 - t)*b.y + t*c.y;
    
    return a;
}


void drawScene(void)
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Re-clear the screen for real rendering
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &gViewMatrix[0][0]);
		glm::vec3 lightPos = glm::vec3(4, 4, 4);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
		
		glEnable(GL_PROGRAM_POINT_SIZE);
    
        
        

		glBindVertexArray(VertexArrayId[0]);	// draw Vertices
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
        
        glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
        glBindVertexArray(0);
        
        //lines
        glBindVertexArray(VertexArrayId[1]);	// draw Vertices
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[1]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices1), Vertices1);
        
        // update buffer data
			//glDrawElements(GL_LINE_LOOP, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
        glDrawElements(GL_LINE_STRIP, NumVert[1], GL_UNSIGNED_SHORT, (void*)0);
        glBindVertexArray(1);
            
        
			
		// ATTN: OTHER BINDING AND DRAWING COMMANDS GO HERE, one set per object:
		//glBindVertexArray(VertexArrayId[<x>]); etc etc
        if(flag1==1)
        {
            // draw lines for k=1
            //lines
            glBindVertexArray(VertexArrayId[2]);	// draw Vertices
            glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[2]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices_a), Vertices_a);
        
            // update buffer data
            glDrawElements(GL_LINE_STRIP, NumVert[2], GL_UNSIGNED_SHORT, (void*)0);
        
            glBindVertexArray(2);
        }
        if(flag1==2)
        {
            //draw lines for k=2
            //lines
            glBindVertexArray(VertexArrayId[3]);	// draw Vertices
            glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[3]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices_b), Vertices_b);
        
            // update buffer data
            glDrawElements(GL_LINE_STRIP, NumVert[3], GL_UNSIGNED_SHORT, (void*)0);
        
            glBindVertexArray(3);
        }
        
        if(flag1==3)
        {
            //draw lines for k=3
            //lines
            glBindVertexArray(VertexArrayId[6]);	// draw Vertices
            glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[6]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices_c), Vertices_c);
            
            // update buffer data
            glDrawElements(GL_LINE_STRIP, NumVert[6], GL_UNSIGNED_SHORT, (void*)0);
            
            glBindVertexArray(6);
        }
        
        if(flag1==4)
        {
            //draw lines for k=4
            //lines
            glBindVertexArray(VertexArrayId[7]);	// draw Vertices
            glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[7]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices_d), Vertices_d);
            
            // update buffer data
            glDrawElements(GL_LINE_STRIP, NumVert[7], GL_UNSIGNED_SHORT, (void*)0);
            
            glBindVertexArray(7);
        }
        
        if(flag1==5)
        {
            //task 4
            glBindVertexArray(VertexArrayId[4]);	// draw lines
            glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[4]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verti), verti);
        
            // update buffer data
            glDrawElements(GL_LINE_STRIP, NumVert[4], GL_UNSIGNED_SHORT, (void*)0);
            glBindVertexArray(4);
        
            glBindVertexArray(VertexArrayId[5]);	// draw Vertices
            glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[5]);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(v_control), v_control);
        
            // update buffer data
            glDrawElements(GL_POINTS, NumVert[5], GL_UNSIGNED_SHORT, (void*)0);
            glBindVertexArray(5);
        }
        
        
        
        
        
	}
	glUseProgram(0);
	// Draw GUI
	TwDraw();

	// Swap buffers
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void pickVertex(void)
{
	// Clear the screen in white
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(pickingProgramID);
	{
		glm::mat4 ModelMatrix = glm::mat4(1.0); // TranslationMatrix * RotationMatrix;
		glm::mat4 MVP = gProjectionMatrix * gViewMatrix * ModelMatrix;

		// Send our transformation to the currently bound shader, in the "MVP" uniform
		glUniformMatrix4fv(PickingMatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniform1fv(pickingColorArrayID, NumVert[0], pickingColor);	// here we pass in the picking marker array

		// Draw the points
		glEnable(GL_PROGRAM_POINT_SIZE);
		glBindVertexArray(VertexArrayId[0]);
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[0]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);	// update buffer data
		glDrawElements(GL_POINTS, NumVert[0], GL_UNSIGNED_SHORT, (void*)0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	// Wait until all the pending drawing commands are really done.
	// Ultra-mega-over slow ! 
	// There are usually a long time between glDrawElements() and
	// all the fragments completely rasterized.
	glFlush();
	glFinish();

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Read the pixel at the center of the screen.
	// You can also use glfwGetMousePos().
	// Ultra-mega-over slow too, even for 1 pixel, 
	// because the framebuffer is on the GPU.
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	unsigned char data[4];
	glReadPixels(xpos, window_height - ypos, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top
	// Convert the color back to an integer ID
	gPickedIndex = int(data[0]);

	// Uncomment these lines to see the picking shader in effect
	//glfwSwapBuffers(window);
	//continue; // skips the normal rendering
}

// fill this function in!
void moveVertex(void)
{
	glm::mat4 ModelMatrix = glm::mat4(1.0);
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glm::vec4 vp = glm::vec4(viewport[0], viewport[1], viewport[2], viewport[3]);
	
	// retrieve your cursor position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
	// get your world coordinates
    glViewport(0,0,1024,768);
    //glm::vec4 viewport = glm::vec4(0,0,800,600);
    glm::vec3 v = glm::unProject (glm::vec3(xpos, window_height-ypos, 0.0), ModelMatrix, gProjectionMatrix, vp);
    
    // move points
    Vertices[gPickedIndex].XYZW[0] = -v[0];
    Vertices[gPickedIndex].XYZW[1] = v[1];
    
    // move lines
    Vertices1[gPickedIndex].XYZW[0] = -v[0];
    Vertices1[gPickedIndex].XYZW[1] = v[1];

	if (gPickedIndex == 255){ // Full white, must be the background !
		gMessage = "background";
	}
	else {
		std::ostringstream oss;
		oss << "point " << gPickedIndex;
		gMessage = oss.str();
	}
}

int initWindow(void)
{
	// Initialise GLFW
	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(window_width, window_height, "Majumdar, Jahin (69139840)", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	// Initialize the GUI
	TwInit(TW_OPENGL_CORE, NULL);
	TwWindowSize(window_width, window_height);
	TwBar * GUI = TwNewBar("Picking");
	TwSetParam(GUI, NULL, "refresh", TW_PARAM_CSTRING, 1, "0.1");
	TwAddVarRW(GUI, "Last picked object", TW_TYPE_STDSTRING, &gMessage, NULL);

    // Create a Vector Buffer Object that will store the vertices on video memory
    GLuint vbo;
    glGenBuffers(1, &vbo);
    // Allocate space and upload the data from CPU to GPU
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    
    
    
	// Set up inputs
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_FALSE);
	glfwSetCursorPos(window, window_width / 2, window_height / 2);
	glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);

	return 0;
}

void initOpenGL(void)
{
	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	//glm::mat4 ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
	// Or, for an ortho camera :
	gProjectionMatrix = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, 0.0f, 100.0f); // In world coordinates

	// Camera matrix
	gViewMatrix = glm::lookAt(
		glm::vec3(0, 0, -5), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

	// Create and compile our GLSL program from the shaders
	programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	pickingProgramID = LoadShaders("Picking.vertexshader", "Picking.fragmentshader");

	// Get a handle for our "MVP" uniform
	MatrixID = glGetUniformLocation(programID, "MVP");
	ViewMatrixID = glGetUniformLocation(programID, "V");
	ModelMatrixID = glGetUniformLocation(programID, "M");
	PickingMatrixID = glGetUniformLocation(pickingProgramID, "MVP");
	// Get a handle for our "pickingColorID" uniform
	pickingColorArrayID = glGetUniformLocation(pickingProgramID, "PickingColorArray");
	pickingColorID = glGetUniformLocation(pickingProgramID, "PickingColor");
	// Get a handle for our "LightPosition" uniform
	LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	
    createVAOs(Vertices, Indices, sizeof(Vertices), sizeof(Indices), 0);
	createVAOs(Vertices1, IndicesLine, sizeof(Vertices1), sizeof(IndicesLine), 1);
    createObjects();
    
	// ATTN: create VAOs for each of the newly created objects here:
	// createVAOs(<fill this appropriately>);
    
    createVAOs(Vertices_a, Indices1, sizeof(Vertices_a), sizeof(Indices1), 2);
    createVAOs(Vertices_b, Indices2, sizeof(Vertices_b), sizeof(Indices2), 3);
    createVAOs(Vertices_c, Indices3, sizeof(Vertices_c), sizeof(Indices3), 6);
    createVAOs(Vertices_d, Indices4, sizeof(Vertices_d), sizeof(Indices4), 7);
    createVAOs(verti, Indices_b, sizeof(verti), sizeof(Indices_b), 4);
    createVAOs(v_control, Indices_c, sizeof(v_control), sizeof(Indices_c), 5);
    
    
    

}

void createVAOs(Vertex Vertices[], unsigned short Indices[], size_t BufferSize, size_t IdxBufferSize, int ObjectId) {

	NumVert[ObjectId] = IdxBufferSize / (sizeof( GLubyte));

	GLenum ErrorCheckValue = glGetError();
	size_t VertexSize = sizeof(Vertices[0]);
	size_t RgbOffset = sizeof(Vertices[0].XYZW);

	// Create Vertex Array Object
	glGenVertexArrays(1, &VertexArrayId[ObjectId]);
	glBindVertexArray(VertexArrayId[ObjectId]);

	// Create Buffer for vertex data
	glGenBuffers(1, &VertexBufferId[ObjectId]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId[ObjectId]);
	glBufferData(GL_ARRAY_BUFFER, BufferSize, Vertices, GL_STATIC_DRAW);

	// Create Buffer for indices
	glGenBuffers(1, &IndexBufferId[ObjectId]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId[ObjectId]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, IdxBufferSize, Indices, GL_STATIC_DRAW);

	// Assign vertex attributes
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, VertexSize, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, VertexSize, (GLvoid*)RgbOffset);

	glEnableVertexAttribArray(0);	// position
	glEnableVertexAttribArray(1);	// color

	// Disable our Vertex Buffer Object 
	glBindVertexArray(0);

	ErrorCheckValue = glGetError();
	if (ErrorCheckValue != GL_NO_ERROR)
	{
		fprintf(
			stderr,
			"ERROR: Could not create a VBO: %s \n",
			gluErrorString(ErrorCheckValue)
			);
	}
}

void cleanup(void)
{
	// Cleanup VBO and shader
	for (int i = 0; i < NumObjects; i++) {
		glDeleteBuffers(1, &VertexBufferId[i]);
		glDeleteBuffers(1, &IndexBufferId[i]);
		glDeleteVertexArrays(1, &VertexArrayId[i]);
	}
	glDeleteProgram(programID);
	glDeleteProgram(pickingProgramID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		pickVertex();
	}
}

static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if(key==GLFW_KEY_1 && action== GLFW_PRESS && (flag1==0 || flag1==5))
    {
        flag1=1;
    }
    else if(key==GLFW_KEY_1 && action== GLFW_RELEASE && flag1==1)
    {
        flag1=1;
    }
    else if(key==GLFW_KEY_1 && action== GLFW_PRESS && flag1==1)
    {
        flag1=2;
    }
    else if(key==GLFW_KEY_1 && action== GLFW_RELEASE && flag1==2)
    {
        flag1=2;
    }
    else if(key==GLFW_KEY_1 && action== GLFW_PRESS && flag1==2)
    {
        flag1=3;
    }
    else if(key==GLFW_KEY_1 && action== GLFW_RELEASE && flag1==3)
    {
        flag1=3;
    }
    else if(key==GLFW_KEY_1 && action== GLFW_PRESS && flag1==3)
    {
        flag1=4;
    }
    else if(key==GLFW_KEY_1 && action== GLFW_RELEASE && flag1==4)
    {
        flag1=4;
    }
    else if(key==GLFW_KEY_1 && action== GLFW_PRESS && flag1==4)
    {
        flag1=0;
    }
    
    else if(key==GLFW_KEY_2 && action== GLFW_PRESS && flag1==0)
    {
        flag1=5;
    }
    
    else if(key==GLFW_KEY_2 && action== GLFW_RELEASE && flag1==5)
    {
        flag1=5;
    }
    
    else if(key==GLFW_KEY_2 && action== GLFW_PRESS && flag1==5)
    {
        flag1=0;
    }
     
    else if(key==GLFW_KEY_2 && action== GLFW_RELEASE && flag1==5)
    {
        flag1=0;
    }
}

int main(void)
{
	// initialize window
	int errorCode = initWindow();
	if (errorCode != 0)
		return errorCode;

	// initialize OpenGL pipeline
	initOpenGL();

	// For speed computation
	double lastTime = glfwGetTime();
	int nbFrames = 0;
	do {
		// Measure speed
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0){ // If last prinf() was more than 1sec ago
			// printf and reset
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
		}

		// DRAGGING: move current (picked) vertex with cursor
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
			moveVertex();

		// DRAWING SCENE
		createObjects();	// re-evaluate curves in case vertices have been moved
		drawScene();


	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
	glfwWindowShouldClose(window) == 0);

	cleanup();

	return 0;
}
