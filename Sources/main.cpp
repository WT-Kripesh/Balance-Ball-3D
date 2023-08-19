#include "main.h"

unsigned int ID;

const int window_width = 1000,window_height = 650;

GLfloat eyeX ,eyeY ,eyeZ ,lookX ,lookY,lookZ;

float wall_w = 40, wall_h = 4, wall_l = 120;
int doorl_open = 1, doorr_open = 1, dooru_open=1;
float doorlx = -wall_w/2, doorrx = wall_w/2, dooru_y=wall_w/4+10;

int up=0,down=0,r=0,l=0;
float ballx=0, bally=7, ballz=70;

bool ambient = true, diffuse = true, specular = true;
int light_1 = 1, light_2= 0, light_3 = 0, light_4 = 0;

float spot_x1=0, spot_y1=145, spot_z1=70;
float spot_x2=52, spot_y2=40, spot_z2=-126;


float obsx=wall_l-wall_w, obsy=10.0, obsz=-(wall_l+wall_w);

bool anglex_rot = true, ball_rotate=true;
float rotate_b=0;

int ball_pos[10] = {0};

float rot = 0;

bool ball_y_inc = true,ball_y_dec = false,jump_for = false,jump_back = false,jump_left = false,jump_right = false;

int cnt, process=0;
clock_t start,en_d,now;
string t;
GLint in=8,out=9;

static GLfloat v_cube[8][3] =
{
    {0,0,0},{0,0,1},{0,1,0},{0,1,1},{1,0,0},{1,0,1},{1,1,0},{1,1,1}
};

static GLubyte c_ind[6][4] =
{
    {3,1,5,7}, //front
    {2,0,1,3}, //left
    {7,5,4,6}, //r
    {2,3,7,6}, //top
    {1,0,4,5}, //bottom
    {6,4,0,2} //back
};

static void getNormal3p(GLfloat x1, GLfloat y1, GLfloat z1,
                        GLfloat x2, GLfloat y2, GLfloat z2,
                        GLfloat x3, GLfloat y3, GLfloat z3)
{
    GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

    Ux = x2-x1;
    Uy = y2-y1;
    Uz = z2-z1;

    Vx = x3-x1;
    Vy = y3-y1;
    Vz = z3-z1;

    Nx = Uy*Vz - Uz*Vy;
    Ny = Uz*Vx - Ux*Vz;
    Nz = Ux*Vy - Uy*Vx;

    glNormal3f(Nx,Ny,Nz);
}

static void resize(int width, int height)
{
    const float ar = (float) window_width / (float) window_height;

    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

void LoadTexture(const char*filename, int rep = 1)
{
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if(rep)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    BmpLoader bl(filename);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bl.iWidth, bl.iHeight, GL_RGB, GL_UNSIGNED_BYTE, bl.textureData );
}

void material_property(float colR=0.5, float colG=0.5, float colB=0.5, bool emission=false, float shine=10)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_ambient[] = { colR, colG, colB, 1.0 };
    GLfloat mat_diffuse[] = { colR, colG, colB, 1.0 };
    GLfloat mat_emission[] = {colR, colG, colB, 1.0};
    GLfloat mat_specular[] = { 1, 1, 1, 1 };
    GLfloat mat_shininess[] = {shine};

    glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv( GL_FRONT, GL_EMISSION, no_mat);

    if(emission)
        glMaterialfv( GL_FRONT, GL_EMISSION, mat_emission);
}

void cube(float colR=0.5, float colG=0.5, float colB=0.5, bool e = false, float shine = 1)
{
    material_property(colR, colG, colB,e, shine);

    glBegin(GL_QUADS);
    for (GLint i = 0; i <6; i++)
    {
        getNormal3p(v_cube[c_ind[i][0]][0], v_cube[c_ind[i][0]][1], v_cube[c_ind[i][0]][2],
                    v_cube[c_ind[i][1]][0], v_cube[c_ind[i][1]][1], v_cube[c_ind[i][1]][2],
                    v_cube[c_ind[i][2]][0], v_cube[c_ind[i][2]][1], v_cube[c_ind[i][2]][2]);

        glTexCoord2f(5,5);
        glVertex3fv(&v_cube[c_ind[i][0]][0]);
        glTexCoord2f(0,5);
        glVertex3fv(&v_cube[c_ind[i][1]][0]);
        glTexCoord2f(0,0);
        glVertex3fv(&v_cube[c_ind[i][2]][0]);
        glTexCoord2f(5,0);
        glVertex3fv(&v_cube[c_ind[i][3]][0]);

    }
    glEnd();
}


void ball_initialize()
{
        ballx=0;bally=7;ballz=70;
        eyeX = 0;eyeY = 40;eyeZ = 100;
        lookX = 0;lookY = 40;lookZ = -1000;
}
void door_touch()
{
    if((((doorlx-10 <= ballx-7 && ballx-7 <= doorlx+10) ||(doorlx-10 <= ballx+7 && ballx+7 <= doorlx+10)) ||
            ((doorrx-10 <= ballx-7 && ballx-7 <= doorrx+10) || (doorrx-10 <= ballx+7 && ballx+7 <= doorrx+10))) && ballz > -6 && ballz < 6)
        ball_initialize();
    else if((dooru_y<33 && ballz>-65 && ballz<-50) || (bally<9 && ballz>-167 && ballz<-165))
        ball_initialize();
}

void position_detect(float width, float len, int bridge_no, float cen_x=0, float cen_z=0)
{
    bool left = (ballx)>=(-width/2+cen_x);
    bool right = (ballx)<=(width/2+cen_x);
    bool fr = (ballz)>=(-len/2+cen_z);
    bool ba = (ballz)<=(len/2+cen_z);
    if(left && right && fr && ba)
        ball_pos[bridge_no] = 1;

    else
        ball_pos[bridge_no] = 0;
}

void update(int value)
{
    if(doorl_open)
    {
        doorlx -= 0.2;
        if(doorlx<-30)
            doorl_open=0;
    }
    else
    {
        doorlx += 0.2;
        if(doorlx>-10)
            doorl_open=1;
    }
    if(doorr_open)
    {
        doorrx += 0.2;
        if(doorrx>30)
            doorr_open=0;
    }
    else
    {
        doorrx -= 0.2;
        if(doorrx<10)
            doorr_open=1;
    }

    if(dooru_open)
    {
        dooru_y += 0.2;
        if(dooru_y>60)
            dooru_open=0;
    }
    if (!dooru_open)
    {
        dooru_y -= 0.2;
        if(dooru_y<20)
            dooru_open=1;
    }

    glutPostRedisplay(); //Tell GLUT that the display has changed
    //Tell GLUT to call update again in 25 milliseconds
    glutTimerFunc(25, update, 0);
}


void ball(double r=1,double g=1,double b=1)
{
    GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
    GLfloat mat_diffuse[] = { 1, 1, 1, 1.0 };
    GLfloat mat_specular[] = { 0.5, 0.5, 0.5, 1.0 };
    GLfloat mat_shininess[] = {100};

    glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess);

    glEnable(GL_TEXTURE_GEN_S); //enable texture coordinate generation
    glEnable(GL_TEXTURE_GEN_T);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,7);

    glPushMatrix();
    glTranslatef(ballx,bally,ballz);
    glutSolidSphere (7, 50, 50);

    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glDisable(GL_TEXTURE_GEN_S);
    glDisable(GL_TEXTURE_GEN_T);
}

void ball_jump()
{
    if(jump_for)
    {
        if(ball_y_inc)
        {
            bally +=0.5;
            ballz -=0.15;
            eyeZ -=0.15;
            lookZ -=0.15;
            if(bally>=30)
            {
                ball_y_inc = false;
                ball_y_dec = true;
            }
        }
        else if(ball_y_dec)
        {
            bally -=0.5;
            ballz -=0.15;
            eyeZ -=0.15;
            lookZ -=0.15;
            if(bally==0)
            {
                ball_y_inc = true;
                ball_y_dec = false;
                jump_for = false;
            }
        }

    }
    if(jump_back)
    {
        if(ball_y_inc)
        {
            bally +=0.5;
            ballz +=0.15;
            eyeZ +=0.15;
            lookZ +=0.15;
            if(bally>=30)
            {
                ball_y_inc = false;
                ball_y_dec = true;
            }
        }
        else if(ball_y_dec)
        {
            bally -=0.5;
            ballz +=0.15;
            eyeZ +=0.15;
            lookZ +=0.15;
            if(bally==0)
            {
                ball_y_inc = true;
                ball_y_dec = false;
                jump_back = false;
            }
        }
    }
    if(jump_left)
    {
        if(ball_y_inc)
        {
            bally +=0.5;
            ballx -=0.135;
            eyeX -=0.135;
            lookX -=0.135;
            if(bally>=30)
            {
                ball_y_inc = false;
                ball_y_dec = true;
            }
        }
        else if(ball_y_dec)
        {
            bally -=0.5;
            ballx -=0.135;
            eyeX -=0.135;
            lookX -=0.135;
            if(bally==0)
            {
                ball_y_inc = true;
                ball_y_dec = false;
                jump_left = false;
            }
        }
    }
    if(jump_right)
    {
        if(ball_y_inc)
        {
            bally +=0.5;
            ballx +=0.135;
            eyeX +=0.135;
            lookX +=0.135;
            if(bally>=30)
            {
                ball_y_inc = false;
                ball_y_dec = true;
            }
        }
        else if(ball_y_dec)
        {
            bally -=0.5;
            ballx +=0.135;
            eyeX +=0.135;
            lookX +=0.135;
            if(bally==0)
            {
                ball_y_inc = true;
                ball_y_dec = false;
                jump_right = false;
            }
        }
    }
    glutPostRedisplay();
}

void obstacle()
{
    material_property(1,0.9,0);
    glTranslatef(obsx, obsy, obsz);
    glScalef(1,2,1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,2);//11-steel
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.5,1);
    glVertex3f(-14,8,0);
    glTexCoord2f(0,0);
    glVertex3f(-16,0,0);
    glTexCoord2f(1,0);
    glVertex3f(-10,0,0);

    glTexCoord2f(0.5,1);
    glVertex3f(-8,8,0);
    glTexCoord2f(0,0);
    glVertex3f(-12,0,0);
    glTexCoord2f(1,0);
    glVertex3f(-4,0,0);

    glTexCoord2f(0.5,1);
    glVertex3f(0,8,0);
    glTexCoord2f(0,0);
    glVertex3f(-4,0,0);
    glTexCoord2f(1,0);
    glVertex3f(4,0,0);

    glTexCoord2f(0.5,1);
    glVertex3f(8,8,0);
    glTexCoord2f(0,0);
    glVertex3f(4,0,0);
    glTexCoord2f(1,0);
    glVertex3f(12,0,0);

    glTexCoord2f(0.5,1);
    glVertex3f(13,8,0);
    glTexCoord2f(0,0);
    glVertex3f(10,0,0);
    glTexCoord2f(1,0);
    glVertex3f(16,0,0);

    glEnd();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void bridge()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 4); //4-home
    // square
    glPushMatrix();
    position_detect(wall_w, wall_w, 7, 0, -(wall_l*2+wall_w*2+wall_l/2+wall_w/2));
    glTranslatef(0, 0, -(wall_l*2+wall_w*2+wall_l/2+wall_w/2));
    glScalef(wall_w, wall_h, wall_w);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,1); //1-wood

    // straight
    glPushMatrix();
    position_detect(wall_w, wall_l, 6, 0, -(wall_l*2+wall_w*2));
    glTranslatef(0, 0, -(wall_l*2+wall_w*2));
    glScalef(wall_w, wall_h, wall_l);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();

    // left
    glPushMatrix();
    position_detect(wall_l, wall_w, 5, wall_l/2-wall_w/2, -(wall_l+wall_w+wall_l/2+wall_w/2));
    glTranslatef(wall_l/2-wall_w/2, 0, -(wall_l+wall_w+wall_l/2+wall_w/2));
    glScalef(wall_l, wall_h, wall_w);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 0);
    glPopMatrix();

    // straight
    glPushMatrix();
    position_detect(wall_w, wall_l, 4, wall_l-wall_w, -(wall_l+wall_w));
    glTranslatef(wall_l-wall_w, 0, -(wall_l+wall_w));
    glScalef(wall_w, wall_h, wall_l);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();

    // right
    glPushMatrix();
    position_detect(wall_l, wall_w, 3, wall_l/2-wall_w/2, -(wall_l/2+wall_w/2));
    glTranslatef(wall_l/2-wall_w/2, 0, -(wall_l/2+wall_w/2));
    glScalef(wall_l, wall_h, wall_w);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 0);
    glPopMatrix();

    // straight
    glPushMatrix();
    position_detect(wall_w, wall_l, 2);
    glScalef(wall_w, wall_h, wall_l);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1, 1, 1);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,4);
    // square
    glPushMatrix();
    position_detect(wall_w, wall_w, 1, 0, wall_l/2+wall_w/2);
    glTranslatef(0, 0, wall_l/2+wall_w/2);
    glScalef(wall_w, wall_h, wall_w);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1.0,1.0,1.0);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void door()
{
    //right
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,11);
    glPushMatrix();
    glPushMatrix();
    glTranslatef(doorrx, wall_w/4+10, 0);
    glScalef(wall_w/2, wall_w/2+20, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.7,0.7,0.7);
    glPopMatrix();

    //left
    glPushMatrix();
    glTranslatef(doorlx, wall_w/4+10, 0);
    glScalef(wall_w/2, wall_w/2+20, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.7,0.7,0.7);
    glPopMatrix();
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    //updown
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D,1);
    glPushMatrix();
    glTranslatef(0, dooru_y, -(wall_w/4+50));
    glScalef(wall_w, wall_w/2+20, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(0.7,0.7,0.7);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}

void scene()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 5);
    //front
    glPushMatrix();
    glTranslatef(0, 90, -450);
    glScalef(400, 200, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1,1,1);
    glPopMatrix();

    //back
    glPushMatrix();
    glTranslatef(0, 90, 150);
    glScalef(400, 200, 1);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1,1,1);
    glPopMatrix();

    //right
    glPushMatrix();
    glTranslatef(200, 90, -350);
    glScalef(1, 200, 1000);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1,1,1);
    glPopMatrix();

    //left
    glPushMatrix();
    glTranslatef(-200, 90, -350);
    glScalef(1, 200, 1000);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1,1,1);
    glPopMatrix();

    //up
    glPushMatrix();
    glTranslatef(0, 190, -350);
    glScalef(400, 1, 1000);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1,1,1);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, in);

    glPushMatrix();
    glTranslatef(40, -10, -150);
    glScalef(160, 1, 600);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1.000, 0.271, 0.000);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // left
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, out);

    glPushMatrix();
    glTranslatef(-120, -10, -150);
    glScalef(160, 1, 600);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1.000, 0.627, 0.478);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);

    // right
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, out);

    glPushMatrix();
    glTranslatef(160, -10, -150);
    glScalef(80, 1, 600);
    glTranslatef(-0.5, -0.5, -0.5);
    cube(1.000, 0.627, 0.478);
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
}

void idle()
{
    if(process==2)
    {
        en_d=clock();
        cnt=(en_d-start)/CLOCKS_PER_SEC;

        if(cnt==120)
        {
            process=3;
        }
        else if((cnt<100) && (ballz<=-380))
        {
            process=1;
        }
    }

    if (ball_rotate == true)
    {
        rotate_b+=0.0001;
    }
}

void game_over()
{
    int status = 0;

    for(int i=0; i<10; i++)
    {
        status +=ball_pos[i];
    }
    if(status<=0)
    {
        process = 3;
    }
}

void Text(char *str,int x,int y,int z)
{
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(0.04,0.04,0.04);
    material_property(1,1,1, true);
    for (int i = 0; i < strlen(str); i++)
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,str[i]);
    glPopMatrix();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int lim = 8;
    glFrustum(-lim, lim, -lim, lim, 2, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(eyeX,eyeY,eyeZ, lookX,lookY,lookZ, 0,1,0);

    glPushMatrix();
    obstacle();
    glPopMatrix();

    glPushMatrix();
    scene();
    glPopMatrix();

    glPushMatrix();
    door();
    glPopMatrix();

    glPushMatrix();
    bridge();
    glPopMatrix();

    glPushMatrix();
    ball();
    glPopMatrix();

    glPushMatrix();
    ball_jump();
    glPopMatrix();

    glPushMatrix();
    door_touch();
    glPopMatrix();

    glPushMatrix();
    game_over();
    glPopMatrix();

    glPushMatrix();
    t = "Time: ";
    stringstream ss;
    ss<<cnt;
    string scnt;
    ss>>scnt;
    t+=scnt;
    glPushMatrix();
    glTranslatef(ballx+65, 7+122, ballz+0.5);
    glScalef(0.04,0.04,0.04);
    material_property(1,1,1, true);

    for (int i = 0; i < t.size(); i++)
    {
        glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN,t[i]);
    }
    glPopMatrix();

    glPopMatrix();

    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void output(char *str,int x,int y,int z)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int lim = 8;
    glFrustum(-lim, lim, -lim, lim, 2, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0,40,100, 0,40,-1000, 0,1,0);

    glPushMatrix();
    glRasterPos3f(x,y,z);
    for (int i = 0; i < strlen(str); i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,str[i]);
    }
    glPopMatrix();

    glPushMatrix();
    material_property(1,1,1,true);
    GLUquadricObj *start_ball = gluNewQuadric();
    glEnable(GL_TEXTURE_2D);
    gluQuadricTexture(start_ball, true);
    glBindTexture(GL_TEXTURE_2D,11);
    glRotatef(rotate_b,0.02,1,0);
    glTranslatef(0,120,0);
    glScalef(5,5,5);
    gluSphere(start_ball,5,50,50);
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void Start(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    material_property(1,1,0, true);
    output("3D Ball Balance", -60,250,0);
    glPopMatrix();
    glPushMatrix();
    material_property(1,1,0, true);
    output("Kripesh Nihure", -60,50,0);
    glPopMatrix();
    glPushMatrix();
    material_property(1,1,0, true);
    output("077BCT037", -50,10,0);
    glPopMatrix();
    glPushMatrix();
    material_property(1,1,0, true);
    output("Press ENTER to start the game", -130,-250,0);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void Play()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    material_property(1,1,0, true);
    output("Congratulations! You have completed the game.", -160, 50, 0);
    glPopMatrix();

    glPushMatrix();
    material_property(1,1,0, true);
    output("Press 'q' to exit & 'n' to start again.", -160, 0, 0);
    glPopMatrix();

    glFlush();
    glutSwapBuffers();
}

void End()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    material_property(1,1,0,true);
    output("Game Over!", -60, 50, 0);
    glPopMatrix();
    glPushMatrix();
    material_property(1,1,0,true);
    output("Press 'q' to exit & 'n' to start again.", -160, 0, 0);
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

void func()
{
    if(process==0) Start();
    else if(process==1) Play();
    else if (process==2) display();
    else if (process==3) End();
}

void key1(int x,int y,int z)
{
    switch(x){
    case GLUT_KEY_LEFT:
        ballx -=1;
        eyeX--;
        lookX--;
        break;
    case GLUT_KEY_RIGHT:
        ballx +=1;
        eyeX++;
        lookX++;
        break;
    case GLUT_KEY_UP:
        if(ballz >= -420)
        {
            ballz -=1;
            eyeZ--;
            lookZ--;
        }
        break;
    case GLUT_KEY_DOWN:
        ballz +=1;
        eyeZ++;
        lookZ++;
        break;
    }
    glutPostRedisplay();
}
static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 13:    //13=ASCII of Enter key
        ball_initialize();
        process=2;
        start=clock();
        break;
    case 'n':
        ball_initialize();
        process=2;
        start=clock();
        break;
    case 'q':
        exit(0);
        break;
    case 'u':   //up
        eyeY++;
        lookY++;
        break;
    case 'h':   //down
        eyeY--;
        lookY--;
        break;
    case 'p':   //move right
        eyeX++;
        lookX++;
        break;
    case 'o':   //move left
        eyeX--;
        lookX--;
        break;
    case '9':   //zoom in
        eyeZ--;
        break;
    case '0':   //zoom out
        eyeZ++;
        break;
    case 'j':
        jump_left = true;
        break;
    case 'l':
        jump_right = true;
        break;
    case 'i':
        jump_for = true;
        break;
    case 'k':
        jump_back = true;
        break;
    case 'm':
        if(in==8){
            in=2;out=10;
        }
        else if(in==2){
            in=8;out=9;
        }
        break;
    }

    glutPostRedisplay();
}

void instructions()
{
    cout << "##	INSTRUCTIONS	##\n\nPress\n\n - 'q' to quit\n - 'u' to up\n - 'h' to down\n  - '9' to zoom in\n - '0' to zoom out\n";
    cout << "\n - up key for ball front\n - down key for ball back\n - left key for ball left\n - right key for ball right\n";
    cout << "\n - 'i' for jump front\n - 'k' for jump back\n - 'j' for jump left\n - 'l' for jump right\n";
}

int main(int argc, char *argv[])
{
    instructions();
    glutInit(&argc, argv);
    glutInitWindowSize(window_width,window_height);
    glutInitWindowPosition(250,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Balance It!");
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\wood.bmp",1);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\lava_n.bmp",2);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\finish.bmp",3);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\start.bmp",4);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\sk.bmp",0);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\volcano.bmp",6);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\ball.bmp",7);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\water.bmp",8);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\grass.bmp",9);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\sand.bmp",10);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\steel.bmp",11);
    LoadTexture("C:\\Users\\User\\OneDrive\\Desktop\\Newfolder\\Projects\\Balance\\Images\\steel.bmp",12);

    glutDisplayFunc(func);
    glutKeyboardFunc(key);
    glutSpecialFunc(key1);
    glutIdleFunc(idle);

    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_NORMALIZE);

    glutTimerFunc(25, update, 0);
    glutMainLoop();
    return EXIT_SUCCESS;
}
