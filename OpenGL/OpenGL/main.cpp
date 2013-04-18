#include <windows.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>
#include <stdio.h>

int width = 1280;
int height = 720;

const GLdouble ballRadius = 0.2;
const GLint ballSlices = 20;
const GLint ballStacks = 16;
const GLdouble timeStep = 0.01;
const int nrOfBalls = 300;
const int nrOfColors = 6;
const GLfloat diffColors[nrOfColors][4] = { { 0.5, 0, 0.3, 0 }, { 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }, { 0, 1, 1, 0 }, { 1, 1, 0, 0 } };
const GLfloat lowestInitHeight = 2;
const GLfloat highestInitHeight = 7;
const GLfloat initPosLimit = 2; // the balls will appear in parallelopiped defined by (-initPosLimit, lowestInitHeight, -initPosLimit) -> (initPosLimit, highestInitLimit, initPosLimit)
const GLfloat gravityAcc = 9.81;
const GLfloat camMoveStep = 1;

float diffuseLight[4];
int diffX, diffY, mouseMotionType;
int animate = 1, rotate = 1;

GLfloat positions[nrOfBalls][3];
GLfloat velocities[nrOfBalls][3];
int colors[nrOfBalls];
GLfloat rotAngle = 0;

GLfloat sqr(GLfloat x) { return x*x; }

int ballCollision(int ball1, int ball2) {
	GLfloat d = sqrt( sqr(positions[ball2][0] - positions[ball1][0]) + sqr(positions[ball2][1] - positions[ball1][1]) + sqr(positions[ball2][2] - positions[ball1][2]) );
	return (d > 2*ballRadius) ? 0 : 1;
}

GLfloat scalarProduct(GLfloat *vec1, GLfloat *vec2) { return vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2]; }

GLfloat vecModulo(GLfloat *vec) { return sqrt(sqr(vec[0]) + sqr(vec[1]) + sqr(vec[2])); }

void animationStep(void) {

	if(animate) {

	// update velocities according to gravity:
	for(int i=0; i<nrOfBalls; i++)
		velocities[i][1] -= gravityAcc * timeStep;

	// update positions according to velocities:
	for(int i=0; i<nrOfBalls; i++) {
		positions[i][0] += velocities[i][0] * timeStep;
		positions[i][1] += velocities[i][1] * timeStep;
		positions[i][2] += velocities[i][2] * timeStep;

		// check for collision with other balls
		for(int j=0; j<i; j++)
			if(ballCollision(i, j) == 1) {

				// vector between centers of balls (from i to j)
				GLfloat vecBetweenBalls[3] = { positions[j][0]-positions[i][0], positions[j][1]-positions[i][1], positions[j][2]-positions[i][2] };
				GLfloat vecBetweenBallsLen = vecModulo(vecBetweenBalls);
				// normalize it
				vecBetweenBalls[0] = vecBetweenBalls[0] / vecBetweenBallsLen;
				vecBetweenBalls[1] = vecBetweenBalls[1] / vecBetweenBallsLen;
				vecBetweenBalls[2] = vecBetweenBalls[2] / vecBetweenBallsLen;

				// normal component of v1
				GLfloat v1nLen = scalarProduct(velocities[i], vecBetweenBalls);
				GLfloat v1n[3] = { vecBetweenBalls[0] * v1nLen, vecBetweenBalls[1] * v1nLen, vecBetweenBalls[2] * v1nLen };

				// reverse vecBetweenBalls to calculate v2n
				vecBetweenBalls[0] = -vecBetweenBalls[0];
				vecBetweenBalls[1] = -vecBetweenBalls[1];
				vecBetweenBalls[2] = -vecBetweenBalls[2];

				// normal component of v2
				GLfloat v2nLen = scalarProduct(velocities[j], vecBetweenBalls);
				GLfloat v2n[3] = { vecBetweenBalls[0] * v2nLen, vecBetweenBalls[1] * v2nLen, vecBetweenBalls[2] * v2nLen };

				// average of v1n, v2n
				GLfloat v[3] = { v1n[0] + v2n[0], v1n[0] + v2n[0], v1n[0] + v2n[0] };

				GLfloat v1pn[3] = { v[0]-v1n[0], v[1]-v1n[1], v[2]-v1n[2] };
				GLfloat v2pn[3] = { v[0]-v2n[0], v[1]-v2n[1], v[2]-v2n[2] };
				
				velocities[i][0] += v1pn[0] - v1n[0];
				velocities[i][1] += v1pn[1] - v1n[1];
				velocities[i][2] += v1pn[2] - v1n[2];

				velocities[j][0] += v2pn[0] - v2n[0];
				velocities[j][1] += v2pn[1] - v2n[1];
				velocities[j][2] += v2pn[2] - v2n[2];
			}

		// check for collision with floor ( y < ballRadius )
		if(positions[i][1] < ballRadius) {
			positions[i][1] = ballRadius;
			velocities[i][1] *= -0.6;
		}
	}

	}

	if(rotate)
		rotAngle += 0.7;

	glutPostRedisplay();
}

void drawScene(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
//  glutSolidSphere(ballRadius, ballSlices, ballStacks);
  glPushMatrix();
  glRotatef(rotAngle, 0, 1, 0);

  glPushMatrix();
  glScalef(10, 0.1, 10);
  glutSolidCube(1);
  glPopMatrix();
  glPushMatrix();
  glTranslatef(0, -0.1, 0);
  glScalef(100, 0.1, 100);
  glutSolidCube(1);
  glPopMatrix();

  for(int i=0; i<nrOfBalls; i++) {
	glPushMatrix();
	glTranslatef(positions[i][0], positions[i][1], positions[i][2]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffColors[colors[i]]);
	glutSolidSphere(ballRadius/2.0, ballSlices, ballStacks);
	glPopMatrix();
  }

  glPopMatrix(); 

  glFlush();
  glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
  {
    case 0x1B:
    case 'q':
    case 'Q':
      exit(0);
    break;
    case 'w':
      glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
      glTranslatef(0, 0, camMoveStep);
	  glMatrixMode(GL_MODELVIEW);         // wybierz macierz modeli
    break;
    case 's':
      glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
      glTranslatef(0, 0, -camMoveStep);
	  glMatrixMode(GL_MODELVIEW);         // wybierz macierz modeli
    break;
    case 'a':
      glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
      glTranslatef(camMoveStep, 0, 0);
	  glMatrixMode(GL_MODELVIEW);         // wybierz macierz modeli
    break;
    case 'd':
      glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
      glTranslatef(-camMoveStep, 0, 0);
	  glMatrixMode(GL_MODELVIEW);         // wybierz macierz modeli
    break;
	case 'r':
		rotate = (rotate) ? 0 : 1;
	break;
	case VK_SPACE:
		animate = (animate) ? 0 : 1;
	break;
  }
}

void mouse(int btn, int state, int x, int y)
{
  if (state == GLUT_DOWN)
  {
	  diffX = x;
	  diffY = y;
    if (btn == GLUT_LEFT_BUTTON)
		mouseMotionType = 0;
    else if (btn == GLUT_RIGHT_BUTTON)
		mouseMotionType = 1;
    else
		mouseMotionType = 2;
  }
}

void mouseMove(int x, int y)
{
	switch(mouseMotionType) {
	case 0:
		// move
      glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
      glTranslatef((x-diffX)/10.0, (diffY-y)/10.0, 0);
	  glMatrixMode(GL_MODELVIEW);         // wybierz macierz modeli
	break;
	case 1:
		// rotate
      glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
      glRotatef(1, y-diffY, 0, x-diffX);
	  glMatrixMode(GL_MODELVIEW);         // wybierz macierz modeli
	break;
	}
	diffX = x;
	diffY = y;
}

int main(int argc, char **argv)
{
	srand(time( 0 ));

	/* Init all balls etc */
	for(int i=0; i<nrOfBalls; i++) {
		bool collides = true;
		while(collides) {
			collides = false;
			positions[i][0] = ((GLfloat)rand() / RAND_MAX - 0.5) * 2 * (initPosLimit - ballRadius);
			positions[i][1] = (GLfloat)rand() / RAND_MAX * (highestInitHeight - lowestInitHeight) + lowestInitHeight;
			positions[i][2] = ((GLfloat)rand() / RAND_MAX - 0.5) * 2 * (initPosLimit - ballRadius);

			for(int j=0; j<i; j++)
				if(ballCollision(i, j) == 1) {
					collides = true;
					j = i;
				}
		}

		velocities[i][0] = velocities[i][1] = velocities[i][2] = 0;

		colors[i] = (int)((float)rand() / RAND_MAX * nrOfColors);
	}

	/* Init renderer */
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

  // *** for windowed mode
  glutInitWindowSize(width, height);
  glutCreateWindow("Bouncing balls animation");

  // *** for full screen:
/*  char modeString[50];
  sprintf(modeString, "%dx%d:32@60", width, height);
  glutGameModeString( modeString );
  glutEnterGameMode();
*/
  glutMouseFunc(mouse);
  glutMotionFunc(mouseMove);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(drawScene);
  glutIdleFunc(animationStep);

  glMatrixMode(GL_PROJECTION);         // wybierz macierz projekcji
  glLoadIdentity();         // zresetuj j¹ 
  gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f); // oblicz perspektywê dla okna 
  gluLookAt(0, 10, 15, 0, 5, 0, 0, 1, 0);
  glMatrixMode(GL_MODELVIEW);         // wybierz macier modeli
  glLoadIdentity();         // zresetuj j¹ 
  glClearColor(0.31, 0.59, 1.0, 0.0);
  
  glClearDepth(1.0f);
  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  
  glEnable(GL_LIGHTING);
  glShadeModel(GL_SMOOTH);
  GLfloat positionLight[] = { 2, 2, 4, 1 };
  glLightfv(GL_LIGHT0, GL_POSITION, positionLight);
  glEnable(GL_LIGHT0);

  glutMainLoop();
  return 0;
}
