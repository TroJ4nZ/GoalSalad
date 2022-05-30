#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

// Macro to get array size
#define SPAWN_ARRAY_SIZE(spawnPoints) (sizeof((spawnPoints)) / sizeof((spawnPoints[0])))
// Window H/W
double windowHeight=800, windowWidth=600;

// For lookAt Function, to allow to look left and right.
double eyeX=5.0, eyeY=2.0, eyeZ=15.0, refX = 5, refY=0,refZ=0;

// Speed of rotation and moving of sun
double sunRotate = 0;
double sunToLeft = 2;

// Shoot determines if game in action or not
bool gameInAction = false;
// Scores for player 1/2
float scoreVal1 = 0;
float scoreVal2 = 0;
// Win -> 1: end game, -1 -> lose, 0->still playing
int win = 0;

// Determine speed of ball (difficulty)
double ballZIncrement1=0.05;
// Determine left and right of ball (aiming)
double ballRefX1=0;
// Start position of Ball when game is reset
double ballZ1 = 13;


/// Ball 2
double ballZIncrement2=0.05;
double ballRefX2=0;
double ballZ2 = 13;

// 0 menu /// 1 single /// 2 multi
int state = 0;

/// x-y-z commonly used for making cubes and quads manually.
static GLfloat cubeIndices[8][3] = {
    {0.0, 0.0, 0.0}, //0
    {0.0, 0.0, 3.0}, //1
    {3.0, 0.0, 3.0}, //2
    {3.0, 0.0, 0.0}, //3
    {0.0, 3.0, 0.0}, //4
    {0.0, 3.0, 3.0}, //5
    {3.0, 3.0, 3.0}, //6
    {3.0, 3.0, 0.0}  //7
};
static GLubyte quadIndices[6][4] = {
    {0, 1, 2, 3}, //bottom
    {4, 5, 6, 7}, //top
    {5, 1, 2, 6}, //front
    {0, 4, 7, 3}, // back is clockwise
    {2, 3, 7, 6}, //right
    {1, 5, 4, 0}  //left is clockwise
};


// To load BMP file as a texture GL element
GLuint LoadTexture(GLuint lighting, const char* filename, int w, int h) {
    int width, height;
    unsigned char * data;
    FILE *file;
    file = fopen(filename, "rb");
    if (file == NULL)
        return 0;
    width=w;
    height=h;
    data = (unsigned char *)malloc( width * height * 3 );
    fread(data, width * height * 3, 1, file);
    fclose(file);
    for(int i = 0; i < width * height ; ++i) {
        int index = i*3;
        unsigned char B,R;
        B = data[index];
        R = data[index+2];
        data[index] = R;
        data[index+2] = B;
    }
    glGenTextures( 1, &lighting );
    glBindTexture( GL_TEXTURE_2D, lighting );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT );
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,GL_RGB, GL_UNSIGNED_BYTE, data );
    free(data);
    return lighting;
}

// Initialize the BMP as textures, for football and for brick wall
void init(void) {
    GLuint lighting;
    lighting = LoadTexture(lighting, "foot.bmp", 800,800);
    lighting = LoadTexture(lighting, "brick4.bmp", 526,512);
}

// Draws a cube without any textures given the quad indexes
void drawCube() {
    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++) {
        glVertex3fv(&cubeIndices[quadIndices[i][0]][0]);
        glVertex3fv(&cubeIndices[quadIndices[i][1]][0]);
        glVertex3fv(&cubeIndices[quadIndices[i][2]][0]);
        glVertex3fv(&cubeIndices[quadIndices[i][3]][0]);
    }
    glEnd();
}


// To draw front and left wall but bind textures with it -- not a normal cube
void drawCubeWallTextured() {
    // Disable color mode
    glDisable(GL_COLOR);
    glEnable(GL_TEXTURE_2D);
    // Bind the brick.bmp loaded texture
    glBindTexture(GL_TEXTURE_2D, 2);
    // Start drawing QUAD left-top-right-bottom
    /// with alternating the tex coordinates (TexCoord2f)
    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex3fv(&cubeIndices[quadIndices[2][2]][0]);
    glTexCoord2f(0.0,2);
    glVertex3fv(&cubeIndices[quadIndices[2][3]][0]);
    glTexCoord2f(2,2);
    glVertex3fv(&cubeIndices[quadIndices[2][0]][0]);
    glTexCoord2f(2,0.0);
    glVertex3fv(&cubeIndices[quadIndices[2][1]][0]);
    glEnd();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex3fv(&cubeIndices[quadIndices[4][3]][0]);
    glTexCoord2f(0.0,2.0);
    glVertex3fv(&cubeIndices[quadIndices[4][0]][0]);
    glTexCoord2f(2.0,2.0);
    glVertex3fv(&cubeIndices[quadIndices[4][1]][0]);
    glTexCoord2f(2.0,0.0);
    glVertex3fv(&cubeIndices[quadIndices[4][2]][0]);
    glEnd();

    // Disable texture mode and back to color
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR);
}



// Draws the ground and walls
void base() {
/// Draw cube but textured 3 times to prevent texture bluriness
    // Front Wall
    glColor3f(110/255.0, 38/255.0, 14/255.0);
    glPushMatrix();
    glTranslatef(-2.2,-0.79,.5);
    glScalef(2, 2, 0.1);
    glRotatef(4.5,1,1,0);
    drawCubeWallTextured();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(3.78,-0.79,.5);
    glScalef(2, 2, 0.1);
    glRotatef(4.5,1,1,0);
    drawCubeWallTextured();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(8,-0.79,.5);
    glScalef(2, 2, 0.1);
    glRotatef(4.5,1,1,0);
    drawCubeWallTextured();
    glPopMatrix();


    // Left wall
    glColor3f(149/255.0, 69/255.0, 53/255.0);
    glPushMatrix();
    glTranslatef(-4.5,-1,0);
    glScalef(1, 2, 5);
    drawCubeWallTextured();
    glPopMatrix();



    // Sky
    glColor3f(135/255.0, 206/255.0, 235/255.0);
    glPushMatrix();
    glTranslatef(5,5.1,10);
    glBegin(GL_QUADS);
    glVertex3f(-10,00,-20);
    glVertex3f(10,00,-20);
    glVertex3f(10,00,10);
    glVertex3f(-10,00,10);
    glEnd();
    glPopMatrix();


    // Middle Dark Green Floor
    glColor3f(2/255.0, 48/255.0, 32/255.0);
    glPushMatrix();
    glTranslatef(3,-0.2,7);
    glScalef(1.3, 0.01, 1.7);
    drawCube();
    glPopMatrix();
}


void floor(void) {
    // Outer Light Green Floor

    glColor3f(45/255.0, 78/255.0, 59/255.0);
    glPushMatrix();
    glScalef(6.5, 0.1, 7);
    glTranslatef(-1,-5,0);
    drawCube();
    glPopMatrix();
}

void ball1(void) {
    glPushMatrix();
    /// BallRefX1 state the left/right of ball and Z to move ball as game progresses
    glTranslatef(5+ballRefX1, 0, ballZ1);
    /// BallZ1 to rotate as ball moves for really immersive cinematic effect.
    glRotatef(1+ballZ1*50, 1,0,0);
    glRotatef(1, 0, 1, 0);
    glRotatef(1, 0, 0, 1);
    // Create and texture the ball
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR);
    glBindTexture(GL_TEXTURE_2D, 1);

    glColor3f(1, 1, 1);
    /// Create Sphere Object and attach texture
    GLUquadricObj* BallObj1 = gluNewQuadric();
    gluQuadricDrawStyle(BallObj1, GLU_FILL);
    gluQuadricNormals(BallObj1, GLU_SMOOTH);
    gluQuadricOrientation(BallObj1, GLU_OUTSIDE);
    gluQuadricTexture(BallObj1, GL_TRUE);

    gluSphere(BallObj1, 0.2, 50, 35);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR);
    glPopMatrix();
}



void ball2(void) {
    glPushMatrix();
    glTranslatef(5+ballRefX2, 0, ballZ2);
    glRotatef(1+ballZ2*50, 1,0,0);
    glRotatef(1, 0, 1, 0);
    glRotatef(1, 0, 0, 1);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_COLOR);
    glBindTexture(GL_TEXTURE_2D, 1);
    glColor3f(1, 1, 1);
    GLUquadricObj* BallObj2 = gluNewQuadric();
    gluQuadricDrawStyle(BallObj2, GLU_FILL);
    gluQuadricNormals(BallObj2, GLU_SMOOTH);
    gluQuadricOrientation(BallObj2, GLU_OUTSIDE);
    gluQuadricTexture(BallObj2, GL_TRUE);
    gluSphere(BallObj2, 0.2, 50, 35);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR);
    glPopMatrix();
}


// Draw the drawer to show score
/// Used CPM (The whole drawer is drawn manually without built-in function
void wardrobe() {
    double leftX = -3;
    glRotated(-25,0,1,0);
    // Drawer Base
    glColor3f(0.3-leftX,0.1,0);
    glPushMatrix();
    glTranslatef(0 - leftX,0,4);
    glScalef(0.12, 0.6, 0.4);
    drawCube();
    glPopMatrix();

    // Drawer's 1st drawer
    glColor3f(0.5,0.2,0.2);
    glPushMatrix();
    glTranslatef(0.36  - leftX,1.4,4.05);
    glScalef(0.0001, 0.11, 0.38);
    drawCube();
    glPopMatrix();

    //Drawer's 2nd drawer
    glColor3f(0.5,0.2,0.2);
    glPushMatrix();
    glTranslatef(0.36 - leftX,1,4.05);
    glScalef(0.0001, 0.11, 0.38);
    drawCube();
    glPopMatrix();

    //Drawer's 3rd drawer
    glColor3f(0.5,0.2,0.2);
    glPushMatrix();
    glTranslatef(0.36 - leftX,0.6,4.05);
    glScalef(0.0001, 0.11, 0.38);
    drawCube();
    glPopMatrix();

    //Drawer's 4th drawer
    glColor3f(0.5,0.2,0.2);
    glPushMatrix();
    glTranslatef(0.36 - leftX,0.2,4.05);
    glScalef(0.0001, 0.11, 0.38);
    drawCube();
    glPopMatrix();

    //Drawer's 1st drawer handle
    glColor3f(0.3,0.1,0);
    glPushMatrix();
    glTranslatef(0.37 - leftX,1.5,4.3);
    glScalef(0.0001, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //Drawer's 2nd drawer handle
    glColor3f(0.3,0.1,0);
    glPushMatrix();
    glTranslatef(0.37 - leftX,1.1,4.3);
    glScalef(0.0001, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //Drawer's 3rd drawer handle
    glColor3f(0.3,0.1,0);
    glPushMatrix();
    glTranslatef(0.37 - leftX,0.7,4.3);
    glScalef(0.0001, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    //Drawer's 4th drawer handle
    glColor3f(0.3,0.1,0);
    glPushMatrix();
    glTranslatef(0.37 - leftX,0.3,4.3);
    glScalef(0.0001, 0.03, 0.2);
    drawCube();
    glPopMatrix();

    // Drawer border
    glColor3f(0.5,0.2,0.2);
    glPushMatrix();
    glTranslatef(0.35 - leftX,0,5.3);
    glScalef(0.01, 0.6, 0.0001);
    drawCube();
    glPopMatrix();
}

void sun() {
    glColor3f(249/255.0, 215/255.0, 28/255.0);
    glPushMatrix();
    glTranslatef(11+sunToLeft,3.2 - sunToLeft/6.5,5);
    glRotatef(sunRotate,1,1,1);
    glRotatef(45,1,0,0);
    glutSolidSphere(0.5, 25, 25);
    glPopMatrix();

}

// Uses 3 cylinder built in objects to make 3 poles
void goal() {
    GLUquadricObj* goalPostTop = gluNewQuadric();
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(7.8,2,2.5);
    glScalef(1,0.5,1);
    glRotatef(295,0,1,0);
    glScalef(1,1,0.6);
    gluCylinder(goalPostTop, 0.15, 0.15, 4, 20, 20);
    glPopMatrix();

    GLUquadricObj* goalPostRight = gluNewQuadric();
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(5.5,2,3);
    glScalef(1,0.5,1);
    glRotatef(90,1,0,0);
    gluCylinder(goalPostRight, 0.15, 0.15, 4, 20, 20);
    glPopMatrix( );


    GLUquadricObj* goalPostLeft = gluNewQuadric();
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(7.5,2,2.2);
    glScalef(1,0.5,1);
    glRotatef(90,1,0,0);
    gluCylinder(goalPostRight, 0.15, 0.1, 4, 20, 20);
    glPopMatrix();
}


// Draw net as 4 lines behind goal
void goalnet(void) {
    glColor3f(200/255.0,200/255.0,200/255.0);
    glPushMatrix();
    glTranslatef(6.3,0.7,2.5);
    glScalef(1.2,1,1);
    glBegin(GL_LINES);
    glVertex3f(-1,1,0);
    glVertex3f(1,1,0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.3,0.4,2.5);
    glScalef(1.2,1,1);
    glBegin(GL_LINES);
    glVertex3f(-1,1,0);
    glVertex3f(1,1,0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(6.3,-0.0,2.5);
    glScalef(1.2,1,1);
    glBegin(GL_LINES);
    glVertex3f(-1,1,0);
    glVertex3f(1,1,0);
    glEnd();
    glPopMatrix();


    glPushMatrix();
    glTranslatef(6.3,-0.4,2.5);
    glScalef(1.2,1,1);
    glBegin(GL_LINES);
    glVertex3f(-1,1,0);
    glVertex3f(1,1,0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(6.3,-0.7,2.5);
    glScalef(1.2,1,1);
    glBegin(GL_LINES);
    glVertex3f(-1,1,0);
    glVertex3f(1,1,0);
    glEnd();
    glPopMatrix();
}

// 2D Flag triangle for off-side of the pitch to draw immersion for the player so as to feel that they are playing as Neymar
void flagTriangle() {
    glPushMatrix();
    glColor3f(255/255.0,49/255.0,49/255.0);
    glTranslatef(11.08,1.78,-1.5);
    glRotatef(-45,0,1,0);
    glScalef(0.15,0.15,0.15);
    glBegin(GL_TRIANGLES);
    glVertex3f(5,3,0);
    glVertex3f(0,6,0);
    glVertex3f(0,0,0);
    glEnd();
    glPopMatrix();
}

// Cylinderical pole for the immersive triangle that the player feels as if he is Neymar but sometimes Messi and maybe Salah if you like him
void flagPole() {
    GLUquadricObj* flagPoleObj = gluNewQuadric();
    glColor3f(1,1,1);
    glPushMatrix();
    glTranslatef(11.1,2,-2);
    glRotatef(90,1,0,0);
    glRotatef(35,0,0,1);
    glScalef(0.8,0.8,0.8);
    gluCylinder(flagPoleObj, 0.15, 0.15, 4, 20, 20);
    glPopMatrix( );
}

// Calc score converts the float score from float to text so that it can be printed
char *calcScore1() {
    char *score;
    // gcvt converts float to string -- floatval, numOfDigits, convertedString
    gcvt(scoreVal1, 3, score);
    return score;
}
// For player 2
char *calcScore2() {
    char *score;
    gcvt(scoreVal2, 3, score);
    return score;
}

// Takes in string and loops on it character by character to print it on the viewport
// takes in the x,y,z coordinates and sets font to HELVETICA_18
void printScore1(char *string, float x, float y, float z) {
    int font=(int)GLUT_BITMAP_HELVETICA_18;
    char *c;
    glRasterPos3f(x,y,z);
    // . prevents the printing of floating point --Amr Ibrahim SolIIIIIIIIIIIIIman
    for(c=string; *c!='.' ; c++) {
        glutBitmapCharacter((void *) font, *c);
    }
}

// For player 2
void printScore2(char *string, float x, float y, float z) {
    int font=(int)GLUT_BITMAP_HELVETICA_18;
    char *c;
    glRasterPos3f(x,y,z);
    for(c=string; *c!='.' ; c++) {
        glutBitmapCharacter((void *) font, *c);
    }
}

// Takes in win variable and ends game or resumes depending on if user won or lost
void endGame(int win, float x, float y, float z) {
    int font=(int)GLUT_BITMAP_HELVETICA_18;
    char *c;
    glRasterPos3f(x,y,z);
    if(win==1 && state == 1) {

        for(c="YOU WIN!!!"; *c!='\0' ; c++) {
            glutBitmapCharacter((void *) font, *c);
        }

        gameInAction = false;

    } else if(win==-1 && state == 1) {
        for(c="YOU LOSE!!!"; *c!='\0' ; c++) {
            glutBitmapCharacter((void *) font, *c);
        }
        gameInAction = false;
    }
    /// in case multi-player
    glRasterPos3f(x,y,z);
    if(win==1 && state == 2 && scoreVal1 == 5) {

        for(c="LEFT PLAYER WINS!!!"; *c!='\0' ; c++) {
            glutBitmapCharacter((void *) font, *c);
        }
        gameInAction = false;

    } else if(win==-1 && state == 2 && scoreVal2 == 5) {
        for(c="RIGHT PLAYER WINS!!!"; *c!='\0' ; c++) {
            glutBitmapCharacter((void *) font, *c);
        }
        gameInAction = false;
    }
}

// To display text of main menu
void printMainMenuText(float x, float y) {
    int font=(int)GLUT_BITMAP_HELVETICA_18; // set font
    // set color to green
    glColor3f(0.5,1,0.5);
    char *c = "WELCOME TO GOAL SALAD!\nq: Main Menu\nw: Single-Player\ne:Multi-Player\n\n\na: Left Player 1\nd: Right Player 1\nj: Left Player 2\nk: Right Player 2\n\nb: Start\nn: RESET\n\n\ny: Easy\nu: Medium (Default)\ni: Hard\no:INSANE\np: IMPOSSIBLE!\n\nEscape: EXIT";
    // Reset X to be in middle
    x = 0.33;
    glRasterPos2f(x,y);
    for(; *c!='\0' ; c++) {
        if(*c=='\n') {
            x = 0.41;
            glRasterPos2f(x,y-=0.04);
        }
        glutBitmapCharacter((void *) font, *c);
    }
    // set x and y to be at the most left
    x = y = 0.02;
    font=(int)GLUT_BITMAP_HELVETICA_12;
    char *s = "Made By: TroJanZ";
    glRasterPos2f(x,y);
    for(; *s!='\0' ; s++) {
        glutBitmapCharacter((void *) font, *s);
    }
}
// Chooses from 43 different spawns for the ball and randomizes one spawn using the modulo operator
void generateRandomSpawn() {
    static const int spawnPoints[43] = {-21,-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21};
    int spawnIndex;
    // 0 to 43 --> 44 technique
    // For player 1
    spawnIndex = rand() % SPAWN_ARRAY_SIZE(spawnPoints);
    ballRefX1 = spawnPoints[spawnIndex] / 10.0;

    // for player 2
    spawnIndex = rand() % SPAWN_ARRAY_SIZE(spawnPoints);
    ballRefX2 = spawnPoints[spawnIndex] / 10.0;
}

void displayMainMenu() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Ortho2D - 2D
    // left right bottom top
    gluOrtho2D(0, 1, 0, 1);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    // Display the starting menu
    printMainMenuText(0.41,0.95);
    glFlush();
    glutSwapBuffers();
}

void displaySinglePlayer(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    // FOV, ASPECT RATIO, Z NEAR, Z FAR
    gluPerspective(60,1,1,100);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    // Allows for dynamic looking left and right
    gluLookAt(eyeX,1,15,  refX,0,0,  0,1,0);

    glViewport(0, 0, 800, 600);
    // Draw all shapes
    base();
    floor();
    ball1();
    wardrobe();
    sun();
    flagTriangle();
    flagPole();
    goal();
    goalnet();
    // PrintScore prints the string returned by CalcScore which was a float value but then converted to string for printing
    printScore1(calcScore1(), 3.7,2, 5);
    // End or resume depending on win value
    endGame(win,3,2.5,5);

    glFlush();
    glutSwapBuffers();
}


void displayMultiPlayer(void) {
    /// LEFT SPLIT
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(60,1,1,100);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt(eyeX,1,15,  refX,0,0,  0,1,0); //7,2,15, 0,0,0, 0,1,0
    glViewport(0, 0, 400, 600);
    base();
    floor();
    ball1();
    wardrobe();
    sun();
    flagTriangle();
    flagPole();
    goal();
    goalnet();
    printScore1(calcScore1(), 3.7,2, 5);
    endGame(win,3,2.5,5);
    glFlush();

    /// RIGHT SPLIT
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(60,1,1,100);
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt(eyeX,1,15,  refX,0,0,  0,1,0); //7,2,15, 0,0,0, 0,1,0
    glViewport(400, 0, 400, 600);
    base();
    floor();
    ball2();
    wardrobe();
    sun();
    flagTriangle();
    flagPole();
    goal();
    goalnet();
    printScore2(calcScore2(), 3.7,2, 5);
    endGame(win,3,2.5,5);
    glFlush();
    glutSwapBuffers();
}

void myKeyboardFunc(unsigned char key, int x, int y ) {
    /// Choices of STATE, q->0 MAIN MENU, w->1 SINGLE PLAYER, e->2 MULTIPLAYER
    switch(key) {
    case'q':
        state = 0;
        PlaySound(NULL,0,0);
        break;
    case'w':
        state = 1;
        PlaySound("gametheme.wav", NULL, SND_ASYNC);
        break;
    case'e':
        state = 2;
        PlaySound("gametheme.wav", NULL, SND_ASYNC);
        break;
    case 27:    // Escape key
        exit(1);
    }

    /// USED TO SWITCH SCENES
    if (state == 0) {
        glutDisplayFunc(displayMainMenu);
    } else if (state == 1) {
        glutDisplayFunc(displaySinglePlayer);
    }    else if (state == 2) {
        glutDisplayFunc(displayMultiPlayer);
    }


    /// CONTROLS DIFFICULTY
    // y u i o p ----> difficulty
    switch ( key ) {
    case'y':
        ballZIncrement1= ballZIncrement2=0.01;
        scoreVal1=0;
        scoreVal2=0;
        gameInAction=false;
        ballZ1=ballZ2=13.0;
        generateRandomSpawn();
        break;

    case'u':
        ballZIncrement1=ballZIncrement2=0.05; //inc speed of ball
        scoreVal1=0; // reset the scores
        scoreVal2=0;
        gameInAction=false; //restart game
        ballZ1=ballZ2=13.0; // reset ball place
        generateRandomSpawn(); // put balls in new spawn
        break;
    case'i':
        ballZIncrement1=ballZIncrement2=0.1;
        scoreVal1=0;
        scoreVal2=0;
        gameInAction=false;
        ballZ1=ballZ2=13.0;
        generateRandomSpawn();
        break;

    case'o':
        ballZIncrement1=ballZIncrement2=0.2;
        scoreVal1=0;
        scoreVal2=0;
        gameInAction=false;
        ballZ1=ballZ2=13.0;
        generateRandomSpawn();
        break;

    case'p':
        ballZIncrement1=ballZIncrement2=0.4;
        scoreVal1=0;
        scoreVal2=0;
        gameInAction=false;
        ballZ1=ballZ2=13.0;
        generateRandomSpawn();
        break;

 /// CONTROLS FOR PLAYING
 // player1
    case 'a':
        ballRefX1-=0.2;
        break;
    case 'd':
        ballRefX1+=0.2;
        break;
        // player 2
    case 'j':
        ballRefX2-=0.2;
        break;
    case 'l':
        ballRefX2+=0.2;
        break;
        /// START GAME!
    case 'b':
        scoreVal1=0;
        gameInAction = true;
        break;
        /// RESET GAME!
    case'n':
        scoreVal1=0;
        scoreVal2=0;
        gameInAction=false;
        ballZ1=13.0;
        ballZ2=13.0;
        break;
/// FOR DEBUGGING LOOKING LEFT AND RIGHT
    case'z':
        refX-=0.1;
        break;
    case'x':
        refX+=0.1;
        break;
    case 'c': // move eye point left along X axis
        eyeX-=1.0;
        break;
    case 'v': // move eye point right along X axis
        eyeX+=1.0;
        break;
        glutPostRedisplay();

    }
}



/// ANIMATE USED FOR ANIMATION
void animate() {
    /// IF SINGLE PLAYER WE NEED TO MOVE BALL 1 ONLY!
    if (state == 1) {
        if(gameInAction == true) {
                /// MOVE BALL FURTHER IN FRONT AS LONG AS GAME IS IN ACTION
            ballZ1 -= ballZIncrement1;
        }
        /// IF BALL IS IN GOAL, +1 SCORE
        if(ballZ1 < 5 && ballRefX1 > -1 && ballRefX1 < 0.7) {
            ballZ1=13.0;
            gameInAction=true;
            generateRandomSpawn();
            scoreVal1++;
        }
        /// IF BALL IS IN RIGHT POST, SCORE NO CHANGE
        else if(ballZ1 < 5 && (ballRefX1 == 0.7 || ballRefX1 == 0.8 || ballRefX1 == 0.9)) {
            ballZ1=13.0;
            gameInAction = true;
            generateRandomSpawn();
        }
        /// IF BALL IS IN LEFT POST, SCORE NO CHANGE
        else if(ballZ1 < 5 && (ballRefX1 == -1 || ballRefX1 == -1.1 || ballRefX1 == -1.2)) {
            ballZ1=13.0;
            gameInAction = true;
            generateRandomSpawn();
        }
        /// IF BALL IS OUTSIDE FROM LEFT, -1 SCORE
        else if(ballZ1 < 5 && ballRefX1 < -1.2) {
            ballZ1=13.0;
            gameInAction = true;
            scoreVal1--;
            generateRandomSpawn();

        }
        /// IF BALL IS OUTSIDE FROM RIGHT, -1 SCORE
        else if(ballZ1 < 5 && ballRefX1 > 0.9) {
            ballZ1=13.0;
            gameInAction = true;
            scoreVal1--;
            generateRandomSpawn();
        }
    /// IF SCORE == 5, USER WON, CHANGE WIN TO 1
        if (scoreVal1 >= 5) {
            win = 1;
            /// IF == -3, LOST
        } else if(scoreVal1 <= -3) {
            win = -1;
            /// ELSE, USER STILL PLAYING
        } else {
            win = 0;
        }
    }
    /// STATE 2 IS MULTIPLAYER
    else if (state == 2) {
        if(gameInAction == true) {
            ballZ1-=ballZIncrement1;
        }
        if(ballZ1 < 5 && ballRefX1 > -1 && ballRefX1 < 0.7) {
            ballZ1=13.0;
            gameInAction=true;
            generateRandomSpawn();
            scoreVal1++;
        } else if(ballZ1 < 5 && (ballRefX1 == 0.7 || ballRefX1 == 0.8 || ballRefX1 == 0.9)) {
            ballZ1=13.0;
            gameInAction = true;
            generateRandomSpawn();
        } else if(ballZ1 < 5 && (ballRefX1 == -1 || ballRefX1 == -1.1 || ballRefX1 == -1.2)) {
            ballZ1=13.0;
            gameInAction = true;
            generateRandomSpawn();
        } else if(ballZ1 < 5 && ballRefX1 < -1.2) {
            ballZ1=13.0;
            gameInAction = true;
            scoreVal1--;
            generateRandomSpawn();

        } else if(ballZ1 < 5 && ballRefX1 > 0.9) {
            ballZ1=13.0;
            gameInAction = true;
            scoreVal1--;
            generateRandomSpawn();
        }

        if (scoreVal1 >= 5) {
            win = 1;
        } else if(scoreVal1 <= -3) {
            win = -1;
        } else {
            win = 0;
        }


        if(gameInAction == true) {
            ballZ2-=ballZIncrement2;
        }
        if(ballZ2 < 5 && ballRefX2 > -1 && ballRefX2 < 0.7) {
            ballZ2=13.0;
            gameInAction=true;
            generateRandomSpawn();
            scoreVal2++;
        } else if(ballZ2 < 5 && (ballRefX2 == 0.7 || ballRefX2 == 0.8 || ballRefX2 == 0.9)) {
            ballZ2=13.0;
            gameInAction = true;
            generateRandomSpawn();
        } else if(ballZ2 < 5 && (ballRefX2 == -1 || ballRefX2 == -1.1 || ballRefX2 == -1.2)) {
            ballZ2=13.0;
            gameInAction = true;
            generateRandomSpawn();
        } else if(ballZ2 < 5 && ballRefX2 < -1.2) {
            ballZ2=13.0;
            gameInAction = true;
            scoreVal2--;
            generateRandomSpawn();

        } else if(ballZ2 < 5 && ballRefX2 > 0.9) {
            ballZ2=13.0;
            gameInAction = true;
            scoreVal2--;
            generateRandomSpawn();
        }
        if (scoreVal2 >= 5) {
            win = 1;
        } else if(scoreVal2 <= -3) {
            win = -1;
        } else {
            win = 0;
        }

    } // end of state 2
    /// ANIMATION FOR SUN
    sunRotate+=5;
    if(sunRotate = 360) {
        sunRotate = 0;
    }
    sunToLeft-=0.0025;
    if(sunToLeft < -10) {
        sunToLeft = 2;
    }
    glutPostRedisplay();
}

void lighting (void) {

    const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
    const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

    const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
    const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
    const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
    const GLfloat high_shininess[] = { 100.0f };

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);


}


int main (int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(500, 250);
    glutInitWindowSize(windowHeight, windowWidth);
    glutCreateWindow("GoalSalad");
    init();
    glShadeModel( GL_SMOOTH );
    glEnable( GL_DEPTH_TEST );
    // start display in main menu
    glutDisplayFunc(displayMainMenu);
    // define the function to handle keystrokes
    glutKeyboardFunc(myKeyboardFunc);
    // define the function to handle animation
    glutIdleFunc(animate);
    // default OpenGL lighting
    lighting();
    glutMainLoop();
    return 0;
}
