////////////////////////////////////////////////////
// anim.cpp version 4.1
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef WIN32
#include "GL/freeglut.h"
#else
#include <GLUT/glut.h>
#endif

#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"
#include "Shapes.h"
#include "tga.h"

#include "Angel.h"

#ifdef __APPLE__
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#endif

FrameSaver FrSaver ;
Timer TM ;

BallData *Arcball = NULL ;
int Width = 800;
int Height = 800 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;

int Animate = 0 ;
int Recording = 0 ;

int BENDING_ARMS = 1;
int RAISING_ARMS = 2;
int STILL_ARMS = 3;

void resetArcball() ;
void save_image();
void instructions();
void set_colour(float r, float g, float b) ;

const int STRLEN = 100;
typedef char STR[STRLEN];

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2
#define ROOT2 1.41421356237



//texture

GLuint texture_cube;
GLuint texture_earth;

TgaImage coolImage;
TgaImage blankImage;
TgaImage earthImage;

// Structs that hold the Vertex Array Object index and number of vertices of each shape.
ShapeData cubeData;
ShapeData sphereData;
ShapeData coneData;
ShapeData cylData;

// Matrix stack that can be used to push and pop the modelview matrix.
class MatrixStack {
    int    _index;
    int    _size;
    mat4*  _matrices;

   public:
    MatrixStack( int numMatrices = 32 ):_index(0), _size(numMatrices)
        { _matrices = new mat4[numMatrices]; }

    ~MatrixStack()
	{ delete[]_matrices; }

    void push( const mat4& m ) {
        assert( _index + 1 < _size );
        _matrices[_index++] = m;
    }

    mat4& pop( void ) {
        assert( _index - 1 >= 0 );
        _index--;
        return _matrices[_index];
    }
};

MatrixStack  mvstack;
mat4         model_view;
GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex;

// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye{0, -7.0, 20.0,1.0};
Angel::vec4 ref{0.0, 0.0, 0.0,1.0};
Angel::vec4 up{0.0,1.0,0.0,0.0};

double oldTime = -.1;
double TIME = 0.0 ;

/////////////////////////////////////////////////////
//    PROC: drawCylinder()
//    DOES: this function 
//          render a solid cylinder  oriented along the Z axis. Both bases are of radius 1. 
//          The bases of the cylinder are placed at Z = 0, and at Z = 1.
//
//          
// Don't change.
//////////////////////////////////////////////////////
void drawCylinder(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}

//////////////////////////////////////////////////////
//    PROC: drawCone()
//    DOES: this function 
//          render a solid cone oriented along the Z axis with base radius 1. 
//          The base of the cone is placed at Z = 0, and the top at Z = 1. 
//         
// Don't change.
//////////////////////////////////////////////////////
void drawCone(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawCube()
//    DOES: this function draws a cube with dimensions 1,1,1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawCube(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}


//////////////////////////////////////////////////////
//    PROC: drawSphere()
//    DOES: this function draws a sphere with radius 1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawSphere(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_earth);
    glUniform1i( uEnableTex, 1);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
}


void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}


//////////////////////////////////////////////////////
//    PROC: myKey()
//    DOES: this function gets caled for any keypresses
// 
//////////////////////////////////////////////////////

void myKey(unsigned char key, int x, int y)
{
    float time ;
    switch (key) {
        case 'q':
        case 27:
            exit(0); 
        case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;
        case 'r':
            resetArcball() ;
            glutPostRedisplay() ;
            break ;
        case 'a': // togle animation
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
            time = TM.GetElapsedTime() ;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
            break ;
        case 'm':
            if( Recording == 1 )
            {
                printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();
            break;
    }
    glutPostRedisplay() ;

}

/*********************************************************
    PROC: myinit()
    DOES: performs most of the OpenGL intialization
     -- change these with care, if you must.

**********************************************************/

void myinit(void)
{
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);

    // Generate vertex arrays for geometric shapes
    generateCube(program, &cubeData);
    generateSphere(program, &sphereData);
    generateCone(program, &coneData);
    generateCylinder(program, &cylData);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView       = glGetUniformLocation( program, "View"       );

    glClearColor( 0.1, 0.1, 0.5, 1.0 ); // dark blue background

    uAmbient   = glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse   = glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular  = glGetUniformLocation( program, "SpecularProduct" );
    uLightPos  = glGetUniformLocation( program, "LightPosition"   );
    uShininess = glGetUniformLocation( program, "Shininess"       );
    uTex       = glGetUniformLocation( program, "Tex"             );
    uEnableTex = glGetUniformLocation( program, "EnableTex"       );

    glUniform4f(uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
    glUniform1f(uShininess, 100.0f);

    glEnable(GL_DEPTH_TEST);


    if (!coolImage.loadTGA("challenge.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
    if (!earthImage.loadTGA("earth.tga"))
    {
        printf("Error loading image file\n");
        exit(1);
    }
    
//    // Loading a blank image
//    if (!blankImage.loadTGA("blank.tga"))
//    {
//        printf("Error loading image file\n");
//        exit(1);
//    }

    
//    glGenTextures( 1, &texture_cube );
//    glBindTexture( GL_TEXTURE_2D, texture_cube );
//    
//    glTexImage2D(GL_TEXTURE_2D, 0, 4, coolImage.width, coolImage.height, 0,
//                 (coolImage.byteCount == 3) ? GL_BGR : GL_BGRA,
//                 GL_UNSIGNED_BYTE, coolImage.data );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    
    glGenTextures( 1, &texture_earth );
    glBindTexture( GL_TEXTURE_2D, texture_earth );
    
    glTexImage2D(GL_TEXTURE_2D, 0, 4, earthImage.width, earthImage.height, 0,
                 (earthImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, earthImage.data );
    
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    // Set texture sampler variable to texture unit 0
    // (set in glActiveTexture(GL_TEXTURE0))
    
    glUniform1i( uTex, 0);
    
    Arcball = new BallData;
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

/*********************************************************
    PROC: set_colour();
    DOES: sets all material properties to the given colour
    -- don't change
**********************************************************/

void set_colour(float r, float g, float b)
{
    float ambient  = 0.2f;
    float diffuse  = 0.6f;
    float specular = 0.2f;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1.0f);
}
/*********************************************************
 **********************************************************
 **********************************************************
 
    My additions
 
 **********************************************************
 **********************************************************
 **********************************************************/

void drawGround (float level)
{
    // Store the old model_view
    mvstack.push(model_view);
    
    set_colour(0.0f, 1.0f, 0.0f);
    model_view *= Translate(0.0f, level - 0.05f, 0.0f);
    model_view *= Scale(100.0f, 0.1f, 100.0f);
    drawCube();
    
    set_colour(1.0f, 1.0f, 1.0f);
    // restore old model_view
    model_view = mvstack.pop();
}

void drawPillar (float t)
{
    t *= 2.0f;
    
    mvstack.push(model_view);
    
    float v_offset  = 4.0f;
    float dv_globe  = 2.5f;
    float tot_offset    = v_offset + 1.0f;
    
    model_view *= Translate(0.0f, tot_offset, 0.0f); // so that pillar is drawn from the base, not the center
    
    // Draw top cube
    mvstack.push(model_view);
    model_view *= Translate(0.0f, v_offset, 0.0f);
    model_view *= Scale(2.0f, 2.0f, 2.0f);
    drawCube();
    model_view = mvstack.pop();
    
    // Draw bottom cube
    mvstack.push(model_view);
    model_view *= Translate(0.0f, -v_offset, 0.0f);
    model_view *= Scale(2.0f, 2.0f, 2.0f);
    drawCube();
    model_view = mvstack.pop();
    
    // draw globe
    mvstack.push(model_view);
    model_view *= Translate(0.0f, dv_globe * (sinf(t)), 0.0f);
    model_view *= Scale(1.0f, -(0.5f + fabs(cosf(t) / 2.0f)), 1.0f); // made negative to make globe right-side up.
    model_view *= RotateY(t * 10.0f);
    drawSphere();
    model_view = mvstack.pop();
    
    model_view = mvstack.pop();
}

void drawPillarCircle(float t, float ground_level)
{
    mvstack.push(model_view);
    
    int     num_pillars = 12;
    float   offset      = 360 / num_pillars;
    model_view *= Translate(0.0f, ground_level, 0.0f);
    
    for (int i = 0; i < num_pillars; i++)
    {
        //        if (i == 5)
        //        {
        //            glGenTextures( 1, &texture_cube );
        //            glBindTexture( GL_TEXTURE_2D, texture_cube );
        //
        //            glTexImage2D(GL_TEXTURE_2D, 0, 4, coolImage.width, coolImage.height, 0,
        //                         (coolImage.byteCount == 3) ? GL_BGR : GL_BGRA,
        //                         GL_UNSIGNED_BYTE, coolImage.data );
        //            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        //            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        //            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        //            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        //        }
        //        else
        //        {
        //            glGenTextures( 1, &texture_cube );
        //            glBindTexture( GL_TEXTURE_2D, texture_cube );
        //
        //            glTexImage2D(GL_TEXTURE_2D, 0, 4, blankImage.width, blankImage.height, 0,
        //                         (blankImage.byteCount == 3) ? GL_BGR : GL_BGRA,
        //                         GL_UNSIGNED_BYTE, blankImage.data );
        //            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        //            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        //            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        //            glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        //        }
        
        mvstack.push(model_view);
        model_view *= RotateY(offset * i);
        model_view *= Translate(5.0f, 0.0f, 0.0f);
        drawPillar(TIME*2.0f + (i / 2.0f));
        model_view = mvstack.pop();
    }
    model_view = mvstack.pop();
}

void drawPodium(void)
{
    mvstack.push(model_view);

    model_view *= Translate(0.0f, -5.0f, 0.0f);
    model_view *= RotateX(90.0f);
    model_view *= Scale(2.0f, 2.0f, 5.0f);
    drawCylinder();
    
    model_view = mvstack.pop();
    
}

void drawManBodyTrunk(float height)
{
    set_colour(1.0f, 0.0f, 0.0f);
    //TORSO
    mvstack.push(model_view);
    model_view *= Scale(height * 0.2f, height * 0.2f, height * 0.2f);
    drawCube();
    model_view = mvstack.pop();
    
    set_colour(0.0f, 0.0f, 1.0f);
    //HIPS
    mvstack.push(model_view);
    model_view *= Translate(0.0f, -height * 0.15f, 0.0f);
    model_view *= Scale(height * 0.3f, height * 0.1f, height * 0.2f);
    drawCube();
    model_view = mvstack.pop();
    
    set_colour(1.0f, 0.0f, 0.0f);
    //SHOULDERS(CHEST)
    mvstack.push(model_view);
    model_view *= Translate(0.0f, height * 0.2f, 0.0f);
    model_view *= Scale(height * 0.4f, height * 0.2f, height * 0.2f);
    drawCube();
    model_view = mvstack.pop();
}

void drawManBodyLegs(float height, float progress)
{
    set_colour(0.0f, 0.0f, 1.0f);
    
    //RIGHT UPPER LEG
    mvstack.push(model_view);
    model_view *= Translate(height * 0.075f, -height * 0.275f, 0.0f);
    model_view *= Translate(0.0f, (height * 0.075f) * (ROOT2 * progress) / 4.0f, 0.0f);
    model_view *= RotateX(-45 * progress);
    model_view *= Scale(height * 0.05f, height * 0.15f, height * 0.05f);
    drawCube();
    model_view = mvstack.pop();
    
    //RIGHT LOWER LEG
    mvstack.push(model_view);
    model_view *= Translate(height * 0.075f, -height * 0.425f, 0.0f);
    model_view *= Translate(0.0f, (height * 0.075f) * (ROOT2 * progress) / 2.0f, 0.0f);
    model_view *= RotateX(45 * progress);
    model_view *= Scale(height * 0.05f, height * 0.15f, height * 0.05f);
    drawCube();
    model_view = mvstack.pop();
    
    //LEFT UPPER LEG
    mvstack.push(model_view);
    model_view *= Translate(height * -0.075f, -height * 0.275f, 0.0f);
    model_view *= Translate(0.0f, (height * 0.075f) * (ROOT2 * progress) / 4.0f, 0.0f);
    model_view *= RotateX(-45 * progress);
    model_view *= Scale(height * 0.05f, height * 0.15f, height * 0.05f);
    drawCube();
    model_view = mvstack.pop();
    
    //LEFT LOWER LEG
    mvstack.push(model_view);
    model_view *= Translate(height * -0.075f, -height * 0.425f, 0.0f);
    model_view *= Translate(0.0f, (height * 0.075f) * (ROOT2 * progress) / 2.0f, 0.0f);
    model_view *= RotateX(45 * progress);
    model_view *= Scale(height * 0.05f, height * 0.15f, height * 0.05f);
    drawCube();
    model_view = mvstack.pop();
}

void drawManBodyArms(float height, int action, float progress)
{
    // "action" refers to arm motion: Bending (straightening), raising
    
    set_colour(1.0f, 0.0f, 0.0f);
    
    //RIGHT UPPER ARM
    mvstack.push(model_view);
    if (action == BENDING_ARMS)
        model_view *= Translate(0.0f, (height * 0.075f) * (ROOT2 * progress) / 2.0f, 0.0f);
    else if (action == RAISING_ARMS)
    {
        model_view *= Translate(0.0f, progress * (height * 0.1f), (height * 0.1f) * cosf(-(M_PI / 2)*(1 - progress) + (M_PI / 2) * (progress)));
    }
    model_view *= Translate(height * 0.25f, height * 0.15f, 0.0f);
    if (action == BENDING_ARMS)
        model_view *= RotateX(45 * -progress);
    else if (action == RAISING_ARMS)
    {
        model_view *= RotateX(180 * -progress);
    }
    model_view *= Scale(height * 0.1f, height * 0.2f, height * 0.1f);
    drawCube();
    
    // RIGHT LOWER ARM
    model_view *= Scale(1 / (height * 0.1f), 1 / (height * 0.2f), 1 / (height * 0.1f));
    if (action == BENDING_ARMS)
        model_view *= Translate(0.0f, (height * 0.075f) * (ROOT2 * progress) / 2.0f, 0.0f);
    model_view *= Translate(0.0f, height * -0.2f, 0.0f);
    if (action == BENDING_ARMS)
        model_view *= RotateX(45 * -progress);
    model_view *= Scale(height * 0.05f, height * 0.2f, height * 0.05f);
    drawCube();
    
    model_view = mvstack.pop();

    
    //TODO: Delete this if determined it's not needed
//    //RIGHT LOWER ARM
//    mvstack.push(model_view);
//    model_view *= Translate(height * 0.25f, height * -0.05f, 0.0f);
//    model_view *= Scale(height * 0.05f, height * 0.2f, height * 0.05f);
//    drawCube();
//    model_view = mvstack.pop();
    
    //LEFT UPPER ARM
    mvstack.push(model_view);
    if (action == BENDING_ARMS)
        model_view *= Translate(0.0f, (height * 0.075f) * (ROOT2 * progress) / 2.0f, 0.0f);
    else if (action == RAISING_ARMS)
    {
        model_view *= Translate(0.0f, progress * (height * 0.1f), (height * 0.1f) * cosf(-(M_PI / 2)*(1 - progress) + (M_PI / 2) * (progress)));
    }
    model_view *= Translate(height * -0.25f, height * 0.15f, 0.0f);
    if (action == BENDING_ARMS)
        model_view *= RotateX(45 * -progress);
    else if (action == RAISING_ARMS)
    {
        model_view *= RotateX(180 * -progress);
    }
    model_view *= Scale(height * 0.1f, height * 0.2f, height * 0.1f);
    drawCube();
    
    //LEFT LOWER ARM
    model_view *= Scale(1 / (height * 0.1f), 1 / (height * 0.2f), 1 / (height * 0.1f));
    if (action == BENDING_ARMS)
        model_view *= Translate(0.0f, (height * 0.075f) * (ROOT2 * progress) / 2.0f, 0.0f);
    model_view *= Translate(0.0f, height * -0.2f, 0.0f);
    if (action == BENDING_ARMS)
        model_view *= RotateX(45 * -progress);
    model_view *= Scale(height * 0.05f, height * 0.2f, height * 0.05f);
    drawCube();
    
    model_view = mvstack.pop();

    //TODO: Delete this if determined it's not needed
//    //LEFT LOWER ARM
//    mvstack.push(model_view);
//    model_view *= Translate(height * -0.25f, height * -0.05f, 0.0f);
//    model_view *= Scale(height * 0.05f, height * 0.2f, height * 0.05f);
//    drawCube();
//    model_view = mvstack.pop();
}

void drawManBodyArms(float height)
{
    drawManBodyArms(height, STILL_ARMS, 0);
}

void drawManBodyHead(float height)
{
    set_colour(0.0f, 0.0f, 0.0f);
    
    mvstack.push(model_view);
    model_view *= Translate(0.0f, height * 0.375f, 0.0f);
    model_view *= Scale(height * 0.075f, height * 0.075f, height * 0.075f);
    drawSphere();
    model_view = mvstack.pop();
}

void drawManBodyTemp(float height)
{
    mvstack.push(model_view);
    
    drawManBodyTrunk(height);
    drawManBodyLegs(height, 0);
    drawManBodyArms(height, BENDING_ARMS, 1.0f);
    drawManBodyHead(height);
    
    model_view = mvstack.pop();
}

void drawManBodyFlying(float height, float progress)
{
    mvstack.push(model_view);
    
    drawManBodyTrunk(height);
    drawManBodyLegs(height, 0);
    drawManBodyArms(height, BENDING_ARMS, 1 - progress);
    drawManBodyHead(height);
    
    model_view = mvstack.pop();
}

void drawManSquatting(float height, float progress)
{
    mvstack.push(model_view);
    model_view *= Translate(0.0f, -(height * 0.075f) * (ROOT2 * progress) / 2.0f, 0.0f);
    drawManBodyTrunk(height);
    drawManBodyLegs(height, progress);
    drawManBodyArms(height, BENDING_ARMS, 1 - progress);
    drawManBodyHead(height);
    
    model_view = mvstack.pop();
}
void drawManRaisingArms(float height, float progress)
{
    mvstack.push(model_view);
    
    drawManBodyTrunk(height);
    drawManBodyLegs(height, 0);
    drawManBodyArms(height, RAISING_ARMS, progress);
    drawManBodyHead(height);
    
    model_view = mvstack.pop();
}

void drawManRaisedArms(float height)
{
    mvstack.push(model_view);
    
    drawManBodyTrunk(height);
    drawManBodyLegs(height, 0);
    drawManBodyArms(height, RAISING_ARMS, 1.0f);
    drawManBodyHead(height);
    
    model_view = mvstack.pop();
}

void drawManBody(float height, int status, float progress)
{
    mvstack.push(model_view);
    set_colour(0.0f, 0.0f, 0.0f);
//    model_view *= Translate(0.0f, height/2.0f, 0.0f);
    
    switch (status)
    {
        case 1: drawManSquatting(height, progress); break;
        case 2: drawManSquatting(height, 1 - progress); break;
        case 3: drawManBodyFlying(height, progress); break;
        case 4: drawManRaisingArms(height, progress); break;
        case 5: drawManRaisedArms(height); break;
        default:
            drawManBodyTemp(height);
    }
    
    model_view = mvstack.pop();
}

void drawGlobe(float height, float progress)
{
    mvstack.push(model_view);
    
    float initial_y = 40.0f;
    float final_y = 12.0f + height / 2.0f;
    
    model_view *= Translate(0.0f, initial_y * (1 - progress) + final_y * progress, 0.0f);
    model_view *= RotateX(180);
    model_view *= Scale(12.0f);
    drawSphere();
    
    model_view = mvstack.pop();
}

double parametric(double start, double end, float progress)
{
    return start * (1 - progress) + (end * progress);
}

void drawMan(float ground_level)
{
    // PLAN
    //  1: Squat until time = 2
    //  2: extend legs for 2<time<2.5
    //  3: Jump and back flip for 2.5<time<8.5
    //     Land at time=8.5
    //  4: Put hands up 8.5<time<9.5
    //  5: Catch globe when world = 9.5 (hands up)
    
    float height = 4.0f;
    float progress = 0.0f;
    
    float squatEnd = 2.0f;
    float extendEnd = 2.1f;
    float jumpEnd = 8.5f;
    float reachEnd = 9.5f;
    
    float initial_z = 15.0f;
    float initial_y = ground_level + height/2.0f;
    
    int status = 0;
    
    mvstack.push(model_view);
    set_colour(0.0f, 0.0f, 0.0f);

    if (TIME <= 0.0f)   // Initial
    {
        progress = 0.0f;
        status = 0;
        model_view *= Translate(0.0f, initial_y, initial_z);
    }
    else if (TIME > 0.0f && TIME <= squatEnd)   // Squatting
    {
        progress = TIME / squatEnd;
        status = 1;
        model_view *= Translate(0.0f, initial_y, initial_z);
        
        eye.y = parametric(-7.0, -5.0, progress);
        eye.z = parametric(20.0, 30.0, progress);
    }
    else if (TIME > squatEnd && TIME <= extendEnd)  // extending legs
    {
        progress = (TIME - squatEnd) / (extendEnd - squatEnd);
        status = 2;
        model_view *= Translate(0.0f, initial_y, initial_z);
    }
    else if (TIME > extendEnd && TIME <= jumpEnd)
    {
        progress = (TIME - extendEnd) / (jumpEnd - extendEnd);
        status = 3;
        int numBackflips = 5;
        float degrees = -numBackflips * 360 * progress;
        
        if (progress < .5f)
        {
            model_view *= Translate(0.0f, parametric(initial_y, 4.0f, progress * 2.0f), initial_z - (initial_z * progress));
        }
        else
        {
            model_view *= Translate(0.0f, parametric(4.0f, height / 2.0f, (progress - .5f) * 2.0f), initial_z - (initial_z * progress));
        }
//        model_view *= Translate(0.0f, initial_y * (1 - progress) + (height / 2.0f * progress), initial_z - (initial_z * progress));
        model_view *= RotateX(degrees);
        
        eye.y = parametric(-5.0, 3.0, progress);
        eye.x = parametric(30.0, 10.0, progress) * sinf(progress * 2 * M_PI);
        eye.z = parametric(30.0, 10.0, progress) * cosf(progress * 2 * M_PI);
        
        ref.y = parametric(0.0, 3.0, progress);
    }
    else if (TIME > jumpEnd && TIME <= reachEnd)
    {
        progress = (TIME - jumpEnd) / (reachEnd - jumpEnd);
        status = 4;
        model_view *= Translate(0.0f, height / 2.0f, 0.0f);
        set_colour(1.0f, 1.0f, 1.0f);
        drawGlobe(height, progress);
        
        eye.y = parametric(3.0, -5.0, progress);
        eye.z = parametric(10.0, 25.0, progress);
    }
    else if (TIME > reachEnd)
    {
        // he stands there holding the globe
        status = 5;
        model_view *= Translate(0.0f, height / 2.0f, 0.0f);
        set_colour(1.0f, 1.0f, 1.0f);
        drawGlobe(height, 1.0f);
    }
    else
    {
        progress = 0.0f;
        status = 0;
        model_view *= Translate(0.0f, height / 2.0f, 0.0f);
    }
    set_colour(0.0f, 0.0f, 0.0f);
    drawManBody(height, status, progress);
    
    model_view = mvstack.pop();
}

/*********************************************************
**********************************************************
**********************************************************

    PROC: display()
    DOES: this gets called by the event handler to draw
          the scene, so this is where you need to build
          your ROBOT --  
      
        MAKE YOUR CHANGES AND ADDITIONS HERE

    Add other procedures if you like.

**********************************************************
**********************************************************
**********************************************************/
void display(void)
{
    // Clear the screen with the background colour (set in myinit)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    model_view = mat4(1.0f);
    
    
    model_view *= Translate(0.0f, 0.0f, -15.0f);
    HMatrix r;
    Ball_Value(Arcball,r);

    mat4 mat_arcball_rot(
        r[0][0], r[0][1], r[0][2], r[0][3],
        r[1][0], r[1][1], r[1][2], r[1][3],
        r[2][0], r[2][1], r[2][2], r[2][3],
        r[3][0], r[3][1], r[3][2], r[3][3]);
    model_view *= mat_arcball_rot;
    
    mat4 view = model_view;
    
    model_view = Angel::LookAt(eye, ref, up);//just the view matrix;

    glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );

    // Previously glScalef(Zoom, Zoom, Zoom);
    model_view *= Scale(Zoom);

    mvstack.push(model_view);
    
    float ground_level = -10.0f;
    
    drawGround(ground_level);
    drawPillarCircle(TIME, ground_level);
    drawPodium();
    drawMan(ground_level);
    
    model_view = mvstack.pop();

    glutSwapBuffers();
    if(Recording == 1)
        FrSaver.DumpPPM(Width, Height) ;
}

/**********************************************
    PROC: myReshape()
    DOES: handles the window being resized 
    
      -- don't change
**********************************************************/

void myReshape(int w, int h)
{
    Width = w;
    Height = h;

    glViewport(0, 0, w, h);

    mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
}

void instructions() 
{
    printf("Press:\n");
    printf("  s to save the image\n");
    printf("  r to restore the original view.\n") ;
    printf("  0 to set it to the zero state.\n") ;
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
}

// start or end interaction
void myMouseCB(int button, int state, int x, int y)
{
    Button = button ;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width-1.0;
        arcball_coords.y = -2.0*(float)y/(float)Height+1.0;
        Ball_Mouse(Arcball, arcball_coords) ;
        Ball_Update(Arcball);
        Ball_BeginDrag(Arcball);

    }
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
        Ball_EndDrag(Arcball);
        Button = -1 ;
    }
    if( Button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        PrevY = y ;
    }


    // Tell the system to redraw the window
    glutPostRedisplay() ;
}

// interaction (mouse motion)
void myMotionCB(int x, int y)
{
    if( Button == GLUT_LEFT_BUTTON )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width - 1.0 ;
        arcball_coords.y = -2.0*(float)y/(float)Height + 1.0 ;
        Ball_Mouse(Arcball,arcball_coords);
        Ball_Update(Arcball);
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Zoom  = Zoom * 1.03 ;
        else 
            Zoom  = Zoom * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}


void idleCB(void)
{
    double framerate;
    if( Animate == 1 )
    {
        float deltaTime = 0.033f;
        
        // TM.Reset() ; // commenting out this will make the time run from 0
        // leaving 'Time' counts the time interval between successive calls to idleCB
        
        
        if( Recording == 0 )
            TIME = TM.GetElapsedTime() ;
        else
            TIME += deltaTime ; // save at 30 frames per second.
        
        framerate = 1.0 / (TIME - oldTime);
        oldTime = TIME;
        
//        eye.x = 50*sin(TIME);
//        eye.z = 50*cos(TIME);
        
        printf("TIME %f, recording frame rate = %f, standard frame rate = %f\n", TIME, 1.0f/deltaTime, framerate) ;
        glutPostRedisplay() ;
    }
}
/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    // If your code fails to run, uncommenting these lines may help.
    //glutInitContextVersion(3, 2);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Width,Height);
    glutCreateWindow(argv[0]);
    printf("GL version %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();
    
    myinit();

    glutIdleFunc(idleCB) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}




