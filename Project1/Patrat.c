/*
Programul afiseaza un patrat pe care il translateaza 
pe axa x la apasarea sagetilor stanga, dreapta
*/
#include "glos.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

void myinit(void);
void CALLBACK display(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);
void CALLBACK MutaStanga(void);
void CALLBACK MutaDreapta(void);


static GLfloat x=0;

void myinit (void) {
    glClearColor(0,0,0, 1.0);
}


void CALLBACK MutaStanga(void)
{
	x=x-10;
}

void CALLBACK MutaDreapta(void)
{
	x=x+10;
}
void CALLBACK display (void)
{
    glClear(GL_COLOR_BUFFER_BIT);
    
    glLoadIdentity ();

	glTranslatef(x, 0, 0.0);



	GLUquadricObj * gluObjectToDraw = gluNewQuadric();
	gluQuadricNormals(gluObjectToDraw, GLU_SMOOTH);
	gluQuadricOrientation(gluObjectToDraw, GLU_OUTSIDE);
	gluQuadricDrawStyle(gluObjectToDraw, GLU_FILL);
	gluCylinder(gluObjectToDraw, 50, 50, 50, 50, 50);
    glFlush();
}

/*void CALLBACK myReshape(GLsizei w, GLsizei h)
{
    if (!h) return;			//transformare anizotropica, forma se modifica functie de forma(dimens) viewportului
    glViewport(0, 0, w, h);	//daca w>h stabilim ca baza inaltime, si stab unit logica de dimens in fct de h(h/320, 320 lungime lat patrat)
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho (-160.0, 160.0, -160.0, 
        160.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}*/

void CALLBACK myReshape(GLsizei w, GLsizei h)	
{
    if (!h) return;
    glViewport(0, 0, w, h);	
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)		
    glOrtho (-160.0, 160.0, 160.0*(GLfloat)h/(GLfloat)w, 
        -160.0*(GLfloat)h/(GLfloat)w, -10.0, 10.0);
    else 
    glOrtho (-160.0*(GLfloat)w/(GLfloat)h, 
        160.0*(GLfloat)w/(GLfloat)h, -160.0, 160.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    auxInitDisplayMode (AUX_SINGLE | AUX_RGB);
    auxInitPosition (0, 0, 300, 200);
    auxInitWindow ("Un patrat care se translateaza pe axa x");
    myinit ();
	auxKeyFunc (AUX_LEFT, MutaStanga);
	auxKeyFunc (AUX_RIGHT, MutaDreapta);
	
    auxReshapeFunc (myReshape);
    auxMainLoop(display);
    return(0);
}
