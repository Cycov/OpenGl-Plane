#include "GLOS.h"
#include<GL/GL.H>
#include<GL/GLU.H>
#include<GL/GLAux.h>
#include<math.h>
GLUquadricObj* qobj;
void myinit(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);
void CALLBACK display(void);
void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4], GLfloat mat[4][4]);
void calcCoeficientiPlan(float P[3][3], float coef[4]);


static GLfloat pozSursa[] = { -60, 50, -50, 1 };

void CALLBACK mutaSursaDreapta(AUX_EVENTREC *event) {
	if (pozSursa[0]<100)
		pozSursa[0] += 10;
}
void CALLBACK mutaSursaStanga(AUX_EVENTREC *event) {
	if (pozSursa[0]>-100)
		pozSursa[0] -= 10;
}

static double rotireCub = 0;

void CALLBACK rotireDreapta(void) {
	rotireCub = rotireCub - (3.14 / 4);
}
void CALLBACK rotireStanga(void) {
	rotireCub = rotireCub + (3.14 / 4);
}

static GLfloat x = 0;

void CALLBACK mutaStanga(void)
{
	x = x - 10;
}
void CALLBACK mutaDreapta(void)
{
	x = x + 10;
}

boolean umbra = 0;

void calcCoeficientiPlan(float P[3][3], float coef[4]) {
	float v1[3], v2[3];
	float length;
	static const int x = 0, y = 1, z = 2;

	// calculeaza 2 vectori din 3 pct
	v1[x] = P[0][x] - P[1][x];
	v1[y] = P[0][y] - P[1][y];
	v1[z] = P[0][z] - P[1][z];
	v2[x] = P[1][x] - P[2][x];
	v2[y] = P[1][y] - P[2][y];
	v2[z] = P[1][z] - P[2][z];

	//se calc produsul vectorial al celor 2 vectori => al3lea vector cu componentele A,B,C chiar coef din ec. planului
	coef[x] = v1[y] * v2[z] - v1[z] * v2[y];
	coef[y] = v1[z] * v2[x] - v1[x] * v2[z];
	coef[z] = v1[x] * v2[y] - v1[y] - v2[x];

	//normalizare vector
	length = (float)sqrt(coef[x] * coef[x] + coef[y] * coef[y] + coef[z] * coef[z]);
	coef[x] /= length;
	coef[y] /= length;
	coef[z] /= length;
}
void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4], GLfloat mat[4][4]) {
		// creeaza matricea care da umbra cunoscandu-se coef planului + poz sursei
		// IN mat SE SALVEAZA MATRICEA

		GLfloat coefPlan[4], temp;

		//determina coef planului
		calcCoeficientiPlan(puncte, coefPlan);

		// determinam D
		coefPlan[3] = -(coefPlan[0] * puncte[2][0] + coefPlan[1] * puncte[2][1] + coefPlan[2] * puncte[2][2]);

		// temp= AxL + ByL + CzL + Dw
		temp = coefPlan[0] * pozSursa[0] + coefPlan[1] * pozSursa[1] + coefPlan[2] * pozSursa[2] + coefPlan[3] * pozSursa[3];

		//prima coloana
		mat[0][0] = temp - coefPlan[0] * pozSursa[0];
		mat[1][0] = 0.0f - coefPlan[1] * pozSursa[0];
		mat[2][0] = 0.0f - coefPlan[2] * pozSursa[0];
		mat[3][0] = 0.0f - coefPlan[3] * pozSursa[0];
		//a 2a coloana
		mat[0][1] = 0.0f - coefPlan[0] * pozSursa[1];
		mat[1][1] = temp - coefPlan[1] * pozSursa[1];
		mat[2][1] = 0.0f - coefPlan[2] * pozSursa[1];
		mat[3][1] = 0.0f - coefPlan[3] * pozSursa[1];
		//a 3a coloana
		mat[0][2] = 0.0f - coefPlan[0] * pozSursa[2];
		mat[1][2] = 0.0f - coefPlan[1] * pozSursa[2];
		mat[2][2] = temp - coefPlan[2] * pozSursa[2];
		mat[3][2] = 0.0f - coefPlan[3] * pozSursa[2];
		//a4a coloana
		mat[0][3] = 0.0f - coefPlan[0] * pozSursa[3];
		mat[1][3] = 0.0f - coefPlan[1] * pozSursa[3];
		mat[2][3] = 0.0f - coefPlan[2] * pozSursa[3];
		mat[3][3] = temp - coefPlan[3] * pozSursa[3];
	}

void myinit(void) {
	GLfloat ambient[] = { 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat mat_shininess[] = { 128.0f };

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	//permite urmarirea culorilor
	glEnable(GL_COLOR_MATERIAL);
	//seteaza proprietatile de material pt a urma valorile glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	//seteaza sursa
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, pozSursa);

	//seteaza materialul
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// pt fundal
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST); // ascunderea suprafetelor
}


void desenareCub(void) {
	if (umbra == 1)
		glColor3f(0.05, 0.05, 0.05);
	else glColor3f(0, 0.3, 1);

	glPushMatrix();
	glTranslatef(x, -25, 0);
	glRotatef(rotireCub * 180 / 3.14, 1, 1, 0);
	qobj = gluNewQuadric();
	gluSphere(qobj, 10, 10, 10);
	glPopMatrix();
}

void desenareIarba(void) {
	glPushMatrix();
	glColor3f(0, 0.3, 0);
	glTranslatef(0, -10, 0);
	glRotatef(-90, 1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(-80, -80, -50);
	glVertex3f(-80, 80, -50);
	glVertex3f(80, 80, -50);
	glVertex3f(80, -80, -50);
	glEnd();
	glPopMatrix();
}

void CALLBACK display(void) {
	GLfloat matUmbra[4][4];
	//oricare 3 pct din plan in sens CCW
	GLfloat puncte[3][3] = {
		{ -50.0f, -50.0f, -50.0f },
		{ -50.0f, -50.0f, 50.0f },
		{ 50.0f, -50.0f, 50.0f } 
	};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//determina matricea pt calcularea umbrei
	MatriceUmbra(puncte, pozSursa, matUmbra);

	glPushMatrix();
		glLightfv(GL_LIGHT0, GL_POSITION, pozSursa);
		desenareIarba();
		umbra = 0;
		desenareCub();
	glPopMatrix();

	//deseneaza umbra
	//mai intai se dezactiveaza iluminarea si se salveaza starea matricei de proiectie
	glPushMatrix();
		glPushAttrib(GL_LIGHTING_BIT);
		glDisable(GL_LIGHTING);
		glPushMatrix();
			glMultMatrixf((GLfloat*)matUmbra);// se inmulteste matricea curenta cu matricea de umbrire
			umbra = 1;
			desenareCub();
		glPopMatrix();
		//deseneaza sursa de lumina ca o sfera mica galbena in pozitia reala
		glPushMatrix();
		glTranslatef(pozSursa[0], pozSursa[1], pozSursa[2]);
		glColor3f(1.0, 0.9, 0);
		glPointSize(5.0);
		auxWireSphere(5);
		glPopMatrix();
		//reseteaza starea variabilelor de iluminare
		glPopAttrib();
	glPopMatrix();

	auxSwapBuffers();
}

void CALLBACK myReshape(GLsizei w, GLsizei h) {
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (w <= h)
		glOrtho(-100, 100, -100 * (GLfloat)h / (GLfloat)w, 100 * (GLfloat)h / (GLfloat)w, -100, 100);
	else
		glOrtho(-100 * (GLfloat)w / (GLfloat)h, 100 * (GLfloat)w / (GLfloat)h, -100, 100, -100, 100);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(30, 1, 0, 0);
}

int main(int argc, char** argv) {
	auxInitDisplayMode(AUX_DOUBLE | AUX_DEPTH16 | AUX_RGB);
	auxInitPosition(10, 10, 600, 600);
	auxKeyFunc(AUX_RIGHT, mutaDreapta);
	auxKeyFunc(AUX_LEFT, mutaStanga);
	auxKeyFunc(AUX_UP, rotireDreapta);
	auxKeyFunc(AUX_DOWN, rotireStanga);
	auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, mutaSursaDreapta);
	auxMouseFunc(AUX_RIGHTBUTTON, AUX_MOUSEUP, mutaSursaStanga);
	auxInitWindow("Umbra");
	myinit();
	auxReshapeFunc(myReshape);
	auxMainLoop(display);
	return 0;
}