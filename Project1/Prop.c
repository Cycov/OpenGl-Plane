#include "glos.h"
#include<math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>
#include <time.h>


#pragma region Defines
#define WINDOW_SIZE 4000
#define LIGHT_MOVEMENT_FACTOR 200
#define PLANE_SPEED 1
#define PLANE_ELEVATION_FACTOR 200
#define PROP_QUANTITY_TREE 100
#define PROP_QUANTITY_FLOWER 30
#define PROP_QUANTITY_PUDDLE 10
#pragma endregion

#pragma region Prototypes

#pragma region Other
double fRand(double, double);

typedef struct 
{
	GLdouble X, Y, Z;
} GLpoint;

typedef struct
{
	GLdouble R, G, B;
} GLcolor;

typedef struct 
{
	GLpoint location;
	GLcolor color;
	GLdouble seed;
	GLdouble scale;
} propData;

propData trees[PROP_QUANTITY_TREE];
propData flowers[PROP_QUANTITY_FLOWER];
propData puddles[PROP_QUANTITY_PUDDLE];
GLdouble puddleInc = 0;

GLdouble eqn0[4] = { WINDOW_SIZE, 0.0, 0.0, 1.0 };
GLdouble eqn1[4] = { -WINDOW_SIZE, 0.0, 0.0, 1.0 };
GLdouble eqn2[4] = { 0.0, 0, WINDOW_SIZE * 2, 1.0 };
GLdouble eqn3[4] = { 0.0, 0, -WINDOW_SIZE * 2, 1.0 };

#pragma endregion

#pragma region Default

void myinit(void);
void CALLBACK display(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);
GLfloat ctrlpoints[4][4][3] =
{
	{
		{ 3.827, 47.806, -2.103 },
		{ 1.293, 47.806, 0.795 },
		{ -1.942, 49.161, 3.286 },
		{ -5.616, 49.161, 2.708 }
	},
	{
		{ 10.435, 19.7, -4.497 },
		{ 3.685, 19.7, 0.857 },
		{ -4.298, 16.667, 4.957 },
		{ -14.825, 16.667, 5.291 }
	},
	{
		{ 14.2, -16.667, -3.137 },
		{ 5.131, -16.667, 2.219 },
		{ -2.618, -15.852, 3.962 },
		{ -10.387, -15.852, 3.141 }
	},
	{
		{ 9.12, -47.952, 1.605 },
		{ 3.609, -47.952, 3.075 },
		{ -0.823, -42.623, 2.655 },
		{ -4.128, -42.623, 1.023 }
	},
};


#pragma endregion

#pragma region Animation and control
void CALLBACK increaseRotorRotation(void);
void CALLBACK decreaseRotorRotation(void);
void CALLBACK increaseBladeRotation(void);
void CALLBACK decreaseBladeRotation(void);
GLfloat currentRotorRotation = 0.0, bladeRotation = 30.0;

//Pitch and roll
void CALLBACK increaseRoll(void);
void CALLBACK decreaseRoll(void);
void CALLBACK increasePitch(void);
void CALLBACK decreasePitch(void);
void CALLBACK increaseDebugRotation(void);
void CALLBACK decreaseDebugRotation(void);
GLfloat lightXAxis = 0, lightZAxis = 0, lightYAxis = 1200;

//Lower-raise plane
void CALLBACK raisePlane(void);
void CALLBACK lowerPlane(void);
GLfloat planeElevation = -600;

//Animation
void CALLBACK IdleFunction(void);
GLfloat lapRotation = 0;

//   animation control
void CALLBACK toggleFreezeLap(void);
GLboolean frozen = FALSE;

#pragma endregion

#pragma region Shadowing
void MatriceUmbra(GLfloat puncte[3][3], GLfloat pozSursa[4], GLfloat mat[4][4]);
void calcCoeficientiPlan(float P[3][3], float coef[4]);
void plan(void);
int umbra = 0;
#pragma endregion

#pragma region Texturing
void incarca_textura(const char* s);
GLuint IDMetal, IDStripe;
GLuint IDtextura;
#pragma endregion
#pragma endregion

#pragma region Function bodies

#pragma region Other
double fRand(double fMin, double fMax)
{
	double f = (double)rand() / RAND_MAX;
	return fMin + f * (fMax - fMin);
}
#pragma endregion


#pragma region Texturing and shadowing
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
void incarca_textura(const char* s)
{
	AUX_RGBImageRec *pImagineTextura = auxDIBImageLoad(s);

	if (pImagineTextura != NULL)
	{
		glGenTextures(1, &IDtextura);

		glBindTexture(GL_TEXTURE_2D, IDtextura);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, 3, pImagineTextura->sizeX, pImagineTextura->sizeY,
			0, GL_RGB, GL_UNSIGNED_BYTE, pImagineTextura->data);
	}

	if (pImagineTextura)
	{
		if (pImagineTextura->data)
			free(pImagineTextura->data);

		free(pImagineTextura);
	}

}

#pragma endregion

#pragma region Animation and control
void CALLBACK increaseRotorRotation()
{
	currentRotorRotation = ((int)currentRotorRotation + 15) % 360;
}
void CALLBACK decreaseRotorRotation()
{
	currentRotorRotation = ((int)currentRotorRotation - 15) % 360;
}
void CALLBACK increaseBladeRotation()
{
	bladeRotation = ((int)bladeRotation + 15) % 360;
}
void CALLBACK decreaseBladeRotation()
{
	bladeRotation = ((int)bladeRotation - 15) % 360;
}

void CALLBACK increaseDebugRotation()
{
	lightYAxis += LIGHT_MOVEMENT_FACTOR;
}
void CALLBACK decreaseDebugRotation()
{
	lightYAxis -= LIGHT_MOVEMENT_FACTOR;
}
void CALLBACK increasePitch()
{
	lightZAxis += LIGHT_MOVEMENT_FACTOR;
}
void CALLBACK decreasePitch()
{
	lightZAxis -= LIGHT_MOVEMENT_FACTOR;
}
void CALLBACK increaseRoll()
{
	lightXAxis += LIGHT_MOVEMENT_FACTOR;
}
void CALLBACK decreaseRoll()
{
	lightXAxis -= LIGHT_MOVEMENT_FACTOR;
}

void CALLBACK raisePlane(void)
{
	planeElevation += PLANE_ELEVATION_FACTOR;
}
void CALLBACK lowerPlane(void)
{
	planeElevation -= PLANE_ELEVATION_FACTOR;
}

void CALLBACK IdleFunction()
{
	currentRotorRotation += PLANE_SPEED * 10;
	if (!frozen)
		lapRotation += PLANE_SPEED;
	display();
	Sleep(5);
}
void CALLBACK toggleFreezeLap()
{
	frozen = !frozen;
}
#pragma endregion

#pragma region Modeling
void drawBody(GLUquadricObj* gluObjectToDraw)
{
	glPushMatrix();
	glRotatef(90, 0, 1.0, 0);


	//Back wings
	glPushMatrix();
	glTranslatef(0, 0, 720);
	gluDisk(gluObjectToDraw, 0, 50, 20, 1);
	glTranslatef(0, 50, -50);
	glRotatef(90, 0, 1, 0);
	glTranslatef(-50, 0, 0);
	gluPartialDisk(gluObjectToDraw, 0, 100, 20, 1, 0, 90);
	glTranslatef(50, 0, 0);
	glRotatef(-90, 0, 1, 0);
	glTranslatef(0, -50, 0);
	auxSolidBox(300, 10, 100);
	glPopMatrix();

	//Back cone
	glPushMatrix();
	glTranslatef(0, 0, 620);
	gluCylinder(gluObjectToDraw, 70, 50, 100, 20, 4);
	glPopMatrix();


	//Corp
	glTranslatef(0, 0, 25);
	glBindTexture(GL_TEXTURE_2D, IDMetal);
	gluCylinder(gluObjectToDraw, 70, 70, 600, 20, 4);

	//Cockpit
	glPushMatrix();
	if (umbra == 0)
	{
		glTranslatef(0, 45, 150);
		glColor3f(0, 0, 1);
		glScalef(1, 1, 1.5);
		auxSolidSphere(80);
		glColor3f(1, 1, 1);
	}
	else
	{
		glTranslatef(0, 45, 150);
		glScalef(1, 1, 1.5);
		auxSolidSphere(80);
	}
	glPopMatrix();

	glBindTexture(GL_TEXTURE_2D, IDStripe);
	//Wings
	glPushMatrix();
	glTranslatef(0, -40, 150);
	auxSolidBox(700, 10, 200);

	//Wings pylons
	glPushMatrix();
	glTranslatef(0, 170, 0);
	glRotatef(90, 1, 0, 0);
	glTranslatef(250, -50, 0);
	gluCylinder(gluObjectToDraw, 20, 20, 170, 20, 5);
	glTranslatef(0, 100, 0);
	gluCylinder(gluObjectToDraw, 20, 20, 170, 20, 5);
	glTranslatef(-500, 0, 0);
	gluCylinder(gluObjectToDraw, 20, 20, 170, 20, 5);
	glTranslatef(0, -100, 0);
	gluCylinder(gluObjectToDraw, 20, 20, 170, 20, 5);
	glPopMatrix();


	glTranslatef(0, 170, 0);
	auxSolidBox(700, 10, 200);
	glPopMatrix();

	//Con varf
	glTranslatef(0, 0, -50);
	gluCylinder(gluObjectToDraw, 40, 70, 50, 20, 4);

	//Con varf 2
	glTranslatef(0, 0, -15);
	gluCylinder(gluObjectToDraw, 10, 40, 15, 10, 4);


	glPopMatrix();
}
void plan(void) {
	glBegin(GL_QUADS);
	glColor3f(0, 1, 0);
	glNormal3f(0, 0, 1);
	glVertex3f(WINDOW_SIZE, planeElevation, WINDOW_SIZE*2);
	glNormal3f(0, 0, 1);
	glVertex3f(WINDOW_SIZE, planeElevation, -WINDOW_SIZE*2);
	glNormal3f(0, 0, 1);
	glVertex3f(-WINDOW_SIZE, planeElevation, -WINDOW_SIZE*2);
	glNormal3f(0, 0, 1);
	glVertex3f(-WINDOW_SIZE, planeElevation, WINDOW_SIZE*2);
	glEnd();
}
void drawTree(GLUquadricObj* gluObjectToDraw, propData tree)
{
	glPushMatrix();
		srand(tree.seed);
		glTranslatef(tree.location.X, tree.location.Y, tree.location.Z);
		glScalef(tree.scale, tree.scale, tree.scale);

		if (umbra == 0)
			glColor3f(0.7254901960784314, 0.4784313725490196, 0.3411764705882353); //maro
		else
			glColor3f(0, 0, 0);
		//Bottom cone
		glPushMatrix();
			glRotatef(-90, 1, 0, 0);
			gluCylinder(gluObjectToDraw, 100, 70, 200, 10, 10);
		glPopMatrix();
			//Top cone
		glPushMatrix();
			glTranslatef(0, 400, 0);
			glRotatef(90, 1, 0, 0);
			gluCylinder(gluObjectToDraw, 100, 70, 200, 10, 10);
		glPopMatrix();

		//Some deformed spheres
		glPushMatrix();
			if (umbra == 0)
				glColor3f(tree.color.R, tree.color.G, tree.color.B);
			else
				glColor3f(0, 0, 0);

			glScalef(1.2, 1.32, 0.9);
			glPushMatrix();
				glTranslatef(0, 500, 0);
				glScalef(fRand(0.7,2), 1, 1);
				gluSphere(gluObjectToDraw, 200, 40, 40);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0, 500, 0);
				glScalef(1, 1, fRand(0.7, 2));
				gluSphere(gluObjectToDraw, 200, 40, 40);
			glPopMatrix();
			glPushMatrix();
				glTranslatef(0, 500, 0);
				glScalef(1, fRand(1.1, 1.7), 1);
				gluSphere(gluObjectToDraw, 200, 40, 40);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
}
void drawFlower(GLUquadricObj* gluObjectToDraw, propData flower)
{
	glPushMatrix();
		srand(flower.seed);
		if (umbra == 0)
			glColor3f(flower.color.R, flower.color.G, flower.color.B);
		else
			glColor3f(0, 0, 0);
		glTranslatef(flower.location.X, flower.location.Y, flower.location.Z);
		glScalef(flower.scale,flower.scale,flower.scale);
		glTranslatef(0, 25, 0);
		glPushMatrix();
			glScalef(fRand(0.5,1), fRand(0.5, 2), fRand(0.5, 2));
			gluSphere(gluObjectToDraw, 50, 5, 5);
		glPopMatrix();
		glPushMatrix();
			glScalef(fRand(0.5,2), fRand(0.5, 2), fRand(0.5, 1));
			gluSphere(gluObjectToDraw, 50, 5, 5);
		glPopMatrix();
	glPopMatrix();
}

void drawPuddle(GLUquadricObj* gluObjectToDraw, propData puddle)
{
	glPushMatrix();
		srand(puddle.seed);
		int vertexCount = rand() % 20 + 3;
		int maxAngle = 360 / vertexCount, angle = 0;
		if (umbra == 0)
			glColor3f(puddle.color.R, puddle.color.G, puddle.color.B);
		else
			glColor3f(0, 0, 0);

		glColor3f(puddle.color.R, puddle.color.G, puddle.color.B);
		glTranslatef(puddle.location.X, puddle.location.Y, puddle.location.Z);
		glScalef(puddle.scale, puddle.scale, puddle.scale);
		glBegin(GL_POLYGON);
			while (angle < 360)
			{
				glNormal3f(0, 1, 0);
				glVertex3f(cos(angle) * 1000, 0, sin(angle) * 1000);
				angle += maxAngle;
			}
		glEnd();
	glPopMatrix();
}
void initProps()
{
	int i = 0;
	for (i = 0; i < PROP_QUANTITY_TREE; i++)
	{
		trees[i].location.X = fRand(-WINDOW_SIZE, WINDOW_SIZE);
		trees[i].location.Y = 0;
		trees[i].location.Z = fRand(-WINDOW_SIZE, WINDOW_SIZE) * 2;
		
		trees[i].color.R = 0;
		trees[i].color.G = fRand(0.1, 1);
		trees[i].color.B = 0;

		trees[i].scale = fRand(0.5, 1.1);

		trees[i].seed = rand();
	}
	for (i = 0; i < PROP_QUANTITY_FLOWER; i++)
	{
		flowers[i].location.X = fRand(-WINDOW_SIZE, WINDOW_SIZE);
		flowers[i].location.Y = 0;
		flowers[i].location.Z = fRand(-WINDOW_SIZE, WINDOW_SIZE) * 2;

		flowers[i].color.R = fRand(0.1, 2);
		flowers[i].color.G = fRand(0.1, 2);
		flowers[i].color.B = fRand(0.1, 2);

		flowers[i].scale = fRand(0.7,2);

		flowers[i].seed = rand();
	}
	for (i = 0; i < PROP_QUANTITY_PUDDLE; i++)
	{

		puddles[i].location.X = fRand(-WINDOW_SIZE, WINDOW_SIZE);
		puddles[i].location.Y = 0;
		puddles[i].location.Z = fRand(-WINDOW_SIZE, WINDOW_SIZE) * 2;

		puddles[i].color.R = 0;
		puddles[i].color.G = fRand(0.001, 0.1);
		puddles[i].color.B = fRand(0.5, 1);

		puddles[i].scale = 1;

		puddles[i].seed = rand();
	}
}
void drawProps()
{
	GLUquadricObj * gluTree = gluNewQuadric();
	int i = 0;
	for (i = 0; i < PROP_QUANTITY_TREE; i++)
	{
		trees[i].location.Y = planeElevation;
		drawTree(gluTree, trees[i], 1);
	}
	for (i = 0; i < PROP_QUANTITY_FLOWER; i++)
	{
		flowers[i].location.Y = planeElevation;
		drawFlower(gluTree, flowers[i]);
	}
	for (i = 0; i < PROP_QUANTITY_PUDDLE; i++)
	{
		puddles[i].location.Y = planeElevation;
		drawPuddle(gluTree, puddles[i]);
	}
}
void drawPropellers(GLfloat angle, GLfloat bladeRoll)
{
	const int offset = 90;
	glPushMatrix();

	glRotatef(angle, 1.0, 0, 0);

	glPushMatrix();
	glRotatef(bladeRoll, 0, 1.0, 0);
	glTranslatef(0, offset, 0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, -offset);
	glRotatef(90, 1.0, 0, 0);
	glRotatef(bladeRoll, 0, 1.0, 0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();

	glPushMatrix();
	glRotatef(180, 1.0, 0, 0);
	glRotatef(bladeRoll, 0, 1.0, 0);
	glTranslatef(0, offset, 0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, offset);
	glRotatef(270, 1.0, 0, 0);
	glRotatef(bladeRoll, 0, 1.0, 0);
	glEvalMesh2(GL_FILL, 0, 20, 0, 20);
	glPopMatrix();

	glPopMatrix();
}
void drawTiltedAndTranslatedPlane()
{
	glPushMatrix();
		glRotatef(lapRotation, 0, 1, 0);
		glTranslatef(0, 0, -2000);
		glRotatef(20, 1, 0, 0);
		//david.alexandru23@gmail.com
		GLUquadricObj * gluObjectToDraw = gluNewQuadric();
		gluQuadricNormals(gluObjectToDraw, GLU_SMOOTH);
		gluQuadricOrientation(gluObjectToDraw, GLU_OUTSIDE);
		gluQuadricDrawStyle(gluObjectToDraw, GLU_FILL);
		gluQuadricTexture(gluObjectToDraw, GL_TRUE);

		if (umbra == 0)
			glEnable(GL_TEXTURE_2D);
		else
		{
			glDisable(GL_TEXTURE_2D);
			glColor3f(0, 0, 0);
		}
		drawPropellers(currentRotorRotation, bladeRotation);
		drawBody(gluObjectToDraw);

		glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
void drawAll()
{
	plan();
	glColor3f(1, 1, 1);
	drawProps();
	drawTiltedAndTranslatedPlane();
}
#pragma endregion

#pragma region Default

void myinit(void)
{
	GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	//componenta ambientala din sursa 0 este lumina cu intensitate 0
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	//componenta difuza din sursa 0 este lumina cu intensitate 1 
	//pentru fiecare componenta de culoare R, G, B
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
	//componenta speculara din sursa 0 este lumina cu intensitate 1 
	//pentru fiecare componenta de culoare

	GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat local_view[] = { 0.0 };

	glEnable(GL_DEPTH_TEST);//activare test de adancime
	glDepthFunc(GL_LESS);//modelul de comparatie in testul de adancime

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

	glEnable(GL_LIGHTING);//activare iluminare
	glEnable(GL_LIGHT0);//activare sursa 0

	//Ca sa mearga glColor3f
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_TEXTURE_2D);

	glMap2f(GL_MAP2_VERTEX_3, 0, 1, 3, 4,
		0, 1, 12, 4, &ctrlpoints[0][0][0]);
	glEnable(GL_MAP2_VERTEX_3); // validarea tipului de evaluare GL_MAP2_VERTEX_3
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE); // pentru iluminare
	glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);// intervalele de esantionare 

	glClearColor(0.0, 0.0, 0.0, 0.0);//culoare cyan 



	const char * sir;

	sir = "metal.bmp";
	incarca_textura(sir);
	IDMetal = IDtextura;
	sir = "stripe.bmp";
	incarca_textura(sir);
	IDStripe = IDtextura;


	//glClipPlane(GL_CLIP_PLANE0, eqn0);
	//glEnable(GL_CLIP_PLANE0);

	//glClipPlane(GL_CLIP_PLANE1, eqn1);
	//glEnable(GL_CLIP_PLANE1);

	//glClipPlane(GL_CLIP_PLANE2, eqn2);
	//glEnable(GL_CLIP_PLANE2);

	//glClipPlane(GL_CLIP_PLANE3, eqn3);
	//glEnable(GL_CLIP_PLANE3);

}

void CALLBACK myReshape(GLsizei w, GLsizei h)
{
	if (!h) return;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= (h * 2))
		glOrtho(-WINDOW_SIZE, WINDOW_SIZE, -WINDOW_SIZE * ((GLfloat)h) / (GLfloat)w,
			WINDOW_SIZE *((GLfloat)h) / (GLfloat)w, -WINDOW_SIZE, WINDOW_SIZE*3);
	else
		glOrtho(-WINDOW_SIZE * (GLfloat)w / ((GLfloat)h),
			WINDOW_SIZE *(GLfloat)w / ((GLfloat)h), -WINDOW_SIZE, WINDOW_SIZE, -WINDOW_SIZE, WINDOW_SIZE*3);
	glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
	srand(time(NULL));
	auxInitDisplayMode(AUX_SINGLE | AUX_RGB | AUX_DEPTH16);
	auxInitPosition(0, 0, 600, 450);
	auxInitWindow("Proiect");
	myinit();
	initProps();
	auxKeyFunc(AUX_Z, increaseRotorRotation);
	auxKeyFunc(AUX_z, increaseRotorRotation);
	auxKeyFunc(AUX_X, decreaseRotorRotation);
	auxKeyFunc(AUX_x, decreaseRotorRotation);

	auxKeyFunc(AUX_A, increaseBladeRotation);
	auxKeyFunc(AUX_a, increaseBladeRotation);
	auxKeyFunc(AUX_D, decreaseBladeRotation);
	auxKeyFunc(AUX_d, decreaseBladeRotation);

	auxKeyFunc(AUX_Q, increaseDebugRotation);
	auxKeyFunc(AUX_q, increaseDebugRotation);
	auxKeyFunc(AUX_E, decreaseDebugRotation);
	auxKeyFunc(AUX_e, decreaseDebugRotation);

	auxKeyFunc(AUX_W, raisePlane);
	auxKeyFunc(AUX_w, raisePlane);
	auxKeyFunc(AUX_S, lowerPlane); 
	auxKeyFunc(AUX_s, lowerPlane);

	auxKeyFunc(AUX_f, toggleFreezeLap);
	auxKeyFunc(AUX_F, toggleFreezeLap);

	auxKeyFunc(AUX_UP, decreasePitch);
	auxKeyFunc(AUX_DOWN, increasePitch);
	auxKeyFunc(AUX_RIGHT, increaseRoll);
	auxKeyFunc(AUX_LEFT, decreaseRoll);

	auxReshapeFunc(myReshape);
	auxIdleFunc(IdleFunction);
	auxMainLoop(display);
	return(0);
}
#pragma endregion

void CALLBACK display(void)
{
	GLfloat position[] = { lightXAxis, lightYAxis, lightZAxis, 1.0 };
	GLfloat matUmbra[4][4];

	//oricare 3 pct din plan in sens CCW
	GLfloat puncte[3][3] = {
		{ -100.0f, planeElevation + 20, -500.0f },
	{ -500.0f, planeElevation + 20, 500.0f },
	{ 0, planeElevation + 20, 500.0f }
	};
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	
	glRotatef(30, 1, 1, 1);
		MatriceUmbra(puncte, position, matUmbra);

		glPushMatrix();
			glLightfv(GL_LIGHT0, GL_POSITION, position);
			umbra = 0;
			drawAll();
		glPopMatrix();
		
		glPushMatrix();
			glPushAttrib(GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			glPushMatrix();
				glMultMatrixf((GLfloat*)matUmbra);// se inmulteste matricea curenta cu matricea de umbrire
				umbra = 1;
				drawTiltedAndTranslatedPlane();
				drawProps();
			glPopMatrix();
			//deseneaza sursa de lumina ca o sfera mica galbena in pozitia reala
			glPushMatrix();
				glTranslatef(position[0], position[1], position[2]);
				glColor3f(1.0, 0.9, 0);
				glPointSize(5.0);
				auxWireSphere(100);
			glPopMatrix();
			//reseteaza starea variabilelor de iluminare
			glPopAttrib();
		glPopMatrix();

	glPopMatrix();
	glFlush();
}
#pragma endregion
