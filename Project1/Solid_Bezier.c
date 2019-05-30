/*  Solid_Bezier.c
 *  Programul afiseaza o suprafata Bezier folosind 
 *  evaluatori bidimensionali
 */
#include "glos.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

void myinit(void);
void initlights(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);
void CALLBACK display(void);
// tabloul ctrlpoints defineste cele 16 puncte de control ale suprafetei

GLdouble xRot = 0, yRot = 0, zRot = 0;

GLfloat LeftWingBodyTop[4][4][3] = {
	{ { -5.0, 15.0, 0.0 },{ -3.103, 15.0, 2.025 },{ 0.231, 15.0, 2.025 },{ 5.0, 15.0, 0.0 } },
{ { -5.0, 5.0, 0.0 },{ -3.103, 5.0, 2.025 },{ 0.231, 5.0, 2.025 },{ 5.0, 5.0, 0.0 } },
{ { -5.0, -5.0, 0.0 },{ -3.103, -5.0, 2.025 },{ 0.231, -5.0, 2.025 },{ 5.0, -5.0, 0.0 } },
{ { -5.0, -15.0, 0.0 },{ -3.103, -15.0, 2.025 },{ 0.231, -15.0, 2.025 },{ 5.0, -15.0, 0.0 } },
};
GLfloat LeftWingBodyBottom[4][4][3] = {
	{ { -5.0, 15.0, 0.0 },{ -3.103, 15.0, -2.025 },{ 0.231, 15.0, -2.025 },{ 5.0, 15.0, 0.0 } },
{ { -5.0, 5.0, 0.0 },{ -3.103, 5.0, -2.025 },{ 0.231, 5.0, -2.025 },{ 5.0, 5.0, 0.0 } },
{ { -5.0, -5.0, 0.0 },{ -3.103, -5.0, -2.025 },{ 0.231, -5.0, -2.025 },{ 5.0, -5.0, 0.0 } },
{ { -5.0, -15.0, 0.0 },{ -3.103, -15.0, -2.025 },{ 0.231, -15.0, -2.025 },{ 5.0, -15.0, 0.0 } },
};

void CALLBACK addRotationX(void)
{
	xRot += 1.0;
}
void CALLBACK addRotationY(void)
{
	yRot += 1.0;
}
void CALLBACK addRotationZ(void)
{
	zRot += 1.0;
}

// setari pentru iluminarea suprafetei
void initlights(void)
{
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat position[] = { 0.0, 0.0, 2.0, 1.0 };
    GLfloat mat_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

void CALLBACK display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glRotatef(xRot, 1.0, 0.0, 0.0);
	glRotatef(yRot, 0.0, 1.0, 0.0);
	glRotatef(zRot, 0.0, 0.0, 1.0);
    glPushMatrix();
    glEvalMesh2(GL_FILL, 0, 20, 0, 20); // specifica modul
	// de redare al poligoanelor (GL_FILL, GL_POINT, GL_LINE, 
	// si intervalele de esantionare a suprafetei pentru u si v
    glPopMatrix();
    glFlush();
}

void myinit(void)
{
    glClearColor (0.0, 0.0, 0.0, 1.0);
    glEnable (GL_DEPTH_TEST); // se activeaza ascunderea suprafetelor
	/*	 functia glMap2f defineste caracteristicile suprafetei Bezier
	- tipul punctelor determinate de functia glEvalCoord2f
	- intervalul de variatie al parametrului u (0 -1 in acest caz)
	- intervalul valorilor in tabloul ctrlpoints intre doua puncte
	de control pe directia u
	- numarul punctelor de control pe directia u
	- intervalul de variatie al parametrului v (0 -1 in acest caz)
	- intervalul valorilor in tabloul ctrlpoints intre doua puncte
	de control pe directia v
	- numarul punctelor de control pe directia v
	-tabloul punctelor de control
*/
	//glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &LeftWingBodyTop[0][0][0]);
	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4, &LeftWingBodyBottom[0][0][0]);
    glEnable(GL_MAP2_VERTEX_3); // validarea tipului de evaluare GL_MAP2_VERTEX_3
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE); // pentru iluminare
    glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);// intervalele de esantionare 
	// a suprafetei pentru parametrii u si v
    initlights();   /* doar daca se doreste reprezentarea cu iluminare */
}

void CALLBACK myReshape(GLsizei w, GLsizei h)
{
    if (!h) return;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
    glOrtho(-50.0, 50.0, -50.0*(GLfloat)h/(GLfloat)w,
		50.0*(GLfloat)h/(GLfloat)w, -50.0, 50.0);
    else
    glOrtho(-50.0*(GLfloat)w/(GLfloat)h,
		50.0*(GLfloat)w/(GLfloat)h, -50.0, 50.0, -50.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    auxInitDisplayMode (AUX_SINGLE | AUX_RGB | AUX_DEPTH16);
    auxInitPosition (0, 0, 500, 500);
    auxInitWindow ("Proiect grafica");
    myinit();

	auxKeyFunc(AUX_X, addRotationX);
	auxKeyFunc(AUX_x, addRotationX);
	auxKeyFunc(AUX_Y, addRotationY);
	auxKeyFunc(AUX_y, addRotationY);
	auxKeyFunc(AUX_Z, addRotationZ);
	auxKeyFunc(AUX_z, addRotationZ);

    auxReshapeFunc (myReshape);
    auxMainLoop(display);
    return(0);
}
