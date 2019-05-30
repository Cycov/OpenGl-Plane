

/*  curbe_Bezier.c
 *  
 Programul utilizeaza evaluatorii pentru determinarea
 punctelor de pe curba Bezier
 */
#include "glos.h"

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

void myinit(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);
void CALLBACK display(void);

GLfloat ctrlpoints[4][3] = {			
	//sunt date coordonatele celor 4 puncte de control
    { -4.0, -4.0, 0.0}, { -2.0, 4.0, 0.0}, 
    {2.0, -4.0, 0.0}, {4.0, 4.0, 0.0}};

void myinit(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0); //culoarea background-ului
    glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
/*functia defineste caracteristicile curbei:
-tipul punctelor de control date in vectorul ctrlpoints, si al
datelor de iesire generate de functia de evaluare glEvalCoord1f
-valorile extreme luate de parametrul u (0 si 1 in acst caz)
-numarul coordonatelor date pentru fiecare punct de control, in tabloul
ctrlpoints
-numarul punctelor de control pe baza carora se va determina ordinul curbei
(numar puncte-1)
-vectorul punctelor de control
   */
	
    glEnable(GL_MAP1_VERTEX_3);	//se valideaza un anumit tip de evaluare
    glShadeModel(GL_FLAT); 	//umbrire constanta pe poligoane
	glLineStipple (1, 0x0F0F);
}

void CALLBACK display(void)
{
    int i;

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0, 0.0, 0.0);	//culoarea curenta de desenare
    glBegin(GL_LINE_STRIP);		//se deseneaza curba prin segmente de dreapta
    for (i = 0; i <= 30; i++) 
        glEvalCoord1f((GLfloat) i/30.0); //pentru cele 30 varfuri determinate de 
								// functia glEvalCoord1f
    glEnd();
    
	/* Se afiseaza punctele de control. */
    glPointSize(5.0);	// de dimensiune 5
    glColor3f(1.0, 0.0, 0.0);	// culoare rosie
    glBegin(GL_POINTS);
    for (i = 0; i < 4; i++) 
        glVertex3fv(&ctrlpoints[i][0]);
    glEnd();

		
	
	//poligonul caracteristic ?
	glEnable(GL_LINE_STIPPLE);
	glColor3f(1.0, 0.0, 1.0);
	glBegin (GL_LINE_STRIP);
	for (i=0; i<4; i++)
		glVertex3fv(&ctrlpoints[i][0]);
	glEnd(); 

    glFlush();
}

void CALLBACK myReshape(GLsizei w, GLsizei h)
{
    if (!h) return;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
    glOrtho(-5.0, 5.0, -5.0*(GLfloat)h/(GLfloat)w, 
        5.0*(GLfloat)h/(GLfloat)w, -5.0, 5.0);
    else
    glOrtho(-5.0*(GLfloat)w/(GLfloat)h, 
        5.0*(GLfloat)w/(GLfloat)h, -5.0, 5.0, -5.0, 5.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    auxInitDisplayMode (AUX_SINGLE | AUX_RGB);
    auxInitPosition (0, 0, 500, 500);
    auxInitWindow ("Curbe Bezier");
    myinit();
    auxReshapeFunc (myReshape);
    auxMainLoop(display);
    return(0);
}
