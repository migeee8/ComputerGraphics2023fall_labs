
/* Copyright (c) Mark J. Kilgard, 1994. */


#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <GL/glut.h>

#define TORUS 0
#define TEAPOT 1
#define DOD 2
#define TET 3
#define ISO 4
#define QUIT 5

static int spin = 0;
static int obj = TORUS;
static int begin;

void
menu_select(int item)
{
  if (item == QUIT)
    exit(0);
  obj = item;
  glutPostRedisplay();
}

/* ARGSUSED2 */
void
movelight(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    begin = x;
  }
}

/* ARGSUSED1 */
void
motion(int x, int y)
{
  spin = (spin + (x - begin)) % 360;
  begin = x;
  glutPostRedisplay();
}

void
myinit(void)
{
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);

  glDepthFunc(GL_LESS);
 // glDepthFunc(GL_GREATER);
 //  glDisable(GL_DEPTH_TEST);
 glEnable(GL_DEPTH_TEST);
}

/*  Here is where the light position is reset after the modeling
 *  transformation (glRotated) is called.  This places the 
 *  light at a new position in world coordinates.  The cube
 *  represents the position of the light.
 */
void
display(void)
{
  GLfloat position[] =
  {0.0, 0.0, 1.5, 1.0};

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
	  glTranslatef(0.0, 0.0, -5.0);

	  glPushMatrix();
		  glRotated((GLdouble) spin, 0.0, 1.0, 0.0);
		  glLightfv(GL_LIGHT0, GL_POSITION, position);

		  glTranslated(0.0, 0.0, 1.5);
		  glDisable(GL_LIGHTING);
		  glColor3f(0.0, 1.0, 1.0);
		  glutWireCube(0.1);
		  glEnable(GL_LIGHTING);
	  glPopMatrix();

	  switch (obj) {
	  case TORUS:
		glutSolidTorus(0.275, 0.85, 20, 20);
		break;
	  case TEAPOT:
		glutSolidTeapot(1.0);
		break;
	  case DOD:
		glPushMatrix();
		glScalef(.5, .5, .5);
		glutSolidDodecahedron();
		glPopMatrix();
		break;
	  case TET:
		glutSolidTetrahedron();
		break;
	  case ISO:
		glutSolidIcosahedron();
		break;
	  }

  glPopMatrix();
  glutSwapBuffers();
}

void
myReshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLfloat) w / (GLfloat) h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
}


/*  Main Loop
 *  Open window with initial window size, title bar, 
 *  RGBA display mode, and handle input events.
 */
int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutCreateWindow(argv[0]);
  myinit();
  glutMouseFunc(movelight);
  glutMotionFunc(motion);
  glutReshapeFunc(myReshape);
  glutDisplayFunc(display);


  glutCreateMenu(menu_select);
  glutAddMenuEntry("Torus", TORUS);
  glutAddMenuEntry("Teapot", TEAPOT);
  glutAddMenuEntry("Dodecahedron", DOD);
  glutAddMenuEntry("Tetrahedron", TET);
  glutAddMenuEntry("Icosahedron", ISO);
  glutAddMenuEntry("Quit", QUIT);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  glutMainLoop();
  return 0;             /* ANSI C requires main to return int. */
}
